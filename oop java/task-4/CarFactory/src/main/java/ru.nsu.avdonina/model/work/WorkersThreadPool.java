package ru.nsu.avdonina.model.work;

import ru.nsu.avdonina.model.work.factory.Worker;

import java.util.ArrayList;
import java.util.List;

public class WorkersThreadPool<W extends Worker> {
    private final List<W> workers; // все рабочие потоки
    private final List<W> softlyInterruptedWorkers; // рабочие потоки, которые были мягко прерваны
    private final WorkersCreator<W> creator; // объект, который создает рабочие потоки
    private boolean started = false; // флаг, который отслеживает, был ли пул потоков запущен

    public WorkersThreadPool(WorkersCreator<W> creator, int workersCount) {
        this.workers = new ArrayList<>();
        this.softlyInterruptedWorkers = new ArrayList<>();
        this.creator = creator;
        addWorkers(workersCount); // добавить указанное количество рабочих потоков в пул.
    }

    public void start() { // запускает все рабочие потоки
        started = true;
        workers.forEach(Thread::start);
    }

    public void terminate() { // для завершения работы всех рабочих потоков
        workers.forEach(Thread::interrupt);
        softlyInterruptedWorkers.forEach(Thread::interrupt);
    }

    public void setWorkTime(int workTime) { // для изменения времени работы всех рабочих потоков
        creator.setWorkTime(workTime);
        workers.forEach(worker -> worker.setWorkTime(workTime));
    }

    public void setWorkersCount(int count) { // для изменения количества рабочих потоков в пуле
        if (count == workers.size()) {
            return;
        }
        if (count > workers.size()) {
            addWorkers(count - workers.size());
        } else {
            softlyInterruptSomeWorkers(workers.size() - count);
        }
    }

    private void softlyInterruptSomeWorkers(int count) { // для мягкого прерывания определенного числа рабочих потоков
        for (int i = 0; i < count; i++) {
            final int indexOfLast = workers.size() - 1;
            final W workerToSoftlyInterrupt = workers.get(indexOfLast);
            workerToSoftlyInterrupt.softlyInterrupt();
            workers.remove(indexOfLast);
            softlyInterruptedWorkers.add(workerToSoftlyInterrupt);
        }
    }

    private void addWorkers(int count) { // для добавления новых рабочих потоков в пул
        for (int i = 0; i < count; i++) {
            final W supplier = creator.createWorker();
            workers.add(supplier);
            if (started) {
                supplier.start();
            }
        }
    }
}
