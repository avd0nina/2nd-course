package ru.nsu.avdonina.model.work;

public abstract class WorkersCreator<W extends Worker> {
    private int workTime;

    protected WorkersCreator(int workTime) {
        this.workTime = workTime;
    }

    public abstract W createWorker();

    public int getWorkTime() {
        return workTime;
    }

    public void setWorkTime(int workTime) {
        this.workTime = workTime;
    }
}
