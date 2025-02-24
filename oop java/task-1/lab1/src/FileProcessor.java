import java.io.IOException;
import java.util.List;

public interface FileProcessor {
    List<String> readLines(String filename) throws IOException;
}
