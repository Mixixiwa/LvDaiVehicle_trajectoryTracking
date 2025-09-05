#include "GPS_serial.h"
#include <iostream>
#include <sstream>
#include <vector>
#include <iomanip>
#include <cmath>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

//GPS  坐标转换

// 地球参数（WGS84）单位：米
constexpr double a = 6378137.0;               // 长半轴 单位：米
constexpr double f = 1.0 / 298.257223563;     // 扁率
constexpr double b = a * (1 - f);             // 短半轴
constexpr double e_sq = f * (2 - f);          // 第一偏心率平方

// π/180
constexpr double deg2rad = M_PI / 180.0;

// 将经纬度高度转换为 ECEF 坐标  单位：米
void lla_to_ecef(double lat, double lon, double alt, double& x, double& y, double& z) {
    lat *= deg2rad;
    lon *= deg2rad;

    double sin_lat = sin(lat);
    double cos_lat = cos(lat);
    double sin_lon = sin(lon);
    double cos_lon = cos(lon);

    double N = a / sqrt(1.0 - e_sq * sin_lat * sin_lat);

    x = (N + alt) * cos_lat * cos_lon;
    y = (N + alt) * cos_lat * sin_lon;
    z = (N * (1 - e_sq) + alt) * sin_lat;
}

// 将 ECEF 转换为 ENU（以 refLat, refLon, refAlt 为原点）
void ecef_to_enu(double x, double y, double z,
    double x_ref, double y_ref, double z_ref,
    double lat_ref, double lon_ref,
    double& east, double& north, double& up) {
    lat_ref *= deg2rad;
    lon_ref *= deg2rad;

    double sin_lat = sin(lat_ref);
    double cos_lat = cos(lat_ref);
    double sin_lon = sin(lon_ref);
    double cos_lon = cos(lon_ref);

    double dx = x - x_ref;
    double dy = y - y_ref;
    double dz = z - z_ref;

    east = -sin_lon * dx + cos_lon * dy;
    north = -cos_lon * sin_lat * dx - sin_lon * sin_lat * dy + cos_lat * dz;
    up = cos_lon * cos_lat * dx + sin_lon * cos_lat * dy + sin_lat * dz;
}

// 整合函数：直接从 WGS84 → ENU  单位：米  
// 输入坐标:纬度(lat) : 度(°)，经度(lon) : 度(°)，高度(alt) : 米(m)
//输出坐标:East(东) : 米(m)，North(北) : 米(m)，Up(上) : 米(m)
void lla_to_enu(double lat, double lon, double alt,
    double ref_lat, double ref_lon, double ref_alt,
    double& east, double& north, double& up) {
    double x, y, z;
    double x_ref, y_ref, z_ref;

    lla_to_ecef(lat, lon, alt, x, y, z);
    lla_to_ecef(ref_lat, ref_lon, ref_alt, x_ref, y_ref, z_ref);
    ecef_to_enu(x, y, z, x_ref, y_ref, z_ref, ref_lat, ref_lon, east, north, up);
}

// ---------------- GPSReceiver ----------------

GPSReceiver::GPSReceiver(const std::string& portName, unsigned int baudRate)
    : portName_(portName), baudRate_(baudRate), running_(false), refSet_(false), hSerial_(INVALID_HANDLE_VALUE) {
    csvFile_.open("gps_log.csv", std::ios::out);
    csvFile_ << "Time,X,Y,Altitude,Speed,Heading\n";
}

GPSReceiver::~GPSReceiver() {
    stop();
    if (csvFile_.is_open()) {
        csvFile_.close();
    }
    if (hSerial_ != INVALID_HANDLE_VALUE) {
        CloseHandle(hSerial_);
    }
}

// 启动接收线程
void GPSReceiver::start() {
    running_ = true;
    recvThread_ = std::thread(&GPSReceiver::serialReceiver, this);
}

// 停止接收线程
void GPSReceiver::stop() {
    running_ = false;
    if (recvThread_.joinable()) {
        recvThread_.join();
    }
}

// 串口接收线程
void GPSReceiver::serialReceiver() {
    // 打开串口
    std::wstring wport(portName_.begin(), portName_.end());
    hSerial_ = CreateFileW(wport.c_str(), GENERIC_READ | GENERIC_WRITE, 0, nullptr,
        OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

    if (hSerial_ == INVALID_HANDLE_VALUE) {
        std::cerr << "无法打开串口 " << portName_ << std::endl;
        return;
    }

    // 配置串口参数
    DCB dcbSerialParams = { 0 };
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
    if (!GetCommState(hSerial_, &dcbSerialParams)) {
        std::cerr << "获取串口配置失败！" << std::endl;
        return;
    }

    dcbSerialParams.BaudRate = baudRate_;
    dcbSerialParams.ByteSize = 8;
    dcbSerialParams.StopBits = ONESTOPBIT;
    dcbSerialParams.Parity = NOPARITY;

    if (!SetCommState(hSerial_, &dcbSerialParams)) {
        std::cerr << "设置串口参数失败！" << std::endl;
        return;
    }

    // 设置串口超时
    COMMTIMEOUTS timeouts = { 0 };
    timeouts.ReadIntervalTimeout = 50;
    timeouts.ReadTotalTimeoutConstant = 50;
    timeouts.ReadTotalTimeoutMultiplier = 10;
    SetCommTimeouts(hSerial_, &timeouts);

    // 接收缓冲
    char buffer[256];
    std::string recvBuffer;

    while (running_) {
        DWORD bytesRead;
        if (ReadFile(hSerial_, buffer, sizeof(buffer), &bytesRead, nullptr)) {
            recvBuffer.append(buffer, bytesRead);

            size_t pos;
            while ((pos = recvBuffer.find('\n')) != std::string::npos) {
                std::string frame = recvBuffer.substr(0, pos);
                recvBuffer.erase(0, pos + 1);

                std::lock_guard<std::mutex> lock(queueMutex_);
                frameQueue_.push(frame);
            }
        }
    }
}

// 校验和
bool GPSReceiver::verifyChecksum(const std::string& frame) {
    size_t starPos = frame.find('*');
    if (starPos == std::string::npos) return false;

    unsigned char checksum = 0;
    for (size_t i = 1; i < starPos; ++i) {
        checksum ^= static_cast<unsigned char>(frame[i]);
    }

    std::string hexStr = frame.substr(starPos + 1, 2);
    unsigned int receivedChecksum;
    std::stringstream ss;
    ss << std::hex << hexStr;
    ss >> receivedChecksum;

    return checksum == receivedChecksum;
}

// 处理一帧
void GPSReceiver::processFrame() {
    std::string frame;
    {
        std::lock_guard<std::mutex> lock(queueMutex_);
        if (frameQueue_.empty()) return;
        frame = frameQueue_.front();
        frameQueue_.pop();
    }
    parseFrame(frame);
}

// 解析一帧
void GPSReceiver::parseFrame(const std::string& frame) {
    if (!verifyChecksum(frame)) {
        std::cerr << "校验和错误: " << frame << std::endl;
        return;
    }

    std::stringstream ss(frame);
    std::string token;
    std::vector<std::string> fields;

    while (std::getline(ss, token, ',')) {
        size_t starPos = token.find('*');
        if (starPos != std::string::npos) {
            token = token.substr(0, starPos);
        }
        fields.push_back(token);
    }

    if (fields.size() >= 7) {
        GPSData data;
        data.time = fields[1];
        data.longitude = std::stod(fields[2]);
        data.latitude = std::stod(fields[3]);
        data.altitude = std::stod(fields[4]);
        data.speed = std::stod(fields[5]);
        data.heading = std::stod(fields[6]);

        convertToXY(data);

        /*std::cout << "Time: " << data.time
            << " X: " << data.x
            << " Y: " << data.y
            << " Alt: " << data.altitude
            << " Spd: " << data.speed
            << " Head: " << data.heading << std::endl;*/

        saveToCSV(data);

        // 保存最新数据
        {
            std::lock_guard<std::mutex> lock(dataMutex_);
            latestData_ = data;
        }
    }
}

// 经纬度转平面坐标
void GPSReceiver::convertToXY(GPSData& data) {

    if (!refSet_) {
        refLon_ = data.longitude;
        refLat_ = data.latitude;
        refAlt_ = data.altitude;
        refSet_ = true;
    }
    double east, north, up;
    lla_to_enu(data.latitude, data.longitude, data.altitude, refLat_, refLon_, refAlt_, east, north, up);


    data.x = east;
    data.y = north;
}

// 保存到CSV
void GPSReceiver::saveToCSV(const GPSData& data) {
    if (csvFile_.is_open()) {
        csvFile_ << data.time << ","
            << std::fixed << std::setprecision(3) << data.x << ","
            << data.y << ","
            << data.altitude << ","
            << data.speed << ","
            << data.heading << "\n";
    }
}

// 主函数获取最新数据
bool GPSReceiver::getLatestData(GPSData& data) {
    std::lock_guard<std::mutex> lock(dataMutex_);
    if (!refSet_) return false;
    data = latestData_;
    return true;
}