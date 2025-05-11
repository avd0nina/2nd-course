package ru.nsu.avdonina.model.work.factory;

public class MaxFactoryPlanner extends FactoryPlanner {
    private static final int CARS_TO_SALE_PLAN = 1000; // сколько автомобилей планируется для продажи
    private boolean assignedAllTasks = false; // для отслеживания того, были ли уже назначены все задачи. Это флаг, который показывает, завершена ли уже задача по планированию всех машин для продажи.

    public MaxFactoryPlanner(Factory factory) {
        super(factory);
    }

    @Override
    public int getNewTasksCount() {
        final int ret = assignedAllTasks ? 0 : CARS_TO_SALE_PLAN; // Если assignedAllTasks false, значит, еще не все задачи назначены, и мы возвращаем значение из константы CARS_TO_SALE_PLAN — то есть 1000 машин, которые нужно собрать.
        assignedAllTasks = true;
        return ret;
    }
}
