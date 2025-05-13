package ru.nsu.avdonina.model;

import ru.nsu.avdonina.model.listeners.*;
import ru.nsu.avdonina.model.parsing.config.Config;
import ru.nsu.avdonina.model.products.Accessory;
import ru.nsu.avdonina.model.products.Body;
import ru.nsu.avdonina.model.products.Car;
import ru.nsu.avdonina.model.products.Engine;
import ru.nsu.avdonina.model.storage.ProductStorage;
import ru.nsu.avdonina.model.work.ThreadPool;
import ru.nsu.avdonina.model.work.TaskQueue;
import ru.nsu.avdonina.model.work.Worker;
import ru.nsu.avdonina.model.work.WorkersCreator;
import ru.nsu.avdonina.model.work.factory.CarStorageFactoryPlanner;
import ru.nsu.avdonina.model.work.factory.Factory;
import ru.nsu.avdonina.model.work.factory.FactoryWorkersCreator;
import ru.nsu.avdonina.model.work.supply.*;
import ru.nsu.avdonina.model.work.carsale.*;

import java.util.logging.Logger;

public class Emulator {
    private ThreadPool bodySuppliersPool;
    private ThreadPool engineSuppliersPool;
    private ThreadPool accessorySuppliersPool;
    private ThreadPool dealersPool;
    private final Factory factory;
    private final CarStorageFactoryPlanner factoryPlanner;
    private final View view;
    private final BodySuppliersCreator bodySuppliersCreator;
    private final EngineSuppliersCreator engineSuppliersCreator;
    private final AccessorySuppliersCreator accessorySuppliersCreator;
    private final DealersCreator dealersCreator;

    public Emulator(Config config,
                    Logger logger,
                    View view,
                    SupplyListener supplyListener,
                    CarAssembleListener carAssembleListener,
                    CarRequestListener carRequestListener,
                    StorageListener storageListener,
                    FactoryTasksListener factoryTasksListener) {
        this.view = view;
        final ProductStorage<Body> bodyStorage = new ProductStorage<>(config.bodyStorageCapacity(), storageListener);
        final ProductStorage<Engine> engineStorage = new ProductStorage<>(config.engineStorageCapacity(), storageListener);
        final ProductStorage<Accessory> accessoryStorage = new ProductStorage<>(config.accessoryStorageCapacity(), storageListener);
        final ProductStorage<Car> carStorage = new ProductStorage<>(config.carStorageCapacity(), storageListener);

        this.bodySuppliersCreator = new BodySuppliersCreator(bodyStorage, supplyListener, config.bodySupplyTimeMs());
        this.engineSuppliersCreator = new EngineSuppliersCreator(engineStorage, supplyListener, config.engineSupplyTimeMs());
        this.accessorySuppliersCreator = new AccessorySuppliersCreator(accessoryStorage, supplyListener, config.accessorySupplyTimeMs());
        final FactoryWorkersCreator factoryWorkerCreator = new FactoryWorkersCreator(
                bodyStorage, engineStorage, accessoryStorage, carStorage,
                carAssembleListener, factoryTasksListener, config.carAssemblingTimeMs());
        this.factory = new Factory(factoryWorkerCreator, factoryTasksListener, config.workersCount());
        this.factoryPlanner = new CarStorageFactoryPlanner(factory, carStorage);
        this.dealersCreator = new DealersCreator(carStorage, carRequestListener, factoryPlanner, logger, config.carRequestTimeMs());
        this.bodySuppliersPool = new ThreadPool(config.bodySuppliersCount(), new TaskQueue(1000));
        this.engineSuppliersPool = new ThreadPool(config.engineSuppliersCount(), new TaskQueue(1000));
        this.accessorySuppliersPool = new ThreadPool(config.accessorySuppliersCount(), new TaskQueue(1000));
        this.dealersPool = new ThreadPool(config.dealersCount(), new TaskQueue(1000));
    }

    public void setBodySupplyTime(int newValue) {
        bodySuppliersCreator.setWorkTime(newValue);
    }

    public void setEngineSupplyTime(int newValue) {
        engineSuppliersCreator.setWorkTime(newValue);
    }

    public void setAccessorySupplyTime(int newValue) {
        accessorySuppliersCreator.setWorkTime(newValue);
    }

    public void setCarAssembleTime(int newValue) {
        factory.setWorkTime(newValue);
    }

    public void setCarRequestTime(int newValue) {
        dealersCreator.setWorkTime(newValue);
    }

    public void setBodySuppliersCount(int newValue) {
        bodySuppliersPool.shutdown();
        bodySuppliersPool = new ThreadPool(newValue, new TaskQueue(1000));
        submitSupplierTasks(bodySuppliersCreator, bodySuppliersPool, newValue);
    }

    public void setEngineSuppliersCount(int newValue) {
        engineSuppliersPool.shutdown();
        engineSuppliersPool = new ThreadPool(newValue, new TaskQueue(1000));
        submitSupplierTasks(engineSuppliersCreator, engineSuppliersPool, newValue);
    }

    public void setAccessorySuppliersCount(int newValue) {
        accessorySuppliersPool.shutdown();
        accessorySuppliersPool = new ThreadPool(newValue, new TaskQueue(1000));
        submitSupplierTasks(accessorySuppliersCreator, accessorySuppliersPool, newValue);
    }

    public void setWorkersCount(int newValue) {
        factory.setWorkersCount(newValue);
    }

    public void setDealersCount(int newValue) {
        dealersPool.shutdown();
        dealersPool = new ThreadPool(newValue, new TaskQueue(1000));
        submitDealerTasks(dealersCreator, dealersPool, newValue);
    }

    public void start() {
        submitSupplierTasks(bodySuppliersCreator, bodySuppliersPool, bodySuppliersPool.getWorkersCount());
        submitSupplierTasks(engineSuppliersCreator, engineSuppliersPool, engineSuppliersPool.getWorkersCount());
        submitSupplierTasks(accessorySuppliersCreator, accessorySuppliersPool, accessorySuppliersPool.getWorkersCount());
        submitDealerTasks(dealersCreator, dealersPool, dealersPool.getWorkersCount());
        factoryPlanner.start();
    }

private void submitSupplierTasks(WorkersCreator<?> creator, ThreadPool pool, int count) {
    for (int i = 0; i < count; i++) {
        Runnable task = createSupplierTask(creator, pool);
        pool.execute(task);
    }
}

    private Runnable createSupplierTask(WorkersCreator<?> creator, ThreadPool pool) {
        return new Runnable() {
            @Override
            public void run() {
                Worker worker = creator.createWorker();
                worker.run();
                if (!Thread.currentThread().isInterrupted()) {
                    pool.execute(this); // Пересоздаём текущую задачу
                }
            }
        };
    }
    private void submitDealerTasks(DealersCreator creator, ThreadPool pool, int count) {
        for (int i = 0; i < count; i++) {
            Runnable task = createDealerTask(creator, pool);
            pool.execute(task);
        }
    }

    private Runnable createDealerTask(DealersCreator creator, ThreadPool pool) {
        return new Runnable() {
            @Override
            public void run() {
                Dealer dealer = creator.createWorker();
                dealer.run();
                if (!Thread.currentThread().isInterrupted()) {
                    pool.execute(this); // Пересоздаём текущую задачу
                }
            }
        };
    }

    public void terminate() {
        bodySuppliersPool.shutdown();
        engineSuppliersPool.shutdown();
        accessorySuppliersPool.shutdown();
        dealersPool.shutdown();
        factory.shutdown();
        factoryPlanner.interrupt();
        view.close();
    }
}
