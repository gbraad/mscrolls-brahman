/*******************************************************************************
 * Stefan Bylund 2019
 *
 * Tool for converting a Strand Games .text file to a .txt and .idx file.
 ******************************************************************************/

package magnetic;

import java.io.FileOutputStream;
import java.io.FilterOutputStream;
import java.io.IOException;
import java.io.OutputStream;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.ArrayList;
import java.util.List;

/**
 * Converts a Strand Games .text file to a .txt and .idx file.
 *
 * The .text file contains the strings used by the .prog file for the Strand
 * Games adaptation of the Magnetic interpreter. The ZX Spectrum Next version
 * of the Magnetic interpreter needs to have an index file for the strings in
 * the .text file and have the markup removed from the .text file.
 *
 * The .txt output file is created by removing any markup and carriage return
 * characters from the .text input file.
 *
 * The .idx output file contains an array of offsets for each newline-terminated
 * string in the .txt file. The array length is a 16-bit integer and the offsets
 * are 32-bit integers, all in little endian byte order. File format:
 *
 * array-length
 * string0-offset
 * string1-offset
 * ...
 * stringN-offset
 *
 * Note: After the conversion, do a manual diff between the .text and .txt files
 * to make sure that not too much text has been removed. Also note that the
 * generated .txt file cannot be manually changed after the conversion, that will
 * mess up the generated .idx file.
 */
public class ConvertText {
    public static void main(String[] args) {
        if (args.length != 1) {
            System.err.println("Usage: java -jar convert_text.jar <text_input_file>");
            return;
        }

        String filename = args[0];
        int suffix = filename.lastIndexOf('.');
        if (suffix < 1) {
            System.err.println("Invalid text input file name.");
            return;
        }
        String basename = filename.substring(0, suffix);

        String content;
        try {
            content = Files.readString(Path.of(filename), StandardCharsets.US_ASCII);
        } catch (IOException e) {
            System.err.println("Error reading text input file.");
            return;
        }

        content = removeGameSpecificMarkup(basename, content);
        content = removeMarkup(content);

        try {
            Files.writeString(Path.of(basename + ".txt"), content, StandardCharsets.US_ASCII);
        } catch (IOException e) {
            System.err.println("Error writing string output file.");
            return;
        }

        try {
            writeStringIndexFile(basename, content);
        } catch (IOException e) {
            System.err.println("Error writing string index output file.");
        }

        System.out.println("Converted " + filename + " to " + basename + ".txt and " + basename + ".idx.");
    }

    private static String removeGameSpecificMarkup(String game, String content) {
        switch (game) {
            case "pawn":
                break;
            case "guild":
                break;
            case "jinxter":
                content = content.replace("\\[here](http://strandgames.com/files/jinxter/document.html)\\",
                    "at http://strandgames.com/files/jinxter/document.html");
                break;
            case "corrupt":
                break;
            case "fish":
                break;
            case "myth":
                break;
            case "wonder":
                break;
            default:
                break;
        }

        return content;
    }

    private static String removeMarkup(String content) {
        // Replace "[text](action)", "![text](action)" and "#[text](action)" with "text".
        content = content.replaceAll("[\\!#]\\[([\\w\\s\\.,;:\\!\\?'\"\\-/]*)\\]\\([\\w\\s\\.,;:\\!\\?'\"\\-/]*\\)", "$1");
        content = content.replaceAll("\\[([\\w\\s\\.,;:\\!\\?'\"\\-/]*)\\]\\([\\w\\s\\.,;:\\!\\?'\"\\-/]*\\)", "$1");

        // Replace "[text]" with "text".
        content = content.replaceAll("\\[([\\w\\s\\.,;:\\!\\?'\"\\-/]+)\\]", "$1");

        // Replace "\_text_\" with "text".
        content = content.replace("\\_", "").replace("_\\", "");

        // Replace "**text**" with "text".
        content = content.replace("**", "");

        // Replace "\text\" with "text".
        content = content.replace("\\", "");

        // Remove "{color:<color>}".
        content = content.replaceAll("\\{color:\\w+\\}", "");

        // Remove carriage return characters.
        content = content.replace("\r", "");

        if (content.contains("[") ||
            content.contains("]") ||
            content.contains("{") ||
            content.contains("}") ||
            content.contains("\\") ||
            content.contains("_") ||
            content.contains("//")) {
            System.out.println("Suspicious characters remaining, do a manual check.");
        }

        return content;
    }

    private static void writeStringIndexFile(String basename, String content) throws IOException {
        List<Integer> offsets = new ArrayList<>();

        offsets.add(0);

        for (int i = 0; i < content.length(); i++) {
            char ch = content.charAt(i);
            if ((ch == '\n') || (i == content.length() - 1)) {
                offsets.add(i + 1);
            }
        }

        try (LittleEndianOutputStream out = new LittleEndianOutputStream(new FileOutputStream(basename + ".idx"))) {
            out.writeShort(offsets.size());

            for (int offset : offsets) {
                out.writeInt(offset);
            }
        }
    }

    /**
     * An output stream for writing 16/32-bit integers in little endian format.
     */
    private static class LittleEndianOutputStream extends FilterOutputStream {
        LittleEndianOutputStream(OutputStream out) {
            super(out);
        }

        public void writeShort(int s) throws IOException {
            out.write(s & 0xFF);
            out.write((s >>> 8) & 0xFF);
        }

        public void writeInt(int i) throws IOException {
            out.write(i & 0xFF);
            out.write((i >>> 8) & 0xFF);
            out.write((i >>> 16) & 0xFF);
            out.write((i >>> 24) & 0xFF);
        }
    }
}
