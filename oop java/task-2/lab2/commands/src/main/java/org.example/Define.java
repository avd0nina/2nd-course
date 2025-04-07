package org.example;

public class Define implements Command {
    public void operation(Contexts context) {
        if (context.getLine().size() > 3) {
            throw new IllegalArgumentException("the command DEFINE has a signature - <name number>");
        }
        if (!Types.isChar(context.getLine().get(1)) && !Types.isDouble(context.getLine().get(2))) {
            throw new IllegalArgumentException("the command DEFINE has a signature - <name number>");
        }
        Character name = (context.getLine().get(1)).charAt(0);
        Double val = Double.parseDouble(context.getLine().get(2));
        context.newDefine(name, val);
    }
}
