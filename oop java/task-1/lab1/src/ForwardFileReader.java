import java.io.*;
import java.util.ArrayList;
import java.util.List;

public class ForwardFileReader implements FileProcessor {
    @Override
    public List<String> readLines(String filename) throws IOException {
        List<String> lines = new ArrayList<>();
        try (BufferedReader reader = new BufferedReader(new FileReader(filename))) {
            String line;
            while ((line = reader.readLine()) != null) {
                lines.add(line);
            }
        }
    return lines;
    }

}
