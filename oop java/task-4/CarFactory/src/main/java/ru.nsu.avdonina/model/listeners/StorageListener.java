package ru.nsu.avdonina.model.listeners;

import ru.nsu.avdonina.model.products.Product;

public interface StorageListener {
    void notifyAboutProductPut(Product product); // продукт кладут на склад
    void notifyAboutProductTaken(Product product); // продукт забирают со склада
}
