import matplotlib.pyplot as plt
import numpy as np
import time

path = [(x, np.sin(0.5*x)*3) for x in np.linspace(0, 20, 100)]
np.savetxt("path.csv", path, delimiter=",")
time.sleep(0.5)
# 读取CSV数据
data = np.loadtxt("path.csv", delimiter=",")
x,y = data[:, 0], data[:, 1]


# 画图
plt.figure(figsize=(8, 6))
# 绘制目标轨迹
plt.plot(x,y, 'r--', label='Target Path')

plt.scatter(x[0], y[0], c='green', marker='o', label='Start')
plt.xlabel("X Position (m)")
plt.ylabel("Y Position (m)")
plt.title("Tracked Path vs Target Path")
plt.axis("equal")
plt.grid(True)
plt.legend()
plt.tight_layout()
plt.show()