//package org.example;
//
//public class CommandFactory {
//    public static Command makeCommand(String commandName) {
//        return switch (commandName) {
//            case "+" -> new Plus();
//            case "-" -> new Minus();
//            case "*" -> new Multiplication();
//            case "/" -> new Division();
//            case "SQRT" -> new SQRT();
//            case "DEFINE" -> new Define();
//            case "PRINT" -> new Print();
//            case "POP" -> new Pop();
//            case "PUSH" -> new Push();
//            default -> null; //exception
//        };
//    }
//}
package org.example;

import java.util.Properties;

public class CommandFactory {
    private static final Properties commands = ConfigLoader.loadProperties("commands.properties");

    public static Command makeCommand(String commandName) {
        String className = commands.getProperty(commandName);
        if (className == null) {
            throw new IllegalArgumentException("Unknown command: " + commandName);
        }
        try {
            Class<?> clazz = Class.forName(className);
            return (Command) clazz.getDeclaredConstructor().newInstance();
        } catch (Exception e) {
            throw new RuntimeException("Error creating command instance", e);
        }
    }
}
