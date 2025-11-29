/*
 * xxds.c - Variable Block (VB) File Hex Dumper
 *
 * This program reads Variable Block format files from stdin and prints them
 * in hexadecimal format with EBCDIC character representation.
 *
 * Program Operation:
 * 1. Reads VB records from stdin: reads a 2-byte big-endian length, then the
 *    remaining bytes
 * 2. Prints record headers with record number, record length, and file offset
 * 3. Formats hex output: 16-byte chunks with offset, 4 groups of 4 bytes, and
 *    EBCDIC representation
 * 4. Handles incomplete records: detects and reports incomplete records
 * 5. Prints end-of-file summary: record count and total byte count
 *
 * Output Format Example:
 *   0000:  00580000 00800050  D7C1E4E3 E2E4D4F0  .......&PAUTSUM0
 *   0010:  00000000 00000000  00000000 00000000  ................
 */

/* Date-Written: 2025-11-29 */
/* Copyright: Roger Bowler, 2025 */
/* SPDX-License-Identifier: MIT */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>

/* EBCDIC (Code Page 037) to ASCII conversion table (printable characters only) */
static const unsigned char ebcdic_to_ascii[256] = {
    /*         0     1     2     3     4     5     6     7      8     9     A     B     C     D     E     F  */
    /*00-0F*/ '.',  '.',  '.',  '.',  '.',  '.',  '.',  '.',  '.',  '.',  '.',  '.',  '.',  '.',  '.',  '.',
    /*10-1F*/ '.',  '.',  '.',  '.',  '.',  '.',  '.',  '.',  '.',  '.',  '.',  '.',  '.',  '.',  '.',  '.',
    /*20-2F*/ '.',  '.',  '.',  '.',  '.',  '.',  '.',  '.',  '.',  '.',  '.',  '.',  '.',  '.',  '.',  '.',
    /*30-3F*/ '.',  '.',  '.',  '.',  '.',  '.',  '.',  '.',  '.',  '.',  '.',  '.',  '.',  '.',  '.',  '.',
    /*40-4F*/ ' ',  '.',  '.',  '.',  '.',  '.',  '.',  '.',  '.',  '.', 0xA2,  '.',  '<',  '(',  '+',  '|',
    /*50-5F*/ '&',  '.',  '.',  '.',  '.',  '.',  '.',  '.',  '.',  '.',  '!',  '$',  '*',  ')',  ';', 0xAC,
    /*60-6F*/ '-',  '/',  '.',  '.',  '.',  '.',  '.',  '.',  '.',  '.', 0xA6,  ',',  '%',  '_',  '>',  '?',
    /*70-7F*/ '.',  '.',  '.',  '.',  '.',  '.',  '.',  '.',  '.',  '.',  ':',  '#',  '@',  '\'', '=',  '"',
    /*80-8F*/ '.',  'a',  'b',  'c',  'd',  'e',  'f',  'g',  'h',  'i',  '.',  '.',  '.',  '.',  '.',  '.',
    /*90-9F*/ '.',  'j',  'k',  'l',  'm',  'n',  'o',  'p',  'q',  'r',  '.',  '.',  '.',  '.',  '.',  '.',
    /*A0-AF*/ '.',  '~',  's',  't',  'u',  'v',  'w',  'x',  'y',  'z',  '.',  '.',  '.',  '.',  '.',  '.',
    /*B0-BF*/ '^', 0xA3,  '.',  '.',  '.',  '.',  '.',  '.',  '.',  '.',  '[',  ']',  '.',  '.',  '.',  '.',
    /*C0-CF*/ '{',  'A',  'B',  'C',  'D',  'E',  'F',  'G',  'H',  'I',  '.',  '.',  '.',  '.',  '.',  '.',
    /*D0-DF*/ '}',  'J',  'K',  'L',  'M',  'N',  'O',  'P',  'Q',  'R',  '.',  '.',  '.',  '.',  '.',  '.',
    /*E0-EF*/ '\\', '.',  'S',  'T',  'U',  'V',  'W',  'X',  'Y',  'Z',  '.',  '.',  '.',  '.',  '.',  '.',
    /*F0-FF*/ '0',  '1',  '2',  '3',  '4',  '5',  '6',  '7',  '8',  '9',  '.',  '.',  '.',  '.',  '.',  '.'
};

