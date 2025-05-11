package ru.nsu.avdonina.model.work.carsale;

import ru.nsu.avdonina.model.listeners.CarRequestListener;
import ru.nsu.avdonina.model.products.Car;
import ru.nsu.avdonina.model.storage.Storage;
import ru.nsu.avdonina.model.work.factory.FactoryPlanner;
import ru.nsu.avdonina.model.work.factory.Worker;

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
        this.factoryPlanner = factoryPlanner;
        this.storage = storage;
        this.logger = logger;
        this.carRequestListener = carRequestListener;
        this.factoryPlannerSynchronizationObject = factoryPlanner.getSynchronizationObject();
    }

    @Override
    public void run() {
        while (!isSoftlyInterrupted()) {
            try {
                final Car car = storage.take(); // Берет одну машину со склада
                carRequestListener.notifyAboutCarSale(); // Уведомляет о продаже
                if (logger != null) {
                    final String message = String.format("Dealer %s: %s%n", Thread.currentThread().getName(), car); // пишет информацию в лог: кто продал какую машину
                    logger.info(message);
                }
                Thread.sleep(getWorkTime()); // эмуляция времени продажи
                synchronized (factoryPlannerSynchronizationObject) { // Поток входит в синхронизированный блок
                    factoryPlanner.notifyAboutCarSale(); // Вызов notifyAboutCarSale() устанавливает флаг wasNotified = true в FactoryPlanner
                    factoryPlannerSynchronizationObject.notifyAll(); // notifyAll() пробуждает FactoryPlanner, который спал в ожидании события продажи машины
                } // После этого FactoryPlanner снова запускает getNewTasksCount() и добавляет задачи на сборку новых машин
            } catch (InterruptedException exception) {
                interrupt();
                return;
            }
        }
        interrupt();
    }
}
