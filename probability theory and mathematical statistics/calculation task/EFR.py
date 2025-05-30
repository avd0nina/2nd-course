import numpy as np
import matplotlib.pyplot as plt

def empirical_cdf(data):
    """
    Возвращает:
      x_vals — отсортированные значения выборки,
      F_vals — значения эмпирической функции F_n(x_i) = i/n.
    """
    data_sorted = np.sort(data)
    n = len(data_sorted)
    F_vals = np.arange(1, n+1) / n
    return data_sorted, F_vals

# 1) Исходные данные
data = np.array([
    0.191, 0.055, 0.733, 0.577, 0.677, 0.944, 0.123, 0.532, 0.874, 0.935,
    0.655, 0.787, 0.77, 0.768, 0.768, 0.051, 0.234, 0.124, 0.31, 0.451,
    0.111, 0.04, 0.253, 0.554, 0.529, 0.37, 0.706, 0.879, 0.649, 0.483
])

# 2) Вычисляем ЭФР
x_vals, F_vals = empirical_cdf(data)

# 3) Вывод таблицы (i, X_(i), F_n(X_(i)))
print("i\tX_(i)\tF_n(X_(i))")
for i, (x, F) in enumerate(zip(x_vals, F_vals), start=1):
    print(f"{i:2d}\t{x:.3f}\t{F:.3f}")

# 4) Рисуем шаговую функцию F_n(t)
plt.figure(figsize=(10, 6))
plt.step(x_vals, F_vals, where='post', linewidth=2, color='orange')

# Добавляем вертикальные разрывы
for x, F in zip(x_vals, F_vals):
    plt.vlines(x, F - 1/len(data), F, colors='black')

# Украшаем сеткой "по клеточкам"
plt.xticks(np.linspace(data.min(), data.max(), 11))
plt.yticks(np.linspace(0, 1, 11))
plt.minorticks_on()
plt.grid(which='major', linestyle='-', linewidth=0.8)
plt.grid(which='minor', linestyle=':', linewidth=0.5)

plt.xlim(data.min() - 0.01, data.max() + 0.01)
plt.ylim(0, 1.02)
plt.xlabel('t')
plt.ylabel('Fₙ(t)')
plt.title('Эмпирическая функция распределения Fₙ(t)')
plt.show()