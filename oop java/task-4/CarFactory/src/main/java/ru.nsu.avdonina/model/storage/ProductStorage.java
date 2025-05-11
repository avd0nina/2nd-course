package ru.nsu.avdonina.model.storage;

import ru.nsu.avdonina.model.listeners.StorageListener;
import ru.nsu.avdonina.model.products.Product;

import java.util.concurrent.ArrayBlockingQueue; // имеет фиксированный размер и блокирует операции put/take, если очередь полная/пустая
import java.util.concurrent.BlockingQueue; // потокобезопасная очередь с блокировкой

public class ProductStorage<P extends Product> implements Storage<P> {
    private final BlockingQueue<P> storage; // Очередь для хранения объектов типа P (например, деталей или автомобилей)
    private final StorageListener storageListener; // Слушатель, которого будут уведомлять о добавлении или извлечении продукта.
    private final int maxCapacity; // Максимальное количество элементов, которое можно хранить в очереди.

    public ProductStorage(int maxCapacity, StorageListener storageListener) {
        storage = new ArrayBlockingQueue<>(maxCapacity);
        this.storageListener = storageListener;
        this.maxCapacity = maxCapacity;
    }

    @Override
    public void put(P product) throws InterruptedException {
        storage.put(product); // блокируется, если очередь переполнена
        storageListener.notifyAboutProductPut(product);
    }

    @Override
    public P take() throws InterruptedException {
        final P ret = storage.take(); // блокируется, если очередь пуста
        storageListener.notifyAboutProductTaken(ret);
        return ret;
    }

    @Override
    public int getMaxCapacity() {
        return maxCapacity;
    }

    @Override
    public int getSize() {
        return storage.size();
    }

    @Override
    public int getRemainingCapacity() {
        return storage.remainingCapacity();
    }
}
