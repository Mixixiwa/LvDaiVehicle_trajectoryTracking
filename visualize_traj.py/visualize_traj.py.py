import matplotlib.pyplot as plt
import numpy as np

# ¶ÁÈ¡CSVÊý¾Ý
data = np.loadtxt("VehicleSimulator.csv", delimiter=",")
x, y = data[:, 0], data[:, 1]

# »­Í¼
plt.figure(figsize=(8, 6))
plt.plot(x, y, label="Tracked Vehicle Path")
plt.xlabel("X Position (m)")
plt.ylabel("Y Position (m)")
plt.title("Tracked Vehicle Trajectory")
plt.axis("equal")
plt.grid(True)
plt.legend()
plt.show()
