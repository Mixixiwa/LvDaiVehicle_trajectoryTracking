import matplotlib.pyplot as plt
import numpy as np

data = np.loadtxt("path_output.csv", delimiter=",")
x,y = data[:, 0], data[:, 1]


# 画图
plt.figure(figsize=(8, 6))
# 绘制目标轨迹
plt.plot(x,y, 'r--', label='Target Path')

plt.scatter(x[0], y[0], c='green', marker='o', label='Start')
plt.xlim(0, 200)      # 横坐标从 0 到 500
plt.ylim(-2, 5)     # 纵坐标从 -2 到 5
plt.xlabel("X Position (m)")
plt.ylabel("Y Position (m)")
plt.title("Target Path")
#plt.axis("equal")
plt.grid(True)
plt.legend()
plt.tight_layout()
plt.show()
