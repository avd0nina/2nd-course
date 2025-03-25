package org.example;

import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;
import java.util.concurrent.locks.Condition;
import java.util.Scanner;

class ControlledThread implements Runnable {
    private final Lock lock;
    private final Condition condition;
    private volatile boolean running = false;
    private volatile boolean paused = false;

    public ControlledThread(Lock lock, Condition condition) {
        this.lock = lock;
        this.condition = condition;
    }

    public void start() {
        lock.lock();
        try {
            running = true;
        } finally {
            lock.unlock();
        }
    }

    public void pause() {
        lock.lock();
        try {
            paused = true;
        } finally {
            lock.unlock();
        }
    }

    public void resume() {
        lock.lock();
        try {
            paused = false;
            condition.signal();
        } finally {
            lock.unlock();
        }
    }

    public void stop() {
        running = false;
    }

    @Override
    public void run() {
        try {
            while (true) {
                lock.lock();
                try {
                    while (!running) {
                        condition.await();
                    }
                    while (paused) {
                        condition.await();
                    }
                } finally {
                    lock.unlock();
                }
                System.out.println(Thread.currentThread().getName() + " is running...");
                Thread.sleep(2000);
            }
        } catch (InterruptedException e) {
            Thread.currentThread().interrupt();
        }
        System.out.println(Thread.currentThread().getName() + " has stopped.");
    }
}
