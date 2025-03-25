package org.example;

import org.example.ControlledThread;

import java.util.Scanner;
import java.util.concurrent.locks.Condition;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;

public class SingleThreadController {
    public static void main(String[] args) {
        Lock lock = new ReentrantLock();
        Condition condition = lock.newCondition();
        ControlledThread task = new ControlledThread(lock, condition);
        Thread thread = new Thread(task, "Thread");
        thread.start();
        Scanner scanner = new Scanner(System.in);
        System.out.println("Введите команду (start, pause, resume, stop, exit):");
        while (true) {
            String command = scanner.nextLine();
            switch (command) {
                case "start":
                    task.start();
                    lock.lock();
                    try {
                        condition.signal();
                    } finally {
                        lock.unlock();
                    }
                    break;
                case "pause":
                    task.pause();
                    break;
                case "resume":
                    task.resume();
                    break;
                case "stop":
                    task.stop();
                    break;
                case "exit":
                    task.stop();
                    System.exit(0);
                    break;
                default:
                    System.out.println("Unknown command.");
            }
        }
    }
}
