package ru.nsu.avdonina.model.work.carsale;

import ru.nsu.avdonina.model.listeners.CarRequestListener;
import ru.nsu.avdonina.model.products.Car;
import ru.nsu.avdonina.model.storage.Storage;
import ru.nsu.avdonina.model.work.Worker;
import ru.nsu.avdonina.model.work.factory.FactoryPlanner;

import java.util.logging.Logger;

public class Dealer extends Worker {
    private final Storage<Car> storage;
    private final FactoryPlanner factoryPlanner;
    private final Logger logger;
    private final CarRequestListener carRequestListener;
    private final Object factoryPlannerSynchronizationObject;

    public Dealer(Storage<Car> storage,
                  CarRequestListener carRequestListener,
                  FactoryPlanner factoryPlanner,
                  Logger logger,
                  int carRequestTime) {
        super(carRequestTime);
        this.storage = storage;
        this.factoryPlanner = factoryPlanner;
        this.logger = logger;
        this.carRequestListener = carRequestListener;
        this.factoryPlannerSynchronizationObject = factoryPlanner.getSynchronizationObject();
    }

    @Override
    public void run() {
        try {
            Car car = storage.take(); // Блокируется, если машин нет
            carRequestListener.notifyAboutCarSale();

            if (logger != null) {
                String message = String.format("Dealer %s: %s%n", Thread.currentThread().getName(), car);
                logger.info(message);
            }

            Thread.sleep(workTime);

            synchronized (factoryPlannerSynchronizationObject) {
                if (factoryPlanner != null) { // Проверка на null
                    factoryPlanner.notifyAboutCarSale();
                    factoryPlannerSynchronizationObject.notifyAll();
                }
            }
        } catch (InterruptedException e) {
            Thread.currentThread().interrupt();
        }
    }
}
