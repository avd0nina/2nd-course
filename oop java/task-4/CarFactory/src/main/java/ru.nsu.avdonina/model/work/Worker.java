package ru.nsu.avdonina.model.work;

public abstract class Worker implements Runnable {
    protected int workTime;

    protected Worker(int workTime) {
        this.workTime = workTime;
    }

    public void setWorkTime(int workTime) {
        this.workTime = workTime;
    }

    public int getWorkTime() {
        return workTime;
    }

    @Override
    public abstract void run();
}
