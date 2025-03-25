package org.example;

import java.lang.reflect.Field;
import java.lang.reflect.Method;
import java.util.Scanner;

public class Researcher {
    public void dig(String className) {
        try {
            Class<?> clazz = Class.forName(className);
            System.out.println("Поля класса " + className + ":");
            Field[] fields = clazz.getDeclaredFields();
            for (Field field : fields) {
                System.out.println("  " + field.getType().getName() + " " + field.getName());
            }
            System.out.println("Методы класса " + className + ":");
            Method[] methods = clazz.getDeclaredMethods();
            for (Method method : methods) {
                System.out.print("  " + method.getReturnType().getName() + " " + method.getName() + "(");
                Class<?>[] parameterTypes = method.getParameterTypes();
                for (int i = 0; i < parameterTypes.length; i++) {
                    System.out.print(parameterTypes[i].getName());
                    if (i < parameterTypes.length - 1) {
                        System.out.print(", ");
                    }
                }
                System.out.println(")");
            }
        } catch (ClassNotFoundException e) {
            System.out.println("Класс " + className + " не найден.");
        }
    }

    public static void main(String[] args) {
        Researcher researcher = new Researcher();
        Scanner scanner = new Scanner(System.in);
        while (true) {
            System.out.println("Введите имя класса или 'exit' для выхода:");
            String input = scanner.nextLine();
            if ("exit".equalsIgnoreCase(input)) {
                break;
            }
        researcher.dig(input);
        }
        scanner.close();
    }
}


