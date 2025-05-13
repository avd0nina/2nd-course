package ru.nsu.avdonina.model.work.factory;

import ru.nsu.avdonina.model.listeners.CarAssembleListener;
import ru.nsu.avdonina.model.listeners.FactoryTasksListener;
import ru.nsu.avdonina.model.products.Accessory;
import ru.nsu.avdonina.model.products.Body;
import ru.nsu.avdonina.model.products.Car;
import ru.nsu.avdonina.model.products.Engine;
import ru.nsu.avdonina.model.storage.Storage;
import ru.nsu.avdonina.model.work.WorkersCreator;

public class FactoryWorkersCreator extends WorkersCreator<CarAssemblyTask> {
    private final Storage<Body> bodyStorage;
    private final Storage<Engine> engineStorage;
    private final Storage<Accessory> accessoryStorage;
    private final Storage<Car> carStorage;
    private final CarAssembleListener carAssembleListener;
    private final FactoryTasksListener factoryTasksListener;

    public FactoryWorkersCreator(Storage<Body> bodyStorage,
                                 Storage<Engine> engineStorage,
                                 Storage<Accessory> accessoryStorage,
                                 Storage<Car> carStorage,
                                 CarAssembleListener carAssembleListener,
                                 FactoryTasksListener factoryTasksListener,
                                 int workTime) {
        super(workTime);
        this.bodyStorage = bodyStorage;
        this.engineStorage = engineStorage;
        this.accessoryStorage = accessoryStorage;
        this.carStorage = carStorage;
        this.carAssembleListener = carAssembleListener;
        this.factoryTasksListener = factoryTasksListener;
    }

    @Override
    public CarAssemblyTask createWorker() {
        return new CarAssemblyTask(
                getWorkTime(),
                bodyStorage,
                engineStorage,
                accessoryStorage,
                carStorage,
                carAssembleListener,
                factoryTasksListener
        );
    }
}
