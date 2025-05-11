package ru.nsu.avdonina.model.work.supply;

import ru.nsu.avdonina.model.listeners.SupplyListener;
import ru.nsu.avdonina.model.products.Engine;
import ru.nsu.avdonina.model.storage.Storage;

public class EngineSupplier extends Supplier<Engine> {
    public EngineSupplier(Storage<Engine> storage, SupplyListener supplyListener, int workTime) {
        super(storage, supplyListener, workTime);
    }

    @Override
    protected Engine supply() {
        return new Engine();
    }
}
