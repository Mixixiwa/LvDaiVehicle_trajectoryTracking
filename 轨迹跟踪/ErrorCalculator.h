#pragma once
#include <array>

    // 输入：状态（x, y, phi, vx, vy, phi_dot）与参考路径信息（xr, yr, thetar, kappar）
    // 输出：err（长度为4），es，s_dot
  void computeErrors(
        double x, double y, double phi,
        double vx, double vy, double phi_dot,
        double xr, double yr, double thetar, double kappar,
        std::array<double, 4>& err,
        double& es,
        double& s_dot
    );
