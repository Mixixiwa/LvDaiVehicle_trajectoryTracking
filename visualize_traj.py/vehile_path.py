import matplotlib
matplotlib.use("TkAgg")   # 或者 "Qt5Agg"
import matplotlib.pyplot as plt
import numpy as np

def quintic_polynomial(start, end, T, num_points=100):
    
    y0, y0_dot, y0_ddot = start
    yT, yT_dot, yT_ddot = end

    # 构造矩阵
    A = np.array([
        [T**3, T**4, T**5],
        [3*T**2, 4*T**3, 5*T**4],
        [6*T, 12*T**2, 20*T**3]
    ])
    b = np.array([
        yT - (y0 + y0_dot*T + 0.5*y0_ddot*T**2),
        yT_dot - (y0_dot + y0_ddot*T),
        yT_ddot - y0_ddot
    ])

    coeffs = np.linalg.solve(A, b)
    a0 = y0
    a1 = y0_dot
    a2 = 0.5 * y0_ddot
    a3, a4, a5 = coeffs

    # 生成轨迹
    t = np.linspace(0, T, num_points)
    y = a0 + a1*t + a2*t**2 + a3*t**3 + a4*t**4 + a5*t**5
    return t, y


def generate_double_lane_change(length=20.0, width=3.0, vx=1.0, dt=0.1):
    
    T_total = length / vx
    t = np.arange(0, T_total+dt, dt)
    x = vx * t

    y = np.zeros_like(x)

    # 时间分段
    t1 = 5 / vx
    t2 = 15 / vx

    # 索引
    idx1 = np.where(x <= 5)[0]
    idx2 = np.where((x > 5) & (x <= 15))[0]
    idx3 = np.where(x > 15)[0]

    # 0–5 m: 直线
    y[idx1] = 0

    # 5–15 m: 五次多项式
    T_poly = t2 - t1
    _, y_poly = quintic_polynomial([0, 0, 0], [width, 0, 0],
                                   T=T_poly, num_points=len(idx2))
    y[idx2] = y_poly

    # 15–20 m: 延续 y=width
    y[idx3] = width

    return x, y


if __name__ == "__main__":
    x, y = generate_double_lane_change(length=20.0, width=3.0, vx=1.0, dt=0.1)

    # 保存 CSV
    data = np.column_stack((x, y))
    np.savetxt("double_lane_change_continuous.csv", data, delimiter=",",
               header="x,y", comments='', fmt="%.6f")

    plt.figure(figsize=(8, 4))
    plt.plot(x, y, label="Lane Change (Continuous at 15m)", linewidth=2)
    plt.axhline(0, color="gray", linestyle="--", linewidth=0.8)
    plt.xlabel("X (m)")
    plt.ylabel("Y (m)")
    plt.title("Lane Change with Fixed Velocity (Continuous at 15m)")
    plt.legend()
    plt.axis("equal")
    plt.grid(True)
    plt.show()