/* Convert EBCDIC byte to printable ASCII character */
static char ebcdic_to_printable(unsigned char c) {
    unsigned char ascii = ebcdic_to_ascii[c];
    if (isprint(ascii) && ascii != 0x7F) {
        return ascii;
    }
    return '.';
}

/* Print a 16-byte chunk in the specified format */
static void print_chunk(unsigned char *data, int offset, int len) {
    int i;
    
    /* Print offset */
    printf("%04X:", offset);
    
    /* Print hex representation in 4 groups of 4 bytes */
    printf("  ");
    for (i = 0; i < 16; i++) {
        if (i > 0 && i % 4 == 0) {
            printf(" ");
        }
        if (i < len) {
            printf("%02X", data[i]);
        } else {
            printf("  ");
        }
    }
    
    /* Print EBCDIC character representation */
    printf("  ");
    for (i = 0; i < len; i++) {
        printf("%c", ebcdic_to_printable(data[i]));
    }
    printf("\n");
}

int main(void) {
    uint32_t file_offset = 0;
    uint32_t record_count = 0;
    uint64_t total_bytes = 0;
    int incomplete_record = 0;
    
    while (1) {
        unsigned char len_bytes[2];
        size_t bytes_read;
        uint16_t reclen;
        unsigned char *record_data;
        size_t data_len;
        int i;
        
        /* Read the 2-byte record length (big-endian) */
        bytes_read = fread(len_bytes, 1, 2, stdin);
        
        if (bytes_read == 0) {
            /* End of file */
            break;
        }
        
        if (bytes_read < 2) {
            /* Incomplete length field */
            incomplete_record = 1;
            break;
        }
        
        /* Convert big-endian 16-bit integer */
        reclen = (uint16_t)((len_bytes[0] << 8) | len_bytes[1]);
        
        record_count++;
        
        /* Print record header */
        printf("Record: %u  Length: %u  Offset: %08X\n", record_count, reclen, file_offset);
        
        file_offset += 2;
        total_bytes += 2;
        
        /* Calculate data length (reclen - 2) */
        if (reclen < 2) {
            data_len = 0;
        } else {
            data_len = reclen - 2;
        }
        
        /* Allocate buffer for full record (including length field) */
        record_data = (unsigned char *)malloc(reclen);
        if (record_data == NULL && reclen > 0) {
            fprintf(stderr, "Memory allocation error\n");
            exit(1);
        }
        
        /* Store length field at start of record */
        record_data[0] = len_bytes[0];
        record_data[1] = len_bytes[1];
        
        /* Read the record data */
        bytes_read = fread(record_data + 2, 1, data_len, stdin);
        
        if (bytes_read < data_len) {
            /* Incomplete record */
            incomplete_record = 1;
            reclen = 2 + bytes_read;  /* Adjust total record length */
        }
        
        file_offset += bytes_read;
        total_bytes += bytes_read;
        
        /* Print the record in 16-byte chunks */
        for (i = 0; i < reclen; i += 16) {
            int chunk_len = (reclen - i < 16) ? (reclen - i) : 16;
            print_chunk(record_data + i, i, chunk_len);
        }
        
        free(record_data);
        
        if (incomplete_record) {
            break;
        }
    }
    
    /* Print end of file message */
    if (incomplete_record) {
        printf("Last record incomplete\n");
    }
    
    printf("End of file: %u record%s, %llu byte%s (%08llX)\n",
           record_count, (record_count == 1) ? "" : "s",
           (unsigned long long)total_bytes, (total_bytes == 1) ? "" : "s",
           (unsigned long long)total_bytes);
    
    return 0;
}

