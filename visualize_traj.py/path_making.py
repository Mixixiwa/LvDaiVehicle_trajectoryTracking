import pandas as pd
import numpy as np

# 创建数据点
x = np.linspace(0, 20, 201)  # X轴从0到20米，201个点
y = np.zeros_like(x)

# 定义双移线路径
# 第一个移线（从右侧进入）
mask1 = (x >= 5) & (x < 7)
y[mask1] = 2 * (x[mask1] - 5) / 2  # 从0到2米

mask2 = (x >= 7) & (x < 9)
y[mask2] = 2  # 保持在2米

mask3 = (x >= 9) & (x < 11)
y[mask3] = 2 - 2 * (x[mask3] - 9) / 2  # 从2米回到0米

# 第二个移线（从左侧进入）
mask4 = (x >= 11) & (x < 13)
y[mask4] = -2 * (x[mask4] - 11) / 2  # 从0到-2米

mask5 = (x >= 13) & (x < 15)
y[mask5] = -2  # 保持在-2米

mask6 = (x >= 15) & (x <= 17)
y[mask6] = -2 + 2 * (x[mask6] - 15) / 2  # 从-2米回到0米

# 创建DataFrame
df = pd.DataFrame({
    'X_position_m': x,
    'Y_position_m': y
})

# 保存为CSV文件
df.to_csv('double_lane_change_path.csv', index=False)

print("双移线行驶工况CSV文件已生成: double_lane_change_path.csv")
print(f"数据点数量: {len(x)}")
print(f"X轴范围: {x.min()} - {x.max()} 米")
print(f"Y轴范围: {y.min()} - {y.max()} 米")
