#pragma once
#include <vector>

//��ͶӰ���ͨ��ģ��,��������x_set,y_set��xy��ͶӰ
//Ѱ��ƥ��㣬����ʵ������͹滮����
//���ƥ���ı�ţ������ƥ��㲻֪һ������ͶӰ�������꣬����ǣ�����

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

