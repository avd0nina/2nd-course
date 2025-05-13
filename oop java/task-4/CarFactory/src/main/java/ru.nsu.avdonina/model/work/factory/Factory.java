package ru.nsu.avdonina.model.work.factory;

import ru.nsu.avdonina.model.listeners.FactoryTasksListener;
import ru.nsu.avdonina.model.work.TaskQueue;
import ru.nsu.avdonina.model.work.ThreadPool;

public class Factory {
    private ThreadPool threadPool;
    private TaskQueue taskQueue;
    private final FactoryWorkersCreator creator;
    private final FactoryTasksListener factoryTasksListener;

    public Factory(FactoryWorkersCreator creator, FactoryTasksListener factoryTasksListener, int workersCount) {
        this.creator = creator;
        this.factoryTasksListener = factoryTasksListener;
        this.taskQueue = new TaskQueue(1000); // Произвольная вместимость
        this.threadPool = new ThreadPool(workersCount, taskQueue);
    }

    public void addTasks(int count) {
        for (int i = 0; i < count; i++) {
            threadPool.execute(creator.createWorker());
            factoryTasksListener.notifyAboutNewTasks(1);
        }
    }

    public void setWorkTime(int workTime) {
        creator.setWorkTime(workTime);
    }

    public void setWorkersCount(int newCount) {
        threadPool.shutdown();
        taskQueue = new TaskQueue(1000);
        threadPool = new ThreadPool(newCount, taskQueue);
    }

    public void shutdown() {
        threadPool.shutdown();
    }
}
