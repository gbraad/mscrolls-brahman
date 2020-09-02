/*******************************************************************************
 * Stefan Bylund 2020
 *
 * Tool for trimming a Strand Games .prog file. The resulting file will have
 * the same name as the .prog input file but with the extension .prg.
 ******************************************************************************/

package magnetic;

import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.nio.charset.StandardCharsets;
import java.util.HashSet;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;
import java.util.Set;

/**
 * Tool for trimming a Strand Games .prog file by only keeping the symbols used
 * by the Spectrum Next version of the Magnetic interpreter and removing the
 * relocation information. The resulting file will have the same name as the
 * .prog input file but with the extension .prg. The symbol table is quite large
 * and removing all unused symbols significantly reduces the loading time of the
 * .prog file. The relocation information is quite small but since it is not
 * needed by the Magnetic interpreter we might just as well remove it.
 *
 * Note: For extra safety, do a manual binary diff between the .prog and .prg
 * files to make sure that only the relocation information has been removed
 * and the symbol section(s) trimmed.
 */
public class TrimProg {
    private static final int HUNK_CODE = 0x3E9;

    private static final int HUNK_DATA = 0x3EA;

    private static final int HUNK_RELOC32 = 0x3EC;

    private static final int HUNK_SYMBOL = 0x3F0;

    private static final int HUNK_END = 0x3F2;

    private static final int HUNK_HEADER = 0x3F3;

    // Set of symbols used by the Spectrum Next version of the Magnetic interpreter, add more as needed.
    private static final Set<String> USED_SYMBOLS = new HashSet<>(List.of(
        "REMASTER",
        "OUTMODE",
        "STOPRON",
        "P.TN"
    ));

    public static void main(String[] args) {
        if (args.length != 1) {
            System.err.println("Usage: java -jar trim_prog.jar <prog_input_file>");
            return;
        }

        String infilename = args[0];
        int suffix = infilename.lastIndexOf('.');
        if (suffix < 1) {
            System.err.println("Invalid prog input file name.");
            return;
        }
        String outfilename = infilename.substring(0, suffix) + ".prg";

        try (DataInputStream in = new DataInputStream(new BufferedInputStream(new FileInputStream(infilename)))) {
            try (DataOutputStream out = new DataOutputStream(new BufferedOutputStream(new FileOutputStream(outfilename)))) {
                try {
                    processProgFile(in, out);
                } catch (IOException | ProgException e) {
                    System.err.println("Error trimming prog file: " + e.getMessage());
                    return;
                }
            } catch (IOException e) {
                System.err.println("Error creating prg output file: " + e.getMessage());
                return;
            }
        } catch (IOException e) {
            System.err.println("Error reading prog input file: " + e.getMessage());
            return;
        }

        System.out.println("Converted " + infilename + " to " + outfilename + ".");
    }

    /*
     * Format of a PROG file (which is an AmigaDOS load file).
     * All field sizes are in units of longwords and all data
     * is in big endian byte order.
     *
     * --------------------------------------------------------
     *
     * <hunk_header>
     * 0
     * num_hunks
     * 0
     * 0
     * hunk_sizes[num_hunks] (in longwords)
     *
     * --------------------------------------------------------
     *
     * <hunk_code>
     * code_size (in longwords)
     * code
     *
     * <hunk_reloc32>
     * num_relocs (0 = end of hunk)
     * hunk_number
     * relocs (num_relocs longwords)
     *
     * <hunk_symbol>
     * symbol_name_length (in longwords, 0 = end of hunk)
     * symbol_name (symbol_name_length longwords of characters)
     * symbol_offset
     *
     * <hunk_end>
     *
     * --------------------------------------------------------
     *
     * <hunk_data> (optional)
     * data_size (in longwords)
     * data
     *
     * <hunk_reloc32>
     * num_relocs (0 = end of hunk)
     * hunk_number
     * relocs (num_relocs longwords)
     *
     * <hunk_symbol>
     * symbol_name_length (in longwords, 0 = end of hunk)
     * symbol_name (symbol_name_length longwords of characters)
     * symbol_offset
     *
     * <hunk_end>
     *
     * --------------------------------------------------------
     */

