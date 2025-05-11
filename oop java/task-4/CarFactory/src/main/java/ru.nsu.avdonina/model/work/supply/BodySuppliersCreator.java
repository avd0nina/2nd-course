package ru.nsu.avdonina.model.work.supply;

import ru.nsu.avdonina.model.listeners.SupplyListener;
import ru.nsu.avdonina.model.products.Body;
import ru.nsu.avdonina.model.storage.Storage;
import ru.nsu.avdonina.model.work.WorkersCreator;

public class BodySuppliersCreator extends WorkersCreator<BodySupplier> { // фабрика для создания объектов типа BodySupplier
    private final Storage<Body> storage;
    private final SupplyListener supplyListener;

    public BodySuppliersCreator(Storage<Body> storage, SupplyListener supplyListener, int workTime) {
        super(workTime);
        this.storage = storage;
        this.supplyListener = supplyListener;
    }

    @Override
    public BodySupplier createWorker() {
        return new BodySupplier(storage, supplyListener, getWorkTime());
    }
}
