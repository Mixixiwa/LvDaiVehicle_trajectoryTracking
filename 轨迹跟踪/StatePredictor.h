#pragma once
struct State {
    double x;
    double y;
    double phi;
    double vx;
    double vy;
    double phi_dot;
};

State predictNextState(const State& current, double ts);
