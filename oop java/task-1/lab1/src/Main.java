import java.io.IOException;

public class Main {
    public static void main(String[] args) {
       try {
           FileProcessor forward = new ForwardFileReader();
           FileProcessor reverse = new ReverseFileReader();
           Composer composer = new Composer(forward, reverse);
           String filename = "src/notes.txt";
           composer.printTwoColumns(filename);
       } catch (IOException e) {
           System.out.println(e.getMessage());
       }
    }
}
