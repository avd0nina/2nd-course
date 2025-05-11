package ru.nsu.avdonina.model.listeners;

public interface FactoryTasksListener {
    void notifyAboutNewTasks(int newTasksCount); // в фабрику поступают новые задачи
    void notifyAboutCompletedTask(); // одна из задач завершена
}