    private static void processProgFile(DataInputStream in, DataOutputStream out) throws IOException, ProgException {
        boolean codeHandled = false;
        boolean dictHandled = false;
        boolean symbolsHandled = false;

        ProgHeader header = processProgHeader(in, out);
        int numSections = header.numHunks;

        loop: while (true) {
            int hunkType = in.readInt();

            switch (hunkType) {
                case HUNK_CODE:
                    int codeSize = in.readInt();
                    if ((codeSize << 2) != header.codeSize) {
                        throw new ProgException("Inconsistent code size.");
                    }
                    out.writeInt(hunkType);
                    out.writeInt(codeSize);
                    out.write(in.readNBytes(header.codeSize));
                    codeHandled = true;
                    break;
                case HUNK_DATA:
                    int dictSize = in.readInt();
                    if ((dictSize << 2) != header.dictSize) {
                        throw new ProgException("Inconsistent data/dictionary size.");
                    }
                    out.writeInt(hunkType);
                    out.writeInt(dictSize);
                    out.write(in.readNBytes(header.dictSize));
                    dictHandled = true;
                    break;
                case HUNK_SYMBOL:
                    trimProgSymbols(in, out);
                    symbolsHandled = true;
                    break;
                case HUNK_RELOC32:
                    // Skip relocation information
                    for (;;) {
                        int numRelocs = in.readInt();
                        if (numRelocs == 0) {
                            break;
                        }
                        in.skipBytes(4 + numRelocs * 4);
                    }
                    break;
                case HUNK_END:
                    out.writeInt(hunkType);
                    if (--numSections == 0) {
                        break loop;
                    }
                    break;
                default:
                    throw new ProgException("Unknown hunk type.");
            }
        }

        if (!codeHandled) {
            throw new ProgException("No code section found.");
        }
        if (header.dictSize > 0 && !dictHandled) {
            throw new ProgException("No data/dictionary section found.");
        }
        if (!symbolsHandled) {
            throw new ProgException("No symbol section found.");
        }
    }

    private static ProgHeader processProgHeader(DataInputStream in, DataOutputStream out) throws IOException, ProgException {
        int hunkHeader = in.readInt();
        if (hunkHeader != HUNK_HEADER) {
            throw new ProgException("No hunk header found.");
        }
        out.writeInt(hunkHeader);

        // skip
        out.writeInt(in.readInt());

        int numHunks = in.readInt();
        if ((numHunks == 0) || (numHunks > 3)) {
            throw new ProgException("Invalid number of hunks.");
        }
        out.writeInt(numHunks);

        // skip
        out.writeInt(in.readInt());
        out.writeInt(in.readInt());

        int[] hunkSizes = {0, 0, 0};
        for (int i = 0; i < numHunks; i++) {
            hunkSizes[i] = in.readInt();
            out.writeInt(hunkSizes[i]);
        }

        int codeSize = hunkSizes[0] << 2;
        int dictSize = hunkSizes[1] << 2;

        return new ProgHeader(numHunks, codeSize, dictSize);
    }

    private static void trimProgSymbols(DataInputStream in, DataOutputStream out) throws IOException, ProgException {
        Map<String, ProgSymbol> symbolMap = new LinkedHashMap<>();

        for (;;) {
            int nameSize = in.readInt() << 2;
            if (nameSize == 0) {
                break; // End of symbol hunk
            } else if (nameSize > 32) {
                throw new ProgException("Too long symbol name.");
            }

            byte[] name = in.readNBytes(nameSize);
            int value = in.readInt();
            ProgSymbol symbol = new ProgSymbol(nameSize, name, value);

            if (USED_SYMBOLS.contains(symbol.str)) {
                symbolMap.put(symbol.str, symbol);
            }
        }

        if (!symbolMap.isEmpty()) {
            out.writeInt(HUNK_SYMBOL);

            for (ProgSymbol symbol : symbolMap.values()) {
                out.writeInt(symbol.nameSize);
                out.write(symbol.name);
                out.writeInt(symbol.value);
            }

            out.writeInt(0); // End of symbol hunk
        }

        if (symbolMap.keySet().containsAll(USED_SYMBOLS)) {
            System.out.println("All required symbols found.");
        }
    }

    @SuppressWarnings("serial")
    private static class ProgException extends Exception {
        ProgException(String message) {
            super(message);
        }
    }

    private static class ProgHeader {
        public final int numHunks;

        public final int codeSize;

        public final int dictSize;

        ProgHeader(int numHunks, int codeSize, int dictSize) {
            this.numHunks = numHunks;
            this.codeSize = codeSize;
            this.dictSize = dictSize;
        }
    }

    private static class ProgSymbol {
        public final int nameSize;

        public final byte[] name;

        public final String str;

        public final int value;

        ProgSymbol(int nameByteSize, byte[] name, int value) {
            this.nameSize = nameByteSize >> 2;
            this.name = name;
            this.str = createString(name);
            this.value = value;
        }

        private static String createString(byte[] chars) {
            int end = chars.length;

            for (int i = 0; i < chars.length; i++) {
                if (chars[i] == 0) {
                    end = i;
                    break;
                }
            }

            return new String(chars, 0, end, StandardCharsets.US_ASCII);
        }
    }
}
