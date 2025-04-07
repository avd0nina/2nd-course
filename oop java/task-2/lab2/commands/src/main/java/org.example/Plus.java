package org.example;


public class Plus implements Command {
    @Override
    public void operation(Contexts context) {
        if (context.getStack().size() < 2) {
            throw new IllegalStateException("Not enough operands for addition");
        }
        Double a = context.getStack().pop();
        Double b = context.getStack().pop();
        context.getStack().push(a + b);
    }
}
