package ru.nsu.avdonina.model.work.factory;

public abstract class FactoryPlanner extends Thread {
    private final Factory factory;
    private final Object synchronizationObject = new Object();
    private volatile boolean wasNotified = false;

    protected FactoryPlanner(Factory factory) {
        this.factory = factory;
    }

    @Override
    public void run() {
        while (!isInterrupted()) {
            int newTasksCount = getNewTasksCount();
            if (newTasksCount > 0) {
                factory.addTasks(newTasksCount);
            }
            wasNotified = false;

            synchronized (synchronizationObject) {
                while (!wasNotified) {
                    try {
                        synchronizationObject.wait();
                    } catch (InterruptedException e) {
                        interrupt();
                        return;
                    }
                }
            }
        }
    }

    public Object getSynchronizationObject() {
        return synchronizationObject;
    }

    public void notifyAboutCarSale() {
        wasNotified = true;
        synchronized (synchronizationObject) {
            synchronizationObject.notifyAll();
        }
    }

    public abstract int getNewTasksCount();
}
