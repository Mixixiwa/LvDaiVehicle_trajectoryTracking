#pragma once

struct VehicleState 
{
    double x = 0.0;      // λ��x
    double y = 0.0;      // λ��y
    double phi = 0.0;    // �����(����)
    double v_x = 0.0;      // �����������ٶ�
    double v_y = 0.0;
    double phi_dot = 0.0;      // ������ٶ�
};

class VehicleSimulator 
{
public:
    VehicleSimulator(double track_width);

    void step(double v_l, double v_r, double dt);
    const VehicleState& getState() const;
    double getTrackWidth() const { return b; }

private:
    double b; // �Ĵ����
    VehicleState state;
};

VehicleState predictNextState(const VehicleState& current, double ts);