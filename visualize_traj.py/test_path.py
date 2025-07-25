import matplotlib.pyplot as plt
import numpy as np

data = np.loadtxt("path_output.csv", delimiter=",")
x,y = data[:, 0], data[:, 1]


# »­Í¼
plt.figure(figsize=(8, 6))
# »æÖÆÄ¿±ê¹ì¼£
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
