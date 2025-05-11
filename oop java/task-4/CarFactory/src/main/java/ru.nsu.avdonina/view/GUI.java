package ru.nsu.avdonina.view;

import ru.nsu.avdonina.model.View;
import ru.nsu.avdonina.model.listeners.*;
import ru.nsu.avdonina.model.parsing.config.Config;
import ru.nsu.avdonina.model.parsing.limits.Limits;
import ru.nsu.avdonina.model.products.*;

import javax.swing.*;
import javax.swing.event.ChangeListener;
import java.awt.event.WindowListener;
import java.util.logging.Logger;

public class GUI implements
        View,
        SupplyListener,
        CarAssembleListener,
        CarRequestListener,
        StorageListener,
        FactoryTasksListener {

    private final Frame frame;
    // Счётчики поставленных и произведённых элементов
    private int bodiesSuppliedCount = 0;
    private int enginesSuppliedCount = 0;
    private int accessoriesSuppliedCount = 0;
    private int carsAssembledCount = 0;
    private int carsSoldCount = 0;
    // Счётчики складских запасов и количества активных задач
    private int bodiesStored = 0;
    private int enginesStored = 0;
    private int accessoriesStored = 0;
    private int carsStored = 0;
    private int tasksCount = 0;

    public GUI(AbstractFrameFactory frameFactory,
               Config config,
               Limits limits,
               Logger logger,
               ChangeListener bodySuppliersCountListener,
               ChangeListener engineSuppliersCountListener,
               ChangeListener accessorySuppliersCountListener,
               ChangeListener workersCountListener,
               ChangeListener dealersCountListener,
               ChangeListener bodySupplyTimeListener,
               ChangeListener engineSupplyTimeListener,
               ChangeListener accessorySupplyTimeListener,
               ChangeListener carAssemblingTimeListener,
               ChangeListener carRequestTimeListener,
               WindowListener windowListener) {

        frame = frameFactory.createMainFrame(
                config,
                limits,
                logger,
                bodySuppliersCountListener,
                engineSuppliersCountListener,
                accessorySuppliersCountListener,
                workersCountListener,
                dealersCountListener,
                bodySupplyTimeListener,
                engineSupplyTimeListener,
                accessorySupplyTimeListener,
                carAssemblingTimeListener,
                carRequestTimeListener,
                windowListener
        );
        SwingUtilities.invokeLater(frame::pack); // Swing, когда у тебя будет время, подбери размер окна под все элементы внутри.
    }

    public void close() { // Метод для корректного закрытия окна
        SwingUtilities.invokeLater(() -> {
            frame.setVisible(false);
            frame.dispose();
        });
    }

    @Override
    public synchronized void notifyAboutSupply(CarPart carPart) {
        if (carPart instanceof Body) { // instanceof — это оператор, который проверяет, является ли объект экземпляром (или наследником) класса.
            bodiesSuppliedCount++;
            frame.setBodiesSuppliedCount(bodiesSuppliedCount); // обновляем значение на экране
        }

        if (carPart instanceof Engine) {
            enginesSuppliedCount++;
            frame.setEnginesSuppliedCount(enginesSuppliedCount);
        }

        if (carPart instanceof Accessory) {
            accessoriesSuppliedCount++;
            frame.setAccessoriesSuppliedCount(accessoriesSuppliedCount);
        }
    }

    @Override
    public synchronized void notifyAboutCarAssemble() {
        carsAssembledCount++;
        frame.setCarsAssembledCount(carsAssembledCount);
    }

    @Override
    public synchronized void notifyAboutCarSale() {
        carsSoldCount++;
        frame.setCarsSoldCount(carsSoldCount);
    }

    @Override
    public synchronized void notifyAboutProductPut(Product product) {
        if (product instanceof Body) {
            bodiesStored++;
            updateBodiesStored();
        }

        if (product instanceof Engine) {
            enginesStored++;
            updateEnginesStored();
        }

        if (product instanceof Accessory) {
            accessoriesStored++;
            updateAccessoriesStored();
        }

        if (product instanceof Car) {
            carsStored++;
            updateCarsStored();
        }
    }

    @Override
    public synchronized void notifyAboutProductTaken(Product product) {
        if (product instanceof Body) {
            bodiesStored--;
            updateBodiesStored();
        }

        if (product instanceof Engine) {
            enginesStored--;
            updateEnginesStored();
        }

        if (product instanceof Accessory) {
            accessoriesStored--;
            updateAccessoriesStored();
        }

        if (product instanceof Car) {
            carsStored--;
            updateCarsStored();
        }
    }

    @Override
    public synchronized void notifyAboutNewTasks(int newTasksCount) {
        tasksCount += newTasksCount;
        updateTasksCount();
    }

    @Override
    public synchronized void notifyAboutCompletedTask() {
        tasksCount--;
        updateTasksCount();
    }

    private void updateBodiesStored() {
        frame.setBodiesStoredCount(bodiesStored);
    }

    private void updateEnginesStored() {
        frame.setEnginesStoredCount(enginesStored);
    }

    private void updateAccessoriesStored() {
        frame.setAccessoriesStoredCount(accessoriesStored);
    }

    private void updateCarsStored() {
        frame.setCarsStoredCount(carsStored);
    }

    private void updateTasksCount() {
        frame.setTasksWaitingCount(tasksCount);
    }
}
