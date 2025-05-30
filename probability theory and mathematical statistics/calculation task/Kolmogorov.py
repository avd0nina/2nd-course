import numpy as np

# Данные выборки
data = np.array([
    0.191, 0.055, 0.733, 0.577, 0.677, 0.944, 0.123, 0.532, 0.874, 0.935,
    0.655, 0.787, 0.77, 0.768, 0.768, 0.051, 0.234, 0.124, 0.31, 0.451,
    0.111, 0.04, 0.253, 0.554, 0.529, 0.37, 0.706, 0.879, 0.649, 0.483
])

# Сортировка выборки
data_sorted = np.sort(data)
n = len(data_sorted)

# ЭФР: F_n^*(X_{(i)}) = i/n
F_n = np.arange(1, n+1) / n

# Теоретическая ФР для U[0,1]: F_0(t) = t
F_0 = data_sorted  # Для U[0,1], F_0(X_{(i)}) = X_{(i)}

# Вычисление D_n
D_n = 0
for i in range(n):
    F_0_i = F_0[i]  # X_{(i)}
    F_n_i = i / n   # F_n^*(X_{(i)})
    F_n_prev = (i-1) / n if i > 0 else 0  # F_n^* перед X_{(i)}
    diff1 = abs(F_0_i - F_n_i)
    diff2 = abs(F_0_i - F_n_prev)
    D_n = max(D_n, diff1, diff2)

# Статистика Колмогорова
T_n = np.sqrt(n) * D_n

# Достигаемый уровень значимости
terms = 10  # Количество слагаемых для точности
alpha_star = 0
for k in range(1, terms + 1):
    alpha_star += (-1)**(k+1) * np.exp(-2 * k**2 * T_n**2)
alpha_star *= 2

# Вывод результатов
epsilon = 0.14
print(f"D_n = {D_n:.4f}")
print(f"T_n = {T_n:.4f}")
print(f"Достигаемый уровень значимости alpha^* = {alpha_star:.4f}")
print(f"Уровень значимости epsilon = {epsilon}")
if alpha_star <= epsilon:
    print("Отвергаем H_0: распределение не является U[0,1]")
else:
    print("Не отвергаем H_0: данные согласуются с U[0,1]")