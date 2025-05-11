package ru.nsu.avdonina.model.work.supply;

import ru.nsu.avdonina.model.listeners.SupplyListener;
import ru.nsu.avdonina.model.products.Engine;
import ru.nsu.avdonina.model.storage.Storage;
import ru.nsu.avdonina.model.work.WorkersCreator;

public class EngineSuppliersCreator extends WorkersCreator<EngineSupplier> { // фабрика для создания объектов типа EngineSupplier
    private final Storage<Engine> storage;
    private final SupplyListener supplyListener;

    public EngineSuppliersCreator(Storage<Engine> storage, SupplyListener supplyListener, int workTime) {
        super(workTime);
        this.storage = storage;
        this.supplyListener = supplyListener;
    }

    @Override
    public EngineSupplier createWorker() {
        return new EngineSupplier(storage, supplyListener, getWorkTime());
    }
}
