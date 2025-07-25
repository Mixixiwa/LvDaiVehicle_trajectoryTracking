import matplotlib.pyplot as plt
import numpy as np
import time

path = [(x, np.sin(0.5*x)*3) for x in np.linspace(0, 10, 1000)]
np.savetxt("path.csv", path, delimiter=",")
time.sleep(0.5)
# ��ȡCSV����
data = np.loadtxt("path.csv", delimiter=",")
x,y = data[:, 0], data[:, 1]


# ��ͼ
plt.figure(figsize=(8, 6))
# ����Ŀ��켣
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