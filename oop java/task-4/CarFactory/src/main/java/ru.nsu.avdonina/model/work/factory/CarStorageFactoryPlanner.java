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
        return carStorage.getRemainingCapacity();
    }
}
