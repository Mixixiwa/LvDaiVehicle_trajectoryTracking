#include"path_heading_kappa.h"

// 辅助函数：差分
std::vector<double> diff(const std::vector<double>& vec) 
{
    std::vector<double> result;
    for (size_t i = 1; i < vec.size(); ++i) {
        result.push_back(vec[i] - vec[i - 1]);
    }
    return result;
}

// 主函数
void computePathHeadingAndKappa(
    const std::vector<std::pair<double, double>>& path,
    std::vector<double>& path_heading,
    std::vector<double>& path_kappa
) {
    
    std::vector<double> path_x;
    std::vector<double> path_y;
    size_t N = path.size();
    path_x.resize(N);
    path_y.resize(N);

    for (size_t i = 0; i < N; ++i) {
        path_x[i] = path[i].first;
        path_y[i] = path[i].second;
    }

    if (N < 2) return;

    // 1. 计算 dx 和 dy
    std::vector<double> dx = diff(path_x);
    std::vector<double> dy = diff(path_y);

    // 2. 中点欧拉法准备
    std::vector<double> dx_final(N);
    std::vector<double> dy_final(N);

    for (size_t i = 0; i < N; ++i) {
        double dx_pre = (i == 0) ? dx[0] : dx[i - 1];
        double dx_after = (i == N - 1) ? dx[N - 2] : dx[i];
        dx_final[i] = 0.5 * (dx_pre + dx_after);

        double dy_pre = (i == 0) ? dy[0] : dy[i - 1];
        double dy_after = (i == N - 1) ? dy[N - 2] : dy[i];
        dy_final[i] = 0.5 * (dy_pre + dy_after);
    }

    // 3. ds 和 heading
    std::vector<double> ds_final(N);
    path_heading.resize(N);

    for (size_t i = 0; i < N; ++i) {
        ds_final[i] = std::sqrt(dx_final[i] * dx_final[i] + dy_final[i] * dy_final[i]);
        path_heading[i] = std::atan2(dy_final[i], dx_final[i]);
    }

    // 4. dheading
    std::vector<double> dheading = diff(path_heading);
    std::vector<double> dheading_final(N);

    for (size_t i = 0; i < N; ++i) {
        double d_pre = (i == 0) ? dheading[0] : dheading[i - 1];
        double d_after = (i == N - 1) ? dheading[N - 2] : dheading[i];
        dheading_final[i] = 0.5 * (d_pre + d_after);
    }

    // 5. kappa
    path_kappa.resize(N);
    for (size_t i = 0; i < N; ++i) {
        if (ds_final[i] != 0)
            path_kappa[i] = std::sin(dheading_final[i]) / ds_final[i];
        else
            path_kappa[i] = 0.0;
    }
}
