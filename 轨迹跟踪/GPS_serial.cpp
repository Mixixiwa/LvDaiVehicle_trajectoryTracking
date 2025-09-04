#include "GPS_serial.h"
#include <iostream>
#include <sstream>
#include <vector>
#include <cmath>
#include<iomanip>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

//GPS串口部分
SerialPort::SerialPort(const std::string& port, DWORD baud)
    : portName(port), baudRate(baud), hSerial(INVALID_HANDLE_VALUE) {
}

SerialPort::~SerialPort() {
    close();
}

bool SerialPort::open() {
    hSerial = CreateFileA(portName.c_str(), GENERIC_READ, 0, nullptr,
        OPEN_EXISTING, 0, nullptr);

    if (hSerial == INVALID_HANDLE_VALUE) {
        std::cerr << "Failed to open port: " << portName << "\n";
        return false;
    }

    DCB dcb = { 0 };
    dcb.DCBlength = sizeof(DCB);
    if (!GetCommState(hSerial, &dcb)) return false;

    dcb.BaudRate = baudRate;
    dcb.ByteSize = 8;
    dcb.Parity = NOPARITY;
    dcb.StopBits = ONESTOPBIT;

    if (!SetCommState(hSerial, &dcb)) return false;

    COMMTIMEOUTS timeouts = { 50, 0, 50, 0, 0 };
    SetCommTimeouts(hSerial, &timeouts);

    return true;
}

void SerialPort::close() {
    if (hSerial != INVALID_HANDLE_VALUE) {
        CloseHandle(hSerial);
        hSerial = INVALID_HANDLE_VALUE;
    }
}

bool SerialPort::isOpen() const {
    return hSerial != INVALID_HANDLE_VALUE;
}

std::string SerialPort::readLine() {
    char ch;
    DWORD bytesRead;
    std::string result;

    while (true) {
        if (!ReadFile(hSerial, &ch, 1, &bytesRead, nullptr) || bytesRead == 0)
            break;
        if (ch == '\n') break;
        result += ch;
    }

    return result;
}

//GPS  坐标转换部分

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

//GPS串口信息解析，并完成坐标转换
static std::vector<std::string> split(const std::string& str, char delim) {
    std::stringstream ss(str);
    std::string item;
    std::vector<std::string> elems;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}

std::optional<GPSData> base_point;
double KSXTParser::GPS_X = 0.0;
double KSXTParser::GPS_Y = 0.0;
double KSXTParser::GPS_Z = 0.0;
double KSXTParser::GPS_V = 0.0;
double KSXTParser::GPS_PHI = 0.0;
std::optional<GPSData> KSXTParser::parse(const std::string& line) {
    if (line.rfind("$KSXT", 0) != 0) return std::nullopt;

    auto tokens = split(line, ',');
    /*if (tokens.size() != 22) {
        std::cerr << "[错误] 字段数量应为22，当前为：" << tokens.size() << std::endl;
        return std::nullopt;
    }*/

    try {
        GPSData data;
        data.timestamp = tokens[1];
        data.Longitude = std::round(std::stod(tokens[2]) * 1e8) / 1e8;  //将一个浮点数保留 小数点后八位，并赋值给变量
        data.Latitude = std::round(std::stod(tokens[3]) * 1e8) / 1e8;
        data.Alt = std::round(std::stod(tokens[4]) * 1e4) / 1e4;
        data.Heading = std::round(std::stod(tokens[5]) * 1e2) / 1e2;
        data.Pitch = std::round(std::stod(tokens[6]) * 1e2) / 1e2;
        data.Track = std::round(std::stod(tokens[7]) * 1e2) / 1e2;
        data.Vel = std::round(std::stod(tokens[8]) * 1e3) / 1e3;
        data.Roll = std::round(std::stod(tokens[9]) * 1e2) / 1e2;

        // 处理最后一个字段，提取数值和校验和
        /*size_t pos = tokens[21].find('*');
        if (pos != std::string::npos) {
            data.gyro_bias_y = std::stod(tokens[21].substr(0, pos));
            data.checksum = tokens[21].substr(pos + 1);
        }
        else {
            data.gyro_bias_y = std::stod(tokens[21]);
            data.checksum = "";
        }*/

        //将GPS经纬度转化为坐标
        // 当前 GPS 坐标
        double lon = data.Longitude;  // 经度
        double lat = data.Latitude;   // 纬度
        double alt = data.Alt;       // 高度

        // 基准点（如起点或地图中心）

        if (!base_point.has_value())
        {
            base_point = data;  // 保存第一次接收到的 GPS 数据
            std::cout << std::fixed << std::setprecision(8) << "保存基准点: 经度=" << base_point->Longitude
                << ", 纬度=" << base_point->Latitude << std::endl;
        }

        double east, north, up;
        lla_to_enu(data.Latitude, data.Longitude, data.Alt, base_point->Latitude, base_point->Longitude, base_point->Alt, east, north, up);

        GPS_X = east;
        GPS_Y = north;
        GPS_Z = up;
        GPS_V= data.Vel;
        GPS_PHI= data.Heading;

        return data;
    }
    catch (const std::exception& e) {
        std::cerr << "解析异常：" << e.what() << std::endl;
        return std::nullopt;
    }
}