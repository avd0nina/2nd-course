package org.example;

import java.util.Arrays;
import java.util.Arrays;
import java.util.List;
import java.util.Objects;
import java.util.Scanner;

public class Main {
    public static void main(String[] args) {
        Scanner scanner = new Scanner(System.in);
        org.example.Contexts context = new MyContext();
        String firstLine = scanner.nextLine();
        if (firstLine.endsWith(".txt")) {
            System.out.println("read from file");
            System.out.println(MyReader.reader(firstLine, context));
        } else {
            String line = "";
            Command currentComand;
            while (!(Objects.equals((line = scanner.nextLine()), "stop"))) {
                if (line.startsWith("#")) {
                    continue;
                }
                context.addLine(Arrays.asList(line.split(" ")));
                currentComand = Calculate.makeComand(context);
                Calculate.executeComand(currentComand, context);
            }
            System.out.println("Everything is ready");
        }
    }
}
