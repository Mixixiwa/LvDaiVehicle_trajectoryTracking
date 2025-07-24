#pragma once
#include <array>

    // ���룺״̬��x, y, phi, vx, vy, phi_dot����ο�·����Ϣ��xr, yr, thetar, kappar��
    // �����err������Ϊ4����es��s_dot
  void computeErrors(
        double x, double y, double phi,
        double vx, double vy, double phi_dot,
        double xr, double yr, double thetar, double kappar,
        std::array<double, 4>& err,
        double& es,
        double& s_dot
    );
