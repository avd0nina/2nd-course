package ru.nsu.avdonina.model.work;

import java.util.ArrayList;
import java.util.List;

public class ThreadPool {
    private final List<Thread> workers = new ArrayList<>();
    private volatile boolean running = true;
    private final TaskQueue taskQueue;

    public ThreadPool(int numberOfWorkers, TaskQueue taskQueue) {
        this.taskQueue = taskQueue;
        for (int i = 0; i < numberOfWorkers; i++) {
            Thread worker = new Thread(() -> {
                while (running && !Thread.currentThread().isInterrupted()) {
                    try {
                        Runnable task = taskQueue.take();
                        task.run();
                    } catch (InterruptedException e) {
                        Thread.currentThread().interrupt();
                        break;
                    }
                }
            }, "ThreadPool-Worker-" + i);
            worker.start();
            workers.add(worker);
        }
    }

    public void execute(Runnable task) {
        if (running) {
            try {
                taskQueue.put(task);
            } catch (InterruptedException e) {
                Thread.currentThread().interrupt();
            }
        }
    }

    public void shutdown() {
        running = false;
        workers.forEach(Thread::interrupt);
    }

    public int getWorkersCount() {
        return workers.size();
    }
}
