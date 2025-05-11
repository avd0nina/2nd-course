package ru.nsu.avdonina.model.work.supply;

import ru.nsu.avdonina.model.listeners.SupplyListener;
import ru.nsu.avdonina.model.products.Accessory;
import ru.nsu.avdonina.model.storage.Storage;

public class AccessorySupplier extends Supplier<Accessory> {
    public AccessorySupplier(Storage<Accessory> storage, SupplyListener supplyListener, int workTime) {
        super(storage, supplyListener, workTime);
    }

    @Override
    protected Accessory supply() {
        return new Accessory();
    }
}
