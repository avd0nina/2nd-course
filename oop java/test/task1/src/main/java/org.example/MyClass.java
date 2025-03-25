package org.example;

public class MyClass {
    private int field1;
    private String field2;

    public MyClass(int field1, String field2) {
        this.field1 = field1;
        this.field2 = field2;
    }

    public void printDetails() {
        System.out.println("Field1: " + field1 + ", Field2: " + field2);
    }

    private void privateMethod() {
        System.out.println("Private method.");
    }

    public void publicMethod() {
        System.out.println("Public method.");
    }
}
