/*
 * This is bin2c program, which allows you to convert binary file to
 * C language array, for use as embedded resource, for instance you can
 * embed graphics or audio file directly into your program.
 * This is public domain software, use it on your own risk.
 * Contact Serge Fukanchik at fuxx@mail.ru  if you have any questions.
 *
 * Some modifications were made by Gwilym Kuiper (kuiper.gwilym@gmail.com)
 * I have decided not to change the licence.
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef USE_BZ2
#include <bzlib.h>
#endif

int
main(int argc, char *argv[])
{
    char *buf;
    char *ident;
    unsigned int i, file_size, need_comma;

    FILE *f_input, *f_output;

    if (argc < 4) {
        fprintf(stderr, "Usage: %s binary_file output_file array_name\n",
                argv[0]);
        return -1;
    }

    f_input = fopen(argv[1], "rb");
    if (f_input == NULL) {
        fprintf(stderr, "%s: can't open %s for reading\n", argv[0], argv[1]);
        return -1;
    }

    // Get the file length
    fseek(f_input, 0, SEEK_END);
    file_size = ftell(f_input);
    fseek(f_input, 0, SEEK_SET);

    buf = (char *) malloc(file_size);
    assert(buf);

    fread(buf, file_size, 1, f_input);
    fclose(f_input);


    f_output = fopen(argv[2], "w");
    if (f_output == NULL) {
        fprintf(stderr, "%s: can't open %s for writing\n", argv[0], argv[2]);
        return -1;
    }

    ident = argv[3];

    need_comma = 0;

    // fprintf(f_output, "const char %s[%i] = {", ident, file_size);
    fprintf(f_output, "const unsigned char %s[] = {", ident);
    for (i = 0; i < file_size; ++i) {
        if (need_comma)
            fprintf(f_output, ", ");
        else
            need_comma = 1;
        if ((i % 11) == 0)
            fprintf(f_output, "\n\t");
        fprintf(f_output, "0x%.2X", buf[i] & 0xff);
    }
    fprintf(f_output, "\n};\n\n");

    // fprintf(f_output, "const int %s_length = %i;\n", ident, file_size);


    fclose(f_output);

    return 0;
}
