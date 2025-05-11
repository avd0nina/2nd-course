package ru.nsu.avdonina.model.listeners;

import ru.nsu.avdonina.model.products.CarPart;

public interface SupplyListener {
    void notifyAboutSupply(CarPart carPart); // поставка нового компонента
}
