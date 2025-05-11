package ru.nsu.avdonina.controller;

import ru.nsu.avdonina.model.Emulator;

import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;
import java.util.logging.Logger;

public class WindowClosingController extends WindowAdapter {
    private final Logger logger;
    private Emulator emulator;

    public WindowClosingController(Logger errorsLogger) {
        this.logger = errorsLogger;
    }

    public void setEmulator(Emulator emulator) {
        this.emulator = emulator;
    }

    @Override
    public void windowClosing(WindowEvent windowEvent) {
        emulator.terminate();
        for (var it : logger.getHandlers()) { //  цикл for-each, который перебирает все обработчики, связанные с логгером.
            it.flush(); // сообщения будут записаны в файл до завершения работы программы.
        }
    }
}
