#pragma once
#include <vector>
#include <cmath>
#include <algorithm>
#include <iostream>

//计算路径切线方向与X轴的夹角和曲率
// 关键是角度的多值问题

void computePathHeadingAndKappa(
    const std::vector<std::pair<double, double>>& path,
    std::vector<double>& path_heading,
    std::vector<double>& path_kappa
);