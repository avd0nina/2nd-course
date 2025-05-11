package ru.nsu.avdonina.view;

import javax.swing.*;

class DefaultFrame extends Frame {
    private final JTable suppliedTable; // таблица, отображающая количество поставленных и обработанных компонентов
    private final JTable storedTable; // таблица, отображающая текущие значения на складах

    public DefaultFrame(JTable suppliedTable, JTable storedTable) {
        this.suppliedTable = suppliedTable;
        this.storedTable = storedTable;
    }

    private static void setValueOfTable(JTable table, int column, int newValue) { // обновляет значение ячейки в таблице
        table.setValueAt(String.valueOf(newValue), 0, column);
    }

    @Override
    public void setBodiesSuppliedCount(int newValue) { // значение поставленных кузовов
        setValueOfTable(suppliedTable, 0, newValue);
    }

    @Override
    public void setEnginesSuppliedCount(int newValue) { // значение поставленных двигателей
        setValueOfTable(suppliedTable, 1, newValue);
    }

    @Override
    public void setAccessoriesSuppliedCount(int newValue) { // значение поставленных аксессуаров
        setValueOfTable(suppliedTable, 2, newValue);
    }

    @Override
    public void setCarsAssembledCount(int newValue) { // значение собранных машин
        setValueOfTable(suppliedTable, 3, newValue);
    }

    @Override
    public void setCarsSoldCount(int newValue) { // значение проданных машин
        setValueOfTable(suppliedTable, 4, newValue);
    }

    @Override
    public void setBodiesStoredCount(int newValue) { // сколько на складе кузовов
        setValueOfTable(storedTable, 0, newValue);
    }

    @Override
    public void setEnginesStoredCount(int newValue) { // сколько на складе двигателей
        setValueOfTable(storedTable, 1, newValue);
    }

    @Override
    public void setAccessoriesStoredCount(int newValue) { // сколько на складе аксессуаров
        setValueOfTable(storedTable, 2, newValue);
    }

    @Override
    public void setCarsStoredCount(int newValue) { // сколько на складе машин
        setValueOfTable(storedTable, 3, newValue);
    }

    @Override
    public void setTasksWaitingCount(int newValue) { // количество ожидающих задач на сборку
        setValueOfTable(storedTable, 4, newValue);
    }
}
