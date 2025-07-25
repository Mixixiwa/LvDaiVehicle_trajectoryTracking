#include "ProjectionMatcher.h"
#include <cmath>
#include <limits>
#include <algorithm>

ProjectionMatcher::ProjectionMatcher(int buffer_size)
    : is_first_run(true), buffer_size(buffer_size) {
}

ProjectionResult ProjectionMatcher::matchProjection(
    const std::vector<double>& x_set,
    const std::vector<double>& y_set,
    const std::vector<double>& path_x,
    const std::vector<double>& path_y,
    const std::vector<double>& path_heading,
    const std::vector<double>& path_kappa
) {
    int input_size = x_set.size();

    ProjectionResult result;
    result.match_point_index_set.assign(buffer_size, -1);
    result.proj_x_set.assign(buffer_size, std::numeric_limits<double>::quiet_NaN());
    result.proj_y_set.assign(buffer_size, std::numeric_limits<double>::quiet_NaN());
    result.proj_heading_set.assign(buffer_size, std::numeric_limits<double>::quiet_NaN());
    result.proj_kappa_set.assign(buffer_size, std::numeric_limits<double>::quiet_NaN());

    if (is_first_run) {
        pre_match_point_index_set.assign(buffer_size, -1);
        for (int i = 0; i < input_size; ++i) {
            int start_search_index = 0;
            double min_distance = std::numeric_limits<double>::infinity();
            int increase_count = 0;

            for (int j = start_search_index; j < path_x.size(); ++j) {
                double dx = path_x[j] - x_set[i];
                double dy = path_y[j] - y_set[i];
                double distance = dx * dx + dy * dy;

                if (distance < min_distance) {
                    min_distance = distance;
                    result.match_point_index_set[i] = j;
                    increase_count = 0;
                }
                else {
                    increase_count++;
                }
                if (increase_count > 50) break;
            }

            computeProjection(i, result, x_set, y_set, path_x, path_y, path_heading, path_kappa);
        }
        pre_match_point_index_set = result.match_point_index_set;
        is_first_run = false;
    }
    else {
        for (int i = 0; i < input_size; ++i) {
            int start_index = pre_match_point_index_set[i];
            if (start_index < 0 || start_index >= path_x.size()) start_index = 0;

            double dx0 = x_set[i] - path_x[start_index];
            double dy0 = y_set[i] - path_y[start_index];
            double heading = path_heading[start_index];
            double dir_x = std::cos(heading);
            double dir_y = std::sin(heading);

            double flag = dx0 * dir_x + dy0 * dir_y;
            int increase_count = 0;
            double min_distance = std::numeric_limits<double>::infinity();

            if (flag > 0) {
                for (int j = start_index; j < path_x.size(); ++j) {
                    double dx = path_x[j] - x_set[i];
                    double dy = path_y[j] - y_set[i];
                    double distance = dx * dx + dy * dy;

                    if (distance < min_distance) {
                        min_distance = distance;
                        result.match_point_index_set[i] = j;
                        increase_count = 0;
                    }
                    else {
                        increase_count++;
                    }
                    if (increase_count > 5) break;
                }
            }
            else {
                for (int j = start_index; j >= 0; --j) {
                    double dx = path_x[j] - x_set[i];
                    double dy = path_y[j] - y_set[i];
                    double distance = dx * dx + dy * dy;

                    if (distance < min_distance) {
                        min_distance = distance;
                        result.match_point_index_set[i] = j;
                        increase_count = 0;
                    }
                    else {
                        increase_count++;
                    }
                    if (increase_count > 5) break;
                }
            }

            computeProjection(i, result, x_set, y_set, path_x, path_y, path_heading, path_kappa);
        }

        pre_match_point_index_set = result.match_point_index_set;
    }

    return result;
}

void ProjectionMatcher::computeProjection(
    int i,
    ProjectionResult& result,
    const std::vector<double>& x_set,
    const std::vector<double>& y_set,
    const std::vector<double>& path_x,
    const std::vector<double>& path_y,
    const std::vector<double>& path_heading,
    const std::vector<double>& path_kappa
) {
    int idx = result.match_point_index_set[i];
    if (idx < 0 || idx >= path_x.size()) return;

    double match_x = path_x[idx];
    double match_y = path_y[idx];
    double heading = path_heading[idx];
    double kappa = path_kappa[idx];

    double dir_x = std::cos(heading);
    double dir_y = std::sin(heading);

    double dx = x_set[i] - match_x;
    double dy = y_set[i] - match_y;

    double ds = dx * dir_x + dy * dir_y;

    double proj_x = match_x + ds * dir_x;
    double proj_y = match_y + ds * dir_y;
    double proj_heading = heading + ds * kappa;
    double proj_kappa = kappa;

    result.proj_x_set[i] = proj_x;
    result.proj_y_set[i] = proj_y;
    result.proj_heading_set[i] = proj_heading;
    result.proj_kappa_set[i] = proj_kappa;
}
