package org.example;

import java.util.Scanner;

public class FactorialCalculation {
    public static long factorialRecursive(int n) {
        if (n == 0 || n == 1) {
            return 1;
        }
        return n * factorialRecursive(n - 1);
    }

    public static long factorialIterative(int n) {
        long result = 1;
        for (int i = 2; i <= n; i++) {
            result *= i;
        }
        return result;
    }

    public static void main(String[] args) {
        Scanner scanner = new Scanner(System.in);
        System.out.print("Введите число для вычисления факториала: ");
        int number = scanner.nextInt();
        scanner.close();
        Thread recursiveThread = new Thread(() -> {
            long result = factorialRecursive(number);
            System.out.println("Рекурсивный результат: " + result);
        });
        Thread iterativeThread = new Thread(() -> {
            long result = factorialIterative(number);
            System.out.println("Итеративный результат: " + result);
        });
        recursiveThread.start();
        iterativeThread.start();
        try {
            recursiveThread.join();
            iterativeThread.join();
        } catch (InterruptedException e) {
            System.out.println("Ошибка: " + e.getMessage());
        }
        System.out.println("Потоки завершили работу");
    }
}
