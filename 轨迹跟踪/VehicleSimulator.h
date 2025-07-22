#pragma once

struct VehicleState 
{
    double x = 0.0;      // 位置x
    double y = 0.0;      // 位置y
    double psi = 0.0;    // 航向角(弧度)
    double v = 0.0;      // 车辆中心线速度
    double r = 0.0;      // 航向角速度
};

class VehicleSimulator 
{
public:
    VehicleSimulator(double track_width);

    void step(double v_l, double v_r, double dt);
    const VehicleState& getState() const;

private:
    double b; // 履带间距
    VehicleState state;
};