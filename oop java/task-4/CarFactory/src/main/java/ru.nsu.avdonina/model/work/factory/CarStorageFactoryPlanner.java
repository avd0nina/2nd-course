package ru.nsu.avdonina.model.work.factory;

import ru.nsu.avdonina.model.products.Car;
import ru.nsu.avdonina.model.storage.Storage;

public class CarStorageFactoryPlanner extends FactoryPlanner {
    private final Storage<Car> carStorage;

    public CarStorageFactoryPlanner(Factory factory, Storage<Car> carStorage) {
        super(factory);
        this.carStorage = carStorage;
    }

    @Override
    public int getNewTasksCount() {
        return carStorage.getRemainingCapacity() > 0 ? carStorage.getMaxCapacity() : 0; // Если на складе есть свободное место (> 0), то планировщик говорит: "давай произведем машин столько, сколько максимум можно хранить".
    }
}
