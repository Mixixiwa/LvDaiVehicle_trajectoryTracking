#pragma once

class PIDController {
public:
    PIDController(double kp, double ki, double kd);

    double compute(double error, double dt);

private:
    double Kp, Ki, Kd;
    double prev_error = 0.0;
    double integral = 0.0;
};