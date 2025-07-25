#include "ErrorCalculator.h"
#include <cmath>

void computeErrors(
    double x, double y, double phi,
    double vx, double vy, double phi_dot,
    double xr, double yr, double thetar, double kappar,
    std::array<double, 4>& err,
    double& es,
    double& s_dot
) {
    // �������� tor �ͷ����� nor
    double tor_x = std::cos(thetar);
    double tor_y = std::sin(thetar);
    double nor_x = -std::sin(thetar);
    double nor_y = std::cos(thetar);

    // ���λ������
    double dx = x - xr;
    double dy = y - yr;

    // ͶӰ�����������
    double ed = nor_x * dx + nor_y * dy;
    es = tor_x * dx + tor_y * dy;

    // ͶӰ��ο������
    double projection_point_thetar = thetar + kappar * es;

    // ed_dot = vy * cos(phi - projection_point_thetar) + vx * sin(phi - projection_point_thetar);
    double dphi = phi - projection_point_thetar;
    double ed_dot = vy * std::cos(dphi) + vx * std::sin(dphi);

    // ephi = sin(phi - projection_point_thetar)
    double ephi = std::sin(dphi);

    // ss_dot = vx * cos(dphi) - vy * sin(dphi)
    double ss_dot = vx * std::cos(dphi) - vy * std::sin(dphi);

    s_dot = ss_dot / (1.0 - kappar * ed);

    // ephi_dot = phi_dot - kappar * s_dot
    double ephi_dot = phi_dot - kappar * s_dot;

    // ����������
    err[0] = ed;
    err[1] = ed_dot;
    err[2] = ephi;
    err[3] = ephi_dot;
}