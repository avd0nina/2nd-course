package ru.nsu.avdonina.controller;

import javax.swing.*;
import javax.swing.event.ChangeEvent;

final class Utils {
    private Utils() {
    }

    public static int getValueFromSliderEvent(ChangeEvent changeEvent) {
        return ((JSlider) changeEvent.getSource()).getValue(); // получить текущее значение, установленное на слайдере
    }
}
