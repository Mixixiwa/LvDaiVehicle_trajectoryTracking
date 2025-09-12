#include "VehicleSimulator.h"
#include <cmath>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

VehicleSimulator::VehicleSimulator(double track_width)
{
    b = track_width;
}

void VehicleSimulator::step(double v_l, double v_r, double dt) {
    double v= 0.5 * (v_l + v_r);
    state.v_x = v * cos(state.phi);
    state.v_y = v * sin(state.phi);
    double omega = (v_r - v_l) / b;

    state.x += state.v_x * dt;
    state.y += state.v_y * dt;
    state.phi += omega * dt;
}

void VehicleSimulator::instae(double x, double y, double heading,double speed)
{
    state.x = x;
    state.y = y;
    // 角度归一化到 [-180, 180]
    // 辅助函数：度->弧度与归一化
    auto deg2rad = [](double deg) { return deg * M_PI / 180.0; };
    auto normalizeAngle = [](double a) {
        while (a > M_PI) a -= 2.0 * M_PI;
        while (a <= -M_PI) a += 2.0 * M_PI;
        return a;
        };
    double heading_rad = deg2rad(heading);

    // 转换到数学角（以 x 轴（东）为 0，逆时针为正）
    // 数学角 theta = pi/2 - bearing_rad
    double phi = M_PI/2 - heading_rad; // M_PI_2 = pi/2，如果未定义，可用 M_PI/2
    
    state.phi = phi;
    state.v_x = speed * cos(state.phi);
    state.v_y = speed * sin(state.phi);
}

VehicleState& VehicleSimulator::getState() {
    return state;
}

VehicleState predictNextState(const VehicleState& current, double ts) {
    VehicleState pred;

    pred.x = current.x + current.v_x * ts * std::cos(current.phi) - current.v_y * ts * std::sin(current.phi);
    pred.y = current.y + current.v_y * ts * std::cos(current.phi) + current.v_x * ts * std::sin(current.phi);
    pred.phi = current.phi;
    pred.v_x = current.v_x;
    pred.v_y = current.v_y;
    pred.phi_dot = current.phi_dot;

    return pred;
}