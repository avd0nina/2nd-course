import java.io.IOException;
import java.util.Collections;
import java.util.List;


public class ReverseFileReader extends ForwardFileReader {
    @Override
    public List<String> readLines(String fileName) throws IOException {
        List<String> lines = super.readLines(fileName);
        Collections.reverse(lines);
        return lines;
    }
}
