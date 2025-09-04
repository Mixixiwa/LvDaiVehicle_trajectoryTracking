import matplotlib.pyplot as plt
import numpy as np

data = np.loadtxt("path_heading_kappa_output.csv", delimiter=",")
x = data[:, 0]
y1 = data[:, 1]
y2 = data[:,2]
y3 = data[:,3]


# 画图
plt.figure(figsize=(8, 6))
# 绘制目标轨迹
plt.plot(x,y1, 'r-', label='path')
plt.plot(x, y2, 'b-', label='path_heading')   # 第二条蓝色实线
plt.plot(x, y3, 'g-', label='path_kappa')

# 设置横轴和纵轴的范围
plt.xlim(0, 200)      # 横坐标从 0 到 200
plt.ylim(-2, 4)     # 纵坐标从 0 到 100
plt.xlabel("X Position(m)")
plt.ylabel("Y (m)")
plt.title("path_heading_kappa_output")
#plt.axis("equal")  画的是二维轨迹（非时间序列），建议关闭 axis("equal")   
plt.grid(True)
plt.legend()
plt.tight_layout()
plt.show()
