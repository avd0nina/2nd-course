package ru.nsu.avdonina.model.parsing.limits;

public record Limits(
        int minBodySuppliersCount, // Количество потоков поставщиков кузовов
        int maxBodySuppliersCount,
        int minBodySupplyTimeMs, // Время между поставками (в миллисекундах)
        int maxBodySupplyTimeMs,

        int minEngineSuppliersCount, // Поставщики двигателей
        int maxEngineSuppliersCount,
        int minEngineSupplyTimeMs,
        int maxEngineSupplyTimeMs,

        int minAccessorySuppliersCount, // Поставщики аксессуаров
        int maxAccessorySuppliersCount,
        int minAccessorySupplyTimeMs,
        int maxAccessorySupplyTimeMs,

        int minWorkersCount, // Рабочие на сборке автомобилей
        int maxWorkersCount,
        int minCarAssemblingTimeMs,
        int maxCarAssemblingTimeMs,

        int minDealersCount, // Дилеры
        int maxDealersCount,
        int minCarRequestTimeMs,
        int maxCarRequestTimeMs) {
}
