# xxds - Record File Hex Dumper

xxds is a C program that reads record-oriented files and displays them in hexadecimal format with EBCDIC character representation. By default it reads Variable Block (VB) format files; fixed-length records are also supported.

## Description

xxds reads files from standard input and prints them in a hex dump format similar to `xxd` or `hexdump`, but specifically designed for EBCDIC-encoded mainframe data.

In the default mode, the program processes Variable Block records by reading a 2-byte big-endian length field, followed by the record data. With the `-f` / `--fixed` option, records are read at a fixed byte length with no length prefix.

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

```
xxds [-f|--fixed record_length] [<file]
```

Read a Variable Block file (default):

```bash
./xxds < input_file.vb
```

Read fixed-length records (for example, 80-byte records):

```bash
./xxds -f 80 < input_file.dat
./xxds --fixed 80 < input_file.dat
```

Or pipe input:

```bash
cat input_file.vb | ./xxds
cat input_file.dat | ./xxds -f 80
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

- Reads Variable Block format files from stdin (default)
- Reads fixed-length records with `-f` / `--fixed`
- Displays records with hex dump and EBCDIC character representation
- Handles incomplete records gracefully
- Tracks record count and total byte count
- Uses EBCDIC Code Page 037 for character conversion
- Supports records up to 32K in size

## Record Format

### Variable Block (default)

VB format records consist of:
- **4 bytes**: RDW consisting of:
  * a big-endian 16-bit unsigned integer specifying the total record length (including the RDW itself)
  * 2 flag bytes (ignored by this program)
- **Remaining bytes**: Record data (length - 4 bytes)

### Fixed Length (`-f` / `--fixed`)

Fixed-length records consist of a constant number of data bytes per record, with no length prefix. The record length is specified on the command line. If the file size is not an exact multiple of the record length, the final partial record is reported as incomplete.

## Error Handling

If a record is incomplete (file ends before the expected end of the record), the program will:
- Display the partial record data
- Print "Last record incomplete"
- Continue with the end-of-file summary

## License

This program is licensed under the MIT License.

