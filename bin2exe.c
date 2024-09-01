#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define MAX_SIZE 0x200000
#define BUFFER_SIZE 0x2000

void print_usage() {
    fprintf(stderr, "Usage: bin2exe infile outfile\n");
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        print_usage();
        return 1;
    }

    FILE *ifile = fopen(argv[1], "rb");
    if (ifile == NULL) {
        fprintf(stderr, "Error opening input file %s\n", argv[1]);
        return 1;
    }

    fseek(ifile, 0, SEEK_END);
    size_t infile_size = ftell(ifile);
    fseek(ifile, 0, SEEK_SET);

    if (infile_size > MAX_SIZE) {
        fprintf(stderr, "Error: Input file %s longer than %d bytes\n", argv[1], MAX_SIZE);
        fclose(ifile);
        return 1;
    }

    FILE *ofile = fopen(argv[2], "wb");
    if (ofile == NULL) {
        fprintf(stderr, "Error opening output file %s\n", argv[2]);
        fclose(ifile);
        return 1;
    }

    // Write header
    fwrite("PS-X EXE", sizeof(char), 8, ofile);
    fseek(ofile, 0x10, SEEK_SET);
    uint32_t number = 0x80010000;
    fwrite(&number, sizeof(uint32_t), 1, ofile);
    number = 0xFFFFFFFF;
    fwrite(&number, sizeof(uint32_t), 1, ofile);
    number = 0x80010000;
    fwrite(&number, sizeof(uint32_t), 1, ofile);
    fseek(ofile, 0x30, SEEK_SET);
    number = 0x801FFF00;
    fwrite(&number, sizeof(uint32_t), 1, ofile);
    fseek(ofile, 0x800, SEEK_SET);

    // Copy input to output
    char buffer[BUFFER_SIZE];
    size_t bytes_read;
    while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, ifile)) > 0) {
        fwrite(buffer, 1, bytes_read, ofile);
    }

    // Pad binary to 0x800 boundary
    long exe_size = ftell(ofile);
    if (exe_size % 0x800 != 0) {
        exe_size += (0x800 - (exe_size % 0x800));
        fseek(ofile, exe_size - 1, SEEK_SET);
        fputc(0, ofile); // fputc is to put a character in the buffer. I was using fwrite and that was making stuff be a mess...
    }

    // Filesize excluding 0x800 byte header
    fseek(ofile, 0x1C, SEEK_SET);
    uint32_t file_size_ex_header = exe_size - 0x800;
    fwrite(&file_size_ex_header, sizeof(uint32_t), 1, ofile);

    fclose(ifile);
    fclose(ofile);

    return 0;
}
