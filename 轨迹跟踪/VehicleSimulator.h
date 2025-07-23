#pragma once

struct VehicleState 
{
    double x = 0.0;      // λ��x
    double y = 0.0;      // λ��y
    double psi = 0.0;    // �����(����)
    double v = 0.0;      // �����������ٶ�
    double r = 0.0;      // ������ٶ�
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