package ru.nsu.avdonina.model.work.factory;

public abstract class FactoryPlanner extends Thread {
    private final Factory factory;
    private final Object synchronizationObject = new Object();
    private boolean wasNotified = false;

    protected FactoryPlanner(Factory factory) {
        this.factory = factory;
    }

    @Override
    public void run() {
        while (true) {
            final int newTasksCount = getNewTasksCount();
            factory.addTasks(newTasksCount);
            wasNotified = false;
            while (!wasNotified) {
                synchronized (synchronizationObject) { // cинхронизируем блок кода чтобы гарантировать, что только один поток будет выполнять блок внутри синхронизации в каждый момент времени
                    try {
                        synchronizationObject.wait(); // поток, который вызвал этот метод, будет приостановлен до тех пор, пока не получит сигнал от другого потока
                    } catch (InterruptedException exception) {
                        interrupt();
                        return;
                    }
                }
            }
        }
    } // Когда рабочие завершат выполнение задач или когда возникнет другая ситуация, планировщик получит сигнал (с помощью метода notify()), и этот блок кода завершит ожидание

    public Object getSynchronizationObject() { // Когда рабочие завершат выполнение задач или когда возникнет другая ситуация, планировщик получит сигнал (с помощью метода notify()), и этот блок кода завершит ожидание
        return synchronizationObject;
    }

    public void notifyAboutCarSale() { // была произведена продажа автомобиля, и планировщик теперь может начать планировать следующие задачи
        wasNotified = true;
    }

    public abstract int getNewTasksCount();
}
