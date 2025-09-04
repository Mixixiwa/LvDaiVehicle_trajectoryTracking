#pragma once
#include <windows.h>
#include <string>
#include <queue>
#include <mutex>
#include <optional>


class SerialPort {
public:
    SerialPort(const std::string& portName, DWORD baudRate = CBR_115200);
    ~SerialPort();
    bool open();
    void close();
    std::string readLine();
    bool isOpen() const;

private:
    std::string portName;
    HANDLE hSerial;
    DWORD baudRate;
};

struct GPSData {
    std::string timestamp;
    double Longitude;  //经度 度  小数点后八位
    double Latitude;   //维度  度  小数点后八位
    double Alt;  //高度，米  小数点后四位
    double Heading;    ////地面航向，从北向起顺时针计算  0~360°  小数点后两位
    double Pitch;  //俯仰角  -90°~90° 小数点后两位
    double Track; //速度角 度
    double Vel;  //速度  单位：米/秒
    double Roll; //横滚角 度

    //double East;       // 以基站为坐标原点的地理坐标系下，东向位置，单位米
    //double North;        // 以基站为坐标原点的地理坐标系下，北向位置，单位米
    //double Heading;       //地面航向，从北向起顺时针计算
};

class KSXTParser {
public:
    static std::optional<GPSData> parse(const std::string& line);

    static double GPS_X;  //East
    static double GPS_Y;  //North
    static double GPS_Z;  //Up
    static double GPS_V;
    static double GPS_PHI;
};