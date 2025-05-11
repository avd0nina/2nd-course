package ru.nsu.avdonina.view;

import javax.swing.*;

abstract class Frame extends JFrame {
    public abstract void setBodiesSuppliedCount(int newValue); // количество поставленных кузовов
    public abstract void setEnginesSuppliedCount(int newValue); // количество поставленных двигателей
    public abstract void setAccessoriesSuppliedCount(int newValue); // количество поставленных аксессуаров
    public abstract void setCarsAssembledCount(int newValue); // сколько машин собрано на данный момент
    public abstract void setCarsSoldCount(int newValue); // количество проданных машин — обновляется, когда дилеры берут машины со склада
    public abstract void setBodiesStoredCount(int newValue); // количество кузовов на складе
    public abstract void setEnginesStoredCount(int newValue); // количество двигателей на складе
    public abstract void setAccessoriesStoredCount(int newValue); // количество аксессуаров на складе
    public abstract void setCarsStoredCount(int newValue); // количество машин на складе, готовых к продаже
    public abstract void setTasksWaitingCount(int newValue); // количество ожидающих задач для сборщиков на фабрике. "очередь машин на сборку".
}
