#include "VehicleSimulator.h"
#include <cmath>

VehicleSimulator::VehicleSimulator(double track_width)
{
    b = track_width;
}

void VehicleSimulator::step(double v_l, double v_r, double dt) {
    state.v = 0.5 * (v_l + v_r);
    double omega = (v_r - v_l) / b;

    state.x += state.v * std::cos(state.psi) * dt;
    state.y += state.v * std::sin(state.psi) * dt;
    state.psi += omega * dt;
}

const VehicleState& VehicleSimulator::getState() const {
    return state;
}