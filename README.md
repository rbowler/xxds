# xxds - Variable Block File Hex Dumper

xxds is a C program that reads Variable Block (VB) format files and displays them in hexadecimal format with EBCDIC character representation.

## Description

xxds reads VB format files from standard input and prints them in a hex dump format similar to `xxd` or `hexdump`, but specifically designed for EBCDIC-encoded Variable Block files. The program processes records by reading the RDW containing a 2-byte big-endian length field, followed by the record data.

## Building

To build the program:

```bash
make
```

This will compile `xxds.c` and create the `xxds` executable.

## Installation

To install the program system-wide:

```bash
sudo make install
```

This will copy the executable to `/usr/local/bin/`.

## Usage

Read from a file:

```bash
./xxds < input_file.vb
```

Or pipe input:

```bash
cat input_file.vb | ./xxds
```

## Output Format

For each record, the program prints:

1. **Record Header**: `Record: nnnn  Length: nnnn  Offset: xxxxxxxx`
   - Record number (starting from 1)
   - Record length in bytes
   - File offset in hexadecimal

2. **Hex Dump**: Each 16-byte chunk is displayed as:
   ```
   oooo:  xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx  cccccccccccccccc
   ```
   - `oooo`: Offset within the record (4 hex digits)
   - `xxxxxxxx`: Four groups of 4 bytes in hexadecimal
   - `cccccccccccccccc`: EBCDIC character representation (unprintable chars shown as dots)

3. **End of File Summary**: Printed after the last record:
   ```
   End of file: n record[s], nnnn byte[s] (xxxxxxxx)
   ```
   - `n`: Total number of records read
   - `nnnn`: Total number of bytes read
   - `xxxxxxxx`: File size in hexadecimal

### Example Output

```
Record: 1  Length: 88  Offset: 00000000
0000:  00580000 00800050  D7C1E4E3 E2E4D4F0  .......&PAUTSUM0
0010:  00000000 00000000  00000000 00000000  ................
0020:  00000000 00000000  00000000 01010072  ................
0030:  D7C1E4E3 C4E3D3F1  00000000 00000000  PAUTDTL1........
0040:  00000000 00000000  00000000 00000000  ................
0050:  00000000 020280CE                    ........
End of file: 1 record, 88 bytes (00000058)
```

## Features

- Reads Variable Block format files from stdin
- Displays records with hex dump and EBCDIC character representation
- Handles incomplete records gracefully
- Tracks record count and total byte count
- Uses EBCDIC Code Page 037 for character conversion
- Supports records up to 32K in size

## Record Format

VB format records consist of:
- **4 bytes**: RDW consisting of:
  * a big-endian 16-bit unsigned integer specifying the total record length (including the RDW itself)
  * 2 flag bytes (ignored by this program)
- **Remaining bytes**: Record data (length - 4 bytes)

## Error Handling

If a record is incomplete (file ends before the expected end of the record), the program will:
- Display the partial record data
- Print "Last record incomplete"
- Continue with the end-of-file summary

## License

This program is licensed under the MIT License.

