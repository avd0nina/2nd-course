package org.example;
import org.junit.jupiter.api.Assertions;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;

import java.io.ByteArrayOutputStream;
import java.io.PrintStream;
import java.util.List;

public class OperationTest {
    private Contexts context;

    @BeforeEach
    public void setUp() {
        context = new MyContext();
    }

    @Test
    public void testPlus() {
        context.getStack().push(3.0);
        context.getStack().push(2.0);
        new Plus().operation(context);
        Assertions.assertEquals(5.0, context.getStack().pop());
    }

    @Test
    public void testMinus() {
        context.getStack().push(10.0);
        context.getStack().push(4.0);
        new Minus().operation(context);
        Assertions.assertEquals(6.0, context.getStack().pop());
    }

    @Test
    public void testMultiplication() {
        context.getStack().push(3.0);
        context.getStack().push(5.0);
        new Multiplication().operation(context);
        Assertions.assertEquals(15.0, context.getStack().pop());
    }

    @Test
    public void testDivision() {
        context.getStack().push(10.0);
        context.getStack().push(2.0);
        new Division().operation(context);
        Assertions.assertEquals(5.0, context.getStack().pop());
    }

    @Test
    public void testDivisionByZero() {
        context.getStack().push(10.0);
        context.getStack().push(0.0);
        Assertions.assertThrows(ArithmeticException.class, () -> new Division().operation(context));
    }

    @Test
    public void testPush() {
        context.addLine(List.of("PUSH 555.5".split(" ")));
        new Push().operation(context);
        Assertions.assertEquals(555.5, context.getStack().pop());
    }

    @Test
    public void testPushWithDefine(){
        context.addLine(List.of("DEFINE a 777.7".split(" ")));
        new Define().operation(context);
        context.addLine(List.of("PUSH a".split(" ")));
        new Push().operation(context);
    }

    @Test
    public void testPop() {
        context.getStack().push(777.7);
        new Pop().operation(context);
        Assertions.assertTrue(context.getStack().isEmpty());
    }

    @Test
    public void testPopFromEmptyStack() {
        Assertions.assertThrows(IllegalStateException.class, () -> new Pop().operation(context));
    }

    @Test
    public void testPrint() {
        context.getStack().push(777.7);
        Print print = new Print();
        ByteArrayOutputStream outputStream = new ByteArrayOutputStream();
        PrintStream realOutput = System.out;
        System.setOut(new PrintStream(outputStream));
        print.operation(context);
        Assertions.assertEquals("777.7", outputStream.toString().trim());
        Assertions.assertEquals(1, context.getStack().size());
        Assertions.assertEquals(777.7, context.getStack().peek());
        System.setOut(realOutput);
    }

    @Test
    public void testDefine() {
        context.addLine(List.of("DEFINE a 555.5".split(" ")));
        new Define().operation(context);
        Assertions.assertEquals(555.5, context.getDefines().get('a'));
    }
}
