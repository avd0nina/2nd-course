package ru.nsu.avdonina.model;

import ru.nsu.avdonina.model.listeners.*;
import ru.nsu.avdonina.model.parsing.config.Config;
import ru.nsu.avdonina.model.products.Accessory;
import ru.nsu.avdonina.model.products.Body;
import ru.nsu.avdonina.model.products.Car;
import ru.nsu.avdonina.model.products.Engine;
import ru.nsu.avdonina.model.storage.ProductStorage;
import ru.nsu.avdonina.model.work.WorkersThreadPool;
import ru.nsu.avdonina.model.work.carsale.Dealer;
import ru.nsu.avdonina.model.work.carsale.DealersCreator;
import ru.nsu.avdonina.model.work.factory.CarStorageFactoryPlanner;
import ru.nsu.avdonina.model.work.factory.Factory;
import ru.nsu.avdonina.model.work.factory.FactoryPlanner;
import ru.nsu.avdonina.model.work.factory.FactoryWorkersCreator;
import ru.nsu.avdonina.model.work.supply.*;

import java.util.logging.Logger;

public class Emulator {
    // Потоки поставщиков кузовов, двигателей и аксессуаров
    private final WorkersThreadPool<BodySupplier> bodySuppliersPool;
    private final WorkersThreadPool<EngineSupplier> engineSuppliersPool;
    private final WorkersThreadPool<AccessorySupplier> accessorySuppliersPool;
    // Поток работников фабрики
    private final Factory factory;
    // Поток дилеров
    private final WorkersThreadPool<Dealer> dealersPool;
    // Планировщик задач на фабрике (говорит фабрике, когда нужно выдать задания)
    private final FactoryPlanner factoryPlanner;
    private final View view;

    public Emulator(Config config,
                    Logger logger,
                    View view,
                    SupplyListener supplyListener,
                    CarAssembleListener carAssembleListener,
                    CarRequestListener carRequestListener,
                    StorageListener storageListener,
                    FactoryTasksListener factoryTasksListener) {
        this.view = view;
        // Создание хранилищ продуктов
        final ProductStorage<Body> bodyStorage = new ProductStorage<>(config.bodyStorageCapacity(), storageListener);
        final ProductStorage<Engine> engineStorage = new ProductStorage<>(config.engineStorageCapacity(), storageListener);
        final ProductStorage<Accessory> accessoryStorage = new ProductStorage<>(config.accessoryStorageCapacity(), storageListener);
        final ProductStorage<Car> carStorage = new ProductStorage<>(config.carStorageCapacity(), storageListener);
        // Создание объектов, которые будут создавать поставщиков
        final BodySuppliersCreator bodySuppliersCreator = new BodySuppliersCreator(
                bodyStorage,
                supplyListener,
                config.bodySupplyTimeMs()
        );
        final EngineSuppliersCreator engineSuppliersCreator = new EngineSuppliersCreator(
                engineStorage,
                supplyListener,
                config.engineSupplyTimeMs()

        );
        final AccessorySuppliersCreator accessorySuppliersCreator = new AccessorySuppliersCreator(
                accessoryStorage,
                supplyListener,
                config.accessorySupplyTimeMs()
        );
        // Создание пулов потоков поставщиков
        this.bodySuppliersPool = new WorkersThreadPool<>(
                bodySuppliersCreator,
                config.bodySuppliersCount()
        );
        this.engineSuppliersPool = new WorkersThreadPool<>(
                engineSuppliersCreator,
                config.engineSuppliersCount()
        );
        this.accessorySuppliersPool = new WorkersThreadPool<>(
                accessorySuppliersCreator,
                config.accessorySuppliersCount()
        );
        // Создание объектов, которые будут создавать работников фабрики
        final FactoryWorkersCreator factoryWorkerWorkersCreator = new FactoryWorkersCreator(
                bodyStorage,
                engineStorage,
                accessoryStorage,
                carStorage,
                carAssembleListener,
                factoryTasksListener,
                config.carAssemblingTimeMs()
        );
        // Создание фабрики (которая управляет сборкой машин)
        this.factory = new Factory(
                factoryWorkerWorkersCreator,
                factoryTasksListener, config.workersCount()
        );
        // Планировщик задач фабрики, ориентированный на свободное место на складе машин
        this.factoryPlanner = new CarStorageFactoryPlanner(factory, carStorage);
        // Создание объектов, которые создают дилеров
        final DealersCreator dealersCreator = new DealersCreator(
                carStorage,
                carRequestListener,
                factoryPlanner,
                logger,
                config.carRequestTimeMs()
        );
        // Создание пула потоков дилеров
        this.dealersPool = new WorkersThreadPool<>(
                dealersCreator,
                config.dealersCount()
        );
    }
    // Методы изменения параметров во время работы
    public void setBodySupplyTime(int newValue) {
        bodySuppliersPool.setWorkTime(newValue);
    }

    public void setEngineSupplyTime(int newValue) {
        engineSuppliersPool.setWorkTime(newValue);
    }

    public void setAccessorySupplyTime(int newValue) {
        accessorySuppliersPool.setWorkTime(newValue);
    }

    public void setCarAssembleTime(int newValue) {
        factory.setWorkTime(newValue);
    }

    public void setCarRequestTime(int newValue) {
        dealersPool.setWorkTime(newValue);
    }

    public void setBodySuppliersCount(int newValue) {
        bodySuppliersPool.setWorkersCount(newValue);
    }

    public void setEngineSuppliersCount(int newValue) {
        engineSuppliersPool.setWorkersCount(newValue);
    }

    public void setAccessorySuppliersCount(int newValue) {
        accessorySuppliersPool.setWorkersCount(newValue);
    }

    public void setWorkersCount(int newValue) {
        factory.setWorkersCount(newValue);
    }

    public void setDealersCount(int newValue) {
        dealersPool.setWorkersCount(newValue);
    }

    public void start() { // Запуск всей симуляции
        bodySuppliersPool.start();
        engineSuppliersPool.start();
        accessorySuppliersPool.start();
        dealersPool.start();
        factoryPlanner.start();
        factory.start();
    }

    public void terminate() { // Остановка всей симуляции
        bodySuppliersPool.terminate();
        engineSuppliersPool.terminate();
        accessorySuppliersPool.terminate();
        dealersPool.terminate();
        factory.terminate();
        factoryPlanner.interrupt();
        view.close();
    }
}
