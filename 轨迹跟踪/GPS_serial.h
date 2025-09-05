#ifndef GPS_RECEIVER_H
#define GPS_RECEIVER_H

#include <string>
#include <queue>
#include <mutex>
#include <thread>
#include <atomic>
#include <fstream>
#include <windows.h>

// GPS 数据结构体
struct GPSData {
    std::string time;   // 时间戳
    double longitude;   // 经度
    double latitude;    // 纬度
    double altitude;    // 高度
    double speed;       // 速度
    double heading;     // 航向角
    double x;           // 转换后的平面坐标X
    double y;           // 转换后的平面坐标Y
};

class GPSReceiver {
public:
    GPSReceiver(const std::string& portName, unsigned int baudRate);
    ~GPSReceiver();

    void start();   // 启动接收线程
    void stop();    // 停止接收线程
    void processFrame();  // 处理并解析一帧

    // 主函数获取最新 GPS 数据
    bool getLatestData(GPSData& data);

private:
    std::string portName_;
    unsigned int baudRate_;
    HANDLE hSerial_;   // 串口句柄
    std::queue<std::string> frameQueue_;
    std::mutex queueMutex_;
    std::atomic<bool> running_;
    std::thread recvThread_;
    std::ofstream csvFile_;


    bool refSet_;       // 参考点是否已设置
    double refLon_;     // 参考点经度
    double refLat_;     // 参考点纬度
    double refAlt_;     //参考点高度

    std::mutex dataMutex_;    // 保护 latestData_
    GPSData latestData_;      // 最新解析的数据

    void serialReceiver();         // 串口接收线程
    void parseFrame(const std::string& frame); // 解析一帧
    bool verifyChecksum(const std::string& frame); // 校验和
    void saveToCSV(const GPSData& data); // 写入CSV
    void convertToXY(GPSData& data); // 经纬度转平面坐标
};

#endif // GPS_RECEIVER_H
#pragma once
