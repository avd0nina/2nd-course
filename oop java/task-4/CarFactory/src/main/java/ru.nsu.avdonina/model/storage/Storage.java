package ru.nsu.avdonina.model.storage;

public interface Storage<P> {
    void put(P product) throws InterruptedException;

    P take() throws InterruptedException;

    int getMaxCapacity();

    int getSize();

    int getRemainingCapacity();
}
