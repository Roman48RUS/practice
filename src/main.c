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
        FILE* keyfile = fopen("key", "rb");
        byte key[8];
        word frame;
        fread(key, 1, 8, keyfile);
        fread(&frame, 1, 4, keyfile);
        fclose(keyfile);

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

        FILE* keyfile = fopen("key", "rb");
        byte key[8];
        word frame;
        fread(key, 1, 8, keyfile);
        fread(&frame, 1, 4, keyfile);
        fclose(keyfile);


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
        FILE* keyfile = fopen("key", "rb");
        byte key[8];
        word frame;
        fread(key, 1, 8, keyfile);
        fread(&frame, 1, 4, keyfile);
        fclose(keyfile);

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
    if (strcmp(command, "-skf") == 0)
    {
        if (argc != 11)
        {
            printf("Incorrect arguments number\n");
            return 1;
        }

        byte key[8];
        for (int i = 0; i < 8; i++)
        {
            if (sscanf(argv[i + 2], "%02X", &key[i]) == 0)
            {
                printf("Incorrect values\n");
                return 3;
            }
        }
        word frame;
        if (sscanf(argv[10], "%d", &frame) == 0)
        {
            printf("Incorrect frame number");
            return 3;
        }

        FILE* keyfile = fopen("key", "wb");
        fwrite(key, 1, 8, keyfile);
        fwrite(&frame, 4, 1, keyfile);
        printf("Key and frame number set.\n");
    }




    return 0;
}