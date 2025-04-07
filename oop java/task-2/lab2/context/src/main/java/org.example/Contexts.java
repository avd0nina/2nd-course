package org.example;

import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Stack;

public interface Contexts {
    Stack<Double> getStack();
    void newDefine(Character name, Double num);
    Double getVal(Character name);
    Map<Character, Double> getDefines();
    void addLine(List<String> newLine);
    List<String> getLine();
}
