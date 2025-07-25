#include "PIDController.h"

PIDController::PIDController(double kp, double ki, double kd)
    : Kp(kp), Ki(ki), Kd(kd) {
}

double PIDController::compute(double error, double dt) {
    integral += error * dt;
    double derivative = (error - prev_error) / dt;
    prev_error = error;
    return Kp * error + Ki * integral + Kd * derivative;
}