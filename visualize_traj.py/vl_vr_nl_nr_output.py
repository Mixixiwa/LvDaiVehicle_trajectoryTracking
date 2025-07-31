import matplotlib.pyplot as plt
import numpy as np

data = np.loadtxt("vl_vr_nl_nr_output.csv", delimiter=",")
x = data[:, 0]
y1 = data[:, 1]
y2 = data[:,2]
y3 = data[:,3]
y4 = data[:,4]


# 画图
plt.figure(figsize=(8, 6))
# 绘制目标轨迹
plt.plot(x,y1, 'r-', label='vl_output')
plt.plot(x, y2, 'b-', label='vr_output')   # 第二条蓝色实线
plt.plot(x, y3, 'g-', label='nl_output')
plt.plot(x, y4, 'k-', label='nr_output')

# 设置横轴和纵轴的范围
# plt.xlim(0, 500)      # 横坐标从 0 到 500
plt.ylim(0, 50)     # 纵坐标从 0 到 100
plt.xlabel("X Time (s)")
plt.ylabel("Y  (m)")
plt.title("vl_vr_nl_nr_output")
#plt.axis("equal")  #画的是二维轨迹（非时间序列），建议关闭 axis("equal")   
plt.grid(True)
plt.legend()
plt.tight_layout()
plt.show()
