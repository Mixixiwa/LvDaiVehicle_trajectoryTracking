import matplotlib.pyplot as plt
import numpy as np

data = np.loadtxt("MatchPoint_index_heading_kappa_output.csv", delimiter=",")
x = data[:, 0]
y1 = data[:, 1]
y2 = data[:,2]
y3 = data[:,3]
y4 = data[:,4]


# 画图
plt.figure(figsize=(8, 6))
# 绘制目标轨迹
plt.plot(x,y1, 'r-', label='target_idx')
plt.plot(x, y2, 'b-', label='x_desire')   # 第二条蓝色实线
plt.plot(x, y3, 'g-', label='thetar_desire')
plt.plot(x, y4, 'k-', label='kappar_desire')

# 设置横轴和纵轴的范围
plt.xlim(0, 500)      # 横坐标从 0 到 500
plt.ylim(-10, 10)     # 纵坐标从 -10 到 10
plt.xlabel("X Time (s)")
plt.ylabel("Y ")
plt.title("MatchPoint_output")
plt.axis("equal")  #画的是二维轨迹（非时间序列），建议关闭 axis("equal")   
plt.grid(True)
plt.legend()
plt.tight_layout()
plt.show()
