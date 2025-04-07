package org.example;

public class Calculate {

    public static Command makeComand(org.example.Contexts context) {
        String commandName = context.getLine().get(0);
        return CommandFactory.makeCommand(commandName);
    }

    public static void executeComand(Command command, org.example.Contexts context) {
        if (command == null) {
            throw new AssertionError("Error: command == null!");
        }
        command.operation(context);
        System.out.println(context.getStack());
    }
}
