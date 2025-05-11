package ru.nsu.avdonina.model.work.supply;

import ru.nsu.avdonina.model.listeners.SupplyListener;
import ru.nsu.avdonina.model.products.CarPart;
import ru.nsu.avdonina.model.storage.Storage;
import ru.nsu.avdonina.model.work.factory.Worker;

public abstract class Supplier<P extends CarPart> extends Worker { // каждый поставщик будет работать в отдельном потоке
    private final Storage<P> storage;
    private final SupplyListener supplyListener;

    protected Supplier(Storage<P> storage, SupplyListener supplyListener, int workTime) {
        super(workTime);
        this.storage = storage;
        this.supplyListener = supplyListener;
    }

    @Override
    public void run() {
        while (!isSoftlyInterrupted()) {
            try {
                Thread.sleep(getWorkTime());
                final P carPart = supply();
                storage.put(carPart);
                supplyListener.notifyAboutSupply(carPart);
            } catch (InterruptedException exception) {
                interrupt();
                return;
            }
        }
        interrupt();
    }

    protected abstract P supply();
}
