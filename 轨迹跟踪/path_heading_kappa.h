#pragma once
#include <vector>
#include <cmath>
#include <algorithm>
#include <iostream>

//����·�����߷�����X��ļнǺ�����
// �ؼ��ǽǶȵĶ�ֵ����

void computePathHeadingAndKappa(
    const std::vector<std::pair<double, double>>& path,
    std::vector<double>& path_heading,
    std::vector<double>& path_kappa
);