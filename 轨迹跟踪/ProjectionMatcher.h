#pragma once
#include <vector>

//找投影点的通用模块,批量计算x_set,y_set中xy的投影
//寻找匹配点，输入实际坐标和规划坐标
//输出匹配点的编号（所需的匹配点不知一个），投影点点的坐标，航向角，曲率

struct ProjectionResult {
    std::vector<int> match_point_index_set;
    std::vector<double> proj_x_set;
    std::vector<double> proj_y_set;
    std::vector<double> proj_heading_set;
    std::vector<double> proj_kappa_set;
};

class ProjectionMatcher {
public:
    ProjectionMatcher(int buffer_size = 128);

    ProjectionResult matchProjection(
        const std::vector<double>& x_set,
        const std::vector<double>& y_set,
        const std::vector<double>& path_x,
        const std::vector<double>& path_y,
        const std::vector<double>& path_heading,
        const std::vector<double>& path_kappa
    );

private:
    bool is_first_run;
    int buffer_size;
    std::vector<int> pre_match_point_index_set;

    void computeProjection(
        int i,
        ProjectionResult& result,
        const std::vector<double>& x_set,
        const std::vector<double>& y_set,
        const std::vector<double>& path_x,
        const std::vector<double>& path_y,
        const std::vector<double>& path_heading,
        const std::vector<double>& path_kappa
    );
};

