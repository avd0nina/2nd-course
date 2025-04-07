package org.example;

public class Print implements Command {
    public void operation(Contexts context) {
        Double num = context.getStack().pop();
        System.out.println(num);
        context.getStack().push(num);
    }
}
