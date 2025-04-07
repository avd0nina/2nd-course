package org.example;

public class Pop implements Command {
    @Override
    public void operation(Contexts context) {
        if (context.getStack().isEmpty()) {
            throw new IllegalStateException("Not enough operands");
        }
        context.getStack().pop();
    }
}
