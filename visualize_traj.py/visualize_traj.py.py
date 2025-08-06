# -*- coding: utf-8 -*-

import matplotlib.pyplot as plt
import numpy as np

# 读取CSV数据
data1 = np.loadtxt("PIDtrajectory_output.csv", delimiter=",")
vehicle_x,vehicle_y = data1[:, 0], data1[:, 1]
data2 = np.loadtxt("Double_lane.csv", delimiter=",")
target_x, target_y= data2[:, 0], data2[:, 1]

# 画图
plt.figure(figsize=(8, 6))
# 绘制目标轨迹
plt.plot(target_x, target_y, 'r--', label='Target Path')

# 绘制车辆轨迹
plt.plot(vehicle_x, vehicle_y, 'b-', label='Vehicle Path')
# 标记起点和终点
plt.scatter(target_x[0], target_y[0], c='green', marker='o', label='Start')
plt.scatter(target_x[-1], target_y[-1], c='red', marker='x', label='Goal')

plt.xlim(0, 200)      # 横坐标从 0 到 200
plt.ylim(-5, 10)     # 纵坐标从 -5 到 10
plt.xlabel("X Position (m)")
plt.ylabel("Y Position (m)")
plt.title("Tracked Path vs Target Path")
#plt.axis("equal")  画的是二维轨迹（非时间序列），建议关闭 axis("equal")
plt.grid(True)
plt.legend()
plt.tight_layout()
plt.show()
