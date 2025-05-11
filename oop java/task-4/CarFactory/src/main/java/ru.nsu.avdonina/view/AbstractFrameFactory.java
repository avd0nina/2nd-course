package ru.nsu.avdonina.view;

import ru.nsu.avdonina.model.parsing.config.Config;
import ru.nsu.avdonina.model.parsing.limits.Limits;

import javax.swing.event.ChangeListener; // интерфейс Swing, который реагирует на изменения значений в элементах интерфейса
import java.awt.event.WindowListener; // позволяет реагировать на события окна, например закрытие, открытие
import java.util.logging.Logger;

public interface AbstractFrameFactory {
    Frame createMainFrame(Config config,
                          Limits limits,
                          Logger logger,
                          ChangeListener bodySuppliersCountListener, // Слушатели на изменение количества поставщиков
                          ChangeListener engineSuppliersCountListener,
                          ChangeListener accessorySuppliersCountListener,
                          ChangeListener workersCountListener, // Слушатели на изменение количества сборщиков и дилеров
                          ChangeListener dealersCountListener,
                          ChangeListener bodySupplyTimeListener, // Слушатели на изменение времени поставки компонентов
                          ChangeListener engineSupplyTimeListener,
                          ChangeListener accessorySupplyTimeListener,
                          ChangeListener carAssemblingTimeListener, // Слушатели на изменение времени сборки машин и времени между запросами дилеров
                          ChangeListener carRequestTimeListener,
                          WindowListener windowListener);
}
