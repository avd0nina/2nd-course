package ru.nsu.avdonina.controller;

import ru.nsu.avdonina.model.Emulator;

import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;

public class AccessorySuppliersController implements ChangeListener {
    private Emulator emulator;

    public void setEmulator(Emulator emulator) {
        this.emulator = emulator;
    }

    @Override
    public void stateChanged(ChangeEvent changeEvent) { // вызывается каждый раз, когда происходит изменение слайдера
        emulator.setAccessorySuppliersCount(Utils.getValueFromSliderEvent(changeEvent));
    }
}
