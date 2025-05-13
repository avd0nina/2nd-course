package ru.nsu.avdonina.model.work;

import java.util.LinkedList;
import java.util.Queue;

public class TaskQueue {
    private final Queue<Runnable> tasks = new LinkedList<>();
    private final int capacity;

    public TaskQueue(int capacity) {
        this.capacity = capacity;
    }

    public synchronized void put(Runnable task) throws InterruptedException {
        while (tasks.size() >= capacity) {
            wait();
        }
        tasks.add(task);
        notifyAll();
    }

    public synchronized Runnable take() throws InterruptedException {
        while (tasks.isEmpty()) {
            wait();
        }
        Runnable task = tasks.poll();
        notifyAll();
        return task;
    }

    public synchronized int size() {
        return tasks.size();
    }
}
