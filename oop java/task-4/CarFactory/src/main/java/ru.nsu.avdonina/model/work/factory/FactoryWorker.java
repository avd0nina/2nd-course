package ru.nsu.avdonina.model.work.factory;

import ru.nsu.avdonina.model.listeners.CarAssembleListener;
import ru.nsu.avdonina.model.listeners.FactoryTasksListener;
import ru.nsu.avdonina.model.products.Accessory;
import ru.nsu.avdonina.model.products.Body;
import ru.nsu.avdonina.model.products.Car;
import ru.nsu.avdonina.model.products.Engine;
import ru.nsu.avdonina.model.storage.Storage;

public class FactoryWorker extends Worker { // рабочий поток, выполняющий какие-то задачи на фабрике
    private static int tasksCount = 0;
    private final Object synchronizationObject; // объект, используемый для синхронизации работы потоков.
    private final Storage<Body> bodyStorage;
    private final Storage<Engine> engineStorage;
    private final Storage<Accessory> accessoryStorage;
    private final Storage<Car> carStorage;
    private final CarAssembleListener carAssembleListener;
    private final FactoryTasksListener factoryTasksListener;

    public FactoryWorker(int workTime,
                         Storage<Body> bodyStorage,
                         Storage<Engine> engineStorage,
                         Storage<Accessory> accessoryStorage,
                         Storage<Car> carStorage,
                         CarAssembleListener carAssembleListener,
                         FactoryTasksListener factoryTasksListener,
                         Object synchronizationObject) {

        super(workTime);
        this.synchronizationObject = synchronizationObject;
        this.bodyStorage = bodyStorage;
        this.engineStorage = engineStorage;
        this.accessoryStorage = accessoryStorage;
        this.carStorage = carStorage;
        this.carAssembleListener = carAssembleListener;
        this.factoryTasksListener = factoryTasksListener;
    }

    public static void addTasks(int newTasks) {
        tasksCount += newTasks;
    }

    @Override
    public void run() {
        while (true) {
            synchronized (synchronizationObject) { // Блокировка, синхронизирующая доступ к общим ресурсам. Это гарантирует, что только один поток может выполнять работу в этот момент времени.
                while (tasksCount == 0) {
                    try {
                        synchronizationObject.wait(); // Если нет задач, рабочий поток будет ожидать появления новых задач.
                    } catch (InterruptedException exception) {
                        interrupt();
                        return;
                    }
                }
                if (isSoftlyInterrupted()) {
                    interrupt();
                    return;
                }
                tasksCount--;
            }
            try {
                execute(); // Вызов метода, который выполняет сборку автомобиля
            } catch (InterruptedException exception) {
                interrupt();
                return;
            }
            if (isSoftlyInterrupted()) {
                interrupt();
            }
        }
    }

    private void execute() throws InterruptedException { // Метод, выполняющий сборку автомобиля
        final Body body = bodyStorage.take(); // Забираем кузов из хранилища
        final Engine engine = engineStorage.take(); // Забираем двигатель
        final Accessory accessory = accessoryStorage.take(); // Забираем аксессуар
        Thread.sleep(getWorkTime());
        final Car car = new Car(body, engine, accessory);
        carStorage.put(car);
        carAssembleListener.notifyAboutCarAssemble();
        factoryTasksListener.notifyAboutCompletedTask();
    }
}
