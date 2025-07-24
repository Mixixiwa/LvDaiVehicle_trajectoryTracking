#include "VehicleSimulator.h"
#include <cmath>

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

const VehicleState& VehicleSimulator::getState() const {
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