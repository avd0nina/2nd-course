package ru.nsu.avdonina.model.work.supply;

import ru.nsu.avdonina.model.listeners.SupplyListener;
import ru.nsu.avdonina.model.products.Body;
import ru.nsu.avdonina.model.storage.Storage;

public class BodySupplier extends Supplier<Body> {
    public BodySupplier(Storage<Body> storage, SupplyListener supplyListener, int workTime) {
        super(storage, supplyListener, workTime);
    }

    @Override
    public Body supply() {
        return new Body();
    }
}
