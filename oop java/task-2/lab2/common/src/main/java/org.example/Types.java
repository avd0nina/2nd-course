package org.example;

public class Types {
    public static boolean isDouble(String value) {
        try {
            Double a = Double.parseDouble(value);
            return true;
        }
        catch (NumberFormatException e) {
            return false;
        }
    }
    public static boolean isChar(String value) {
        return value.length() == 1;
    }
}
