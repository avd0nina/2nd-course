import numpy as np
import matplotlib.pyplot as plt

# 1) Исходные данные
data = np.array([
    0.191, 0.055, 0.733, 0.577, 0.677, 0.944, 0.123, 0.532, 0.874, 0.935,
    0.655, 0.787, 0.77, 0.768, 0.768, 0.051, 0.234, 0.124, 0.31, 0.451,
    0.111, 0.04, 0.253, 0.554, 0.529, 0.37, 0.706, 0.879, 0.649, 0.483
])

# 2) Вычисление параметров гистограммы
n = len(data)  # Объём выборки: 30
# Число интервалов по формуле Стерджеса
K = int(np.floor(np.log2(n))) + 1  # K = 5
# Минимальное и максимальное значения выборки
x_min, x_max = np.min(data), np.max(data)  # 0.04, 0.944
# Длина интервала
h = (x_max - x_min) / K  # h = 0.1808
# Границы интервалов
bins = np.arange(x_min, x_max + h, h)  # [0.04, 0.2208, 0.4016, 0.5824, 0.7632, 0.944]

# 3) Подсчёт элементов в интервалах
hist, bin_edges = np.histogram(data, bins=bins)
# Число элементов в каждом интервале (нук)
nu_k = hist  # [7, 4, 6, 5, 8]

# 4) Вычисление эмпирической плотности
densities = nu_k / (n * h)  # f_n^*(t) = nu_k / (n * h)

# 5) Построение гистограммы
plt.figure(figsize=(10, 6))
plt.bar(bin_edges[:-1], densities, width=h, align='edge', alpha=0.7, color='green', label='Гистограмма')
plt.grid(True)
plt.title('Гистограмма (эмпирическая плотность)')
plt.xlabel('t')
plt.ylabel('fₙ(t)')
plt.xlim(x_min, x_max)
plt.ylim(0, max(densities) * 1.1)  # Устанавливаем верхнюю границу с запасом
plt.legend()
plt.show()

# 6) Вывод данных для проверки
print("Интервалы и плотности:")
for i in range(K):
    print(f"Интервал {bin_edges[i]:.3f} - {bin_edges[i+1]:.3f}: nu_k = {nu_k[i]}, f_n^*(t) = {densities[i]:.4f}")