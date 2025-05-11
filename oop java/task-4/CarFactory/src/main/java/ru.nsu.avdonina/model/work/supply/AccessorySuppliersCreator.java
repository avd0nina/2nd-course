package ru.nsu.avdonina.model.work.supply;

import ru.nsu.avdonina.model.listeners.SupplyListener;
import ru.nsu.avdonina.model.products.Accessory;
import ru.nsu.avdonina.model.storage.Storage;
import ru.nsu.avdonina.model.work.WorkersCreator;

public class AccessorySuppliersCreator extends WorkersCreator<AccessorySupplier> { // фабрика для создания объектов типа AccessorySupplier
    private final Storage<Accessory> storage;
    private final SupplyListener supplyListener;

    public AccessorySuppliersCreator(Storage<Accessory> storage, SupplyListener supplyListener, int workTime) {
        super(workTime);
        this.storage = storage;
        this.supplyListener = supplyListener;
    }

    @Override
    public AccessorySupplier createWorker() {
        return new AccessorySupplier(storage, supplyListener, getWorkTime());
    }
}
