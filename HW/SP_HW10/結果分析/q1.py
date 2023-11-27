import matplotlib.pyplot as plt

# 設定縱軸和橫軸的數值
y_values = list(range(0, 110, 10)) #loss rate
x_values = [300, 500, 800, 1000] #delay(ms)

# customer = 10
line1 = [0.00, 0.00, 0.00, 0.00]

# customer = 100
line2 = [0.07, 0.01, 0.05, 0.01]

# customer = 1000
line3 = [60.17, 43.35, 23.10, 10.17]

# 使用 Matplotlib 畫圖
plt.plot(x_values, line1, label='consumer=10', marker='o')
plt.plot(x_values, line2, label='consumer=100', marker='o')
plt.plot(x_values, line3, label='consumer=1000', marker='o')

# 設定圖的標題和軸的標籤
plt.title('transmission rate and consumer number')
plt.xlabel('dalay(ms)')
plt.ylabel('Loss rate(%)')

# 顯示圖例
plt.legend()

# 顯示圖
plt.show()
