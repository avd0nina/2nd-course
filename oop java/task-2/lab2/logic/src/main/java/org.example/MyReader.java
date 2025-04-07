package org.example;
import java.io.FileReader;


import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

public class MyReader {
    public static String reader(String fileName, org.example.Contexts context) {
        try(BufferedReader reader = new BufferedReader(new FileReader(fileName))) {
            String line;
            List<String[]> values = new ArrayList<String[]>();
            Command currentComand;
            while((line = reader.readLine()) != null) {
                if (line.startsWith("#")) {
                    continue;
                }
                values.add(line.split(" "));
            }
            for (String[] value : values) {
                context.addLine(Arrays.asList(value));
                currentComand = Calculate.makeComand(context);
                Calculate.executeComand(currentComand, context);
            }
        }
        catch (IOException e) {
            throw new RuntimeException(e);
        }
        return "Everything is ready";
    }
}
