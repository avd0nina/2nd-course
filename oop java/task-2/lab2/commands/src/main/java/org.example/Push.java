package org.example;

public class Push implements Command {
    @Override
    public void operation(Contexts context) {
        if (context.getLine().size() > 2) {
            throw new IllegalArgumentException("the command PUSH accepts only one number");
        }
        if (Types.isDouble(context.getLine().get(1))) {
            context.getStack().push(Double.parseDouble(context.getLine().get(1)));
        }
        else if (Types.isChar(context.getLine().get(1)) && Character.isLetter(context.getLine().get(1).charAt(0))) {
            if (context.getDefines().containsKey(context.getLine().get(1).charAt(0))) {
                context.getStack().push(context.getVal(context.getLine().get(1).charAt(0)));
            }
        }
        else {
            throw new IllegalArgumentException("the command PUSH accepts only number");
        }
    }
}
