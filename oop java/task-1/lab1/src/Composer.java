import java.io.IOException;
import java.util.List;

public class Composer {
    private static final int LINE_SIZE = 30;

    private final FileProcessor forwardReader;
    private final FileProcessor reverseReader;

    public Composer(FileProcessor forwardReader, FileProcessor reverseReader) {
        this.forwardReader = forwardReader;
        this.reverseReader = reverseReader;
    }

    private void printTwoColumns(List<String> forwardLines, List<String> reverseLines) {
        int forwardIndex = 0;
        int reverseIndex = 0;
        String forwardCarry = "";
        String reverseCarry = "";

        while (forwardIndex < forwardLines.size() || reverseIndex < reverseLines.size() || !forwardCarry.isEmpty() || !reverseCarry.isEmpty()) {
            String left = !forwardCarry.isEmpty() ? forwardCarry : (forwardIndex < forwardLines.size() ? forwardLines.get(forwardIndex++) : "");
            String right = !reverseCarry.isEmpty() ? reverseCarry : (reverseIndex < reverseLines.size() ? reverseLines.get(reverseIndex++) : "");
            forwardCarry = "";
            reverseCarry = "";

            while (!left.isEmpty() || !right.isEmpty()) {
                String leftPart = left.length() > LINE_SIZE ? left.substring(0, LINE_SIZE) : left;
                String rightPart = right.length() > LINE_SIZE ? right.substring(0, LINE_SIZE) : right;

                System.out.printf("%-30s | %-30s%n", leftPart, rightPart);

                if (left.length() > LINE_SIZE) {
                    left = left.substring(LINE_SIZE);
                } else {
                    left = (forwardIndex < forwardLines.size()) ? forwardLines.get(forwardIndex++) : "";
                }

                if (right.length() > LINE_SIZE) {
                    right = right.substring(LINE_SIZE);
                } else {
                    right = (reverseIndex < reverseLines.size()) ? reverseLines.get(reverseIndex++) : "";
                }
            }
        }
    }

    public void printTwoColumns(String filename) throws IOException {
        List<String> forwardLines = forwardReader.readLines(filename);
        List<String> reverseLines = reverseReader.readLines(filename);
        printTwoColumns(forwardLines, reverseLines);
    }
}
