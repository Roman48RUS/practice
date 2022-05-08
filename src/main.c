#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/a51.h"
#include "../include/encoder.h"
#include "../include/decoder.h"


int main(int argc, char **argv)
{
    typedef unsigned char byte;
    char* command = argv[1];   
    char* outputFile = "output";


    if (strcmp(command, "-cf") == 0)
    {
        char* filename = argv[2];
        if (filename == NULL)
        {
            printf("Enter filename after -c\n");
            return 1;
        }
        else
        {
            if (argc == 5)
            {
                if (strcmp(argv[3], "-o") == 0)
                {
                    outputFile = argv[4];
                    if (outputFile == NULL)
                    {
                        printf("Enter filename after -o\n");
                        return 2;
                    }
                }
            }
        }
        byte key[8] = {0x12, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF};
        word frame = 0x001;
        FILE* in = fopen(filename, "rb");
        FILE* out = fopen(outputFile, "wb");
        fseek(in, 0, SEEK_END);
        int size = ftell(in);
        rewind(in);
        byte data[size];
        byte encodedData[size];
        fread(data, 1, size, in);
        encode(data, size, key, frame, encodedData);
        fwrite(encodedData, 1, size, out);
        fclose(in);
        fclose(out);
    }
    if (strcmp(command, "-ca") == 0)
    {
        char* filename = argv[2];
        if (filename == NULL)
        {
            printf("Enter filename after -c\n");
            return 1;
        }
        else
        {
            if (argc == 5)
            {
                if (strcmp(argv[3], "-o") == 0)
                {
                    outputFile = argv[4];
                    if (outputFile == NULL)
                    {
                        printf("Enter filename after -o\n");
                        return 2;
                    }
                }
            }
        }

        compress(filename, "temp");

        byte key[8] = {0x12, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF};
        word frame = 0x001;

        byte header[44];
        FILE* in = fopen(filename, "rb");
        fread(header, 1, 44, in);

        
        in = fopen("temp", "rb");
        FILE* out = fopen(outputFile, "wb");

        fseek(in, 0, SEEK_END);
        int size = ftell(in);
        rewind(in);

        byte audio[size];
        fread(audio, 1, size, in);

        byte data[size + 44];
        for (int i = 0; i < 44; i++)
        {
            data[i] = header[i];
        }
        for (int i = 0; i < size; i++)
        {
            data[i + 44] = audio[i];
        }

        byte encodedData[size];

        
        encode(data, size + 44, key, frame, encodedData);
        fwrite(encodedData, 1, size + 44, out);
        remove("temp");
        fclose(in);
        fclose(out);
    }
    if (strcmp(command, "-da") == 0)
    {
        char* filename = argv[2];
        if (filename == NULL)
        {
            printf("Enter filename after -c\n");
            return 1;
        }
        else
        {
            if (argc == 5)
            {
                if (strcmp(argv[3], "-o") == 0)
                {
                    outputFile = argv[4];
                    if (outputFile == NULL)
                    {
                        printf("Enter filename after -o\n");
                        return 2;
                    }
                }
            }
        }
        byte key[8] = {0x12, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF};
        word frame = 0x001;

        FILE* in = fopen(filename, "rb");
        fseek(in, 0, SEEK_END);
        unsigned size = ftell(in);
        rewind(in);
        byte encryptedData[size];
        fread (encryptedData, 1, size, in);

        byte data[size];
        encode(encryptedData, size, key, frame, data);

        byte header[44];
        byte audio[size - 44];
        for (int i = 0; i < 44; i++)
            header[i] = data[i];
        for (int i = 0; i < size; i++)
            audio[i] = data[44 + i];
        
        FILE* temp = fopen("temp", "wb");
        fwrite(audio, 1, size - 44, temp);

        FILE* output = fopen(outputFile, "wb");
        fwrite(header, 1, 44, output);

        decompress("temp", outputFile);
        remove("temp");
    }





    return 0;
}