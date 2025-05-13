package ru.nsu.avdonina.model.work.supply;

import ru.nsu.avdonina.model.listeners.SupplyListener;
import ru.nsu.avdonina.model.products.CarPart;
import ru.nsu.avdonina.model.storage.Storage;
import ru.nsu.avdonina.model.work.Worker;

public abstract class Supplier<P extends CarPart> extends Worker {
    private final Storage<P> storage;
    private final SupplyListener supplyListener;

    protected Supplier(Storage<P> storage, SupplyListener supplyListener, int workTime) {
        super(workTime);
        this.storage = storage;
        this.supplyListener = supplyListener;
    }

    @Override
    public void run() {
        try {
            Thread.sleep(workTime);
            P carPart = supply();
            storage.put(carPart);
            supplyListener.notifyAboutSupply(carPart);
        } catch (InterruptedException e) {
            Thread.currentThread().interrupt();
        }
    }

    protected abstract P supply();
}
