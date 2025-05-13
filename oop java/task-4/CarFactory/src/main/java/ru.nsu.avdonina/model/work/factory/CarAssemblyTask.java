package ru.nsu.avdonina.model.work.factory;

import ru.nsu.avdonina.model.listeners.CarAssembleListener;
import ru.nsu.avdonina.model.listeners.FactoryTasksListener;
import ru.nsu.avdonina.model.products.Accessory;
import ru.nsu.avdonina.model.products.Body;
import ru.nsu.avdonina.model.products.Car;
import ru.nsu.avdonina.model.products.Engine;
import ru.nsu.avdonina.model.storage.Storage;
import ru.nsu.avdonina.model.work.Worker;

public class CarAssemblyTask extends Worker {
    private final Storage<Body> bodyStorage;
    private final Storage<Engine> engineStorage;
    private final Storage<Accessory> accessoryStorage;
    private final Storage<Car> carStorage;
    private final CarAssembleListener carAssembleListener;
    private final FactoryTasksListener factoryTasksListener;

    public CarAssemblyTask(int workTime,
                           Storage<Body> bodyStorage,
                           Storage<Engine> engineStorage,
                           Storage<Accessory> accessoryStorage,
                           Storage<Car> carStorage,
                           CarAssembleListener carAssembleListener,
                           FactoryTasksListener factoryTasksListener) {
        super(workTime);
        this.bodyStorage = bodyStorage;
        this.engineStorage = engineStorage;
        this.accessoryStorage = accessoryStorage;
        this.carStorage = carStorage;
        this.carAssembleListener = carAssembleListener;
        this.factoryTasksListener = factoryTasksListener;
    }

    @Override
    public void run() {
        try {
            Body body = bodyStorage.take();
            Engine engine = engineStorage.take();
            Accessory accessory = accessoryStorage.take();
            Thread.sleep(workTime);
            Car car = new Car(body, engine, accessory);
            carStorage.put(car);
            carAssembleListener.notifyAboutCarAssemble();
            factoryTasksListener.notifyAboutCompletedTask();
        } catch (InterruptedException e) {
            Thread.currentThread().interrupt();
        }
    }
}
