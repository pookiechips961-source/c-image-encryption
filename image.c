#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("Invalid number of arguments!\nEnsure exactly 1 image and 1 key is passed at a time.\n");
        return 1;
    }
    int key = atoi(argv[2]);

    char tempPPM[256];
    snprintf(tempPPM, sizeof(tempPPM), "%s.ppm", argv[1]);
    char toppm[1024];
    snprintf(toppm, sizeof(toppm), "ffmpeg -hide_banner -loglevel error -i \"%s\" -pix_fmt rgb24 -update 1 -frames:v 1 \"%s\"", argv[1], tempPPM);
    int convert = system(toppm);
    if(convert != 0)
    {
        printf("ffmpeg failed to convert given image");
        return 1;
    }


    FILE *fp = fopen(tempPPM, "r+b");
    if (fp == NULL)
    {
        printf("Error: Failed to open temporary file for processing.\n");
        return 1;
    }

    char buffer[256];
    char res[128];
    size_t w, h;

    if (!fgets(buffer, sizeof(buffer), fp))
    {
        printf("Error: Could not read magic number.\n");
        fclose(fp);
        return 1;
    }
    while (fgets(res, sizeof(res), fp))
    {
        if (res[0] != '#')
        {
            break;
        }
    }
    if (!fgets(buffer, sizeof(buffer), fp))
    {
        printf("Error: Could not read color bit-depth.\n");
        fclose(fp);
        return 1;
    }
    if (sscanf(res, "%zu %zu", &w, &h) != 2 || w <= 0 || h <= 0)
    {
        printf("Error: Invalid or unparseable image dimensions.\n");
        fclose(fp);
        return 1;
    }

    size_t bytes = w*h*3;
    unsigned char *pixels = malloc(bytes);
    if (pixels == NULL)
    {
        printf("Memory allocation failed!\n");
        fclose(fp);
        return 1;
    }
    fread(pixels, 1, bytes, fp);

    srand(key);
    for (size_t i = 0; i < bytes; i++)
    {
        pixels[i] ^= (rand() % 256);
    }

    fclose(fp);
    fp = fopen(tempPPM, "wb");
    if (fp == NULL)
    {
        printf("Error: Failed to open temporary file for writing.\n");
        free(pixels);
        return 1;
    }
    fprintf(fp, "P6\n%zu %zu\n255\n", w, h);
    fwrite(pixels, 1, bytes, fp);
    free(pixels);
    fclose(fp);

    char output[256];

    if (strncmp(argv[1], "encrypted", 9) == 0)
    {
        snprintf(output, sizeof(output), "%s", argv[1] + 9);
    }
    else
    {
        snprintf(output, sizeof(output), "encrypted_%s.png", argv[1]);
    }

    char topng[1024];
    snprintf(topng, sizeof(topng), "ffmpeg -hide_banner -loglevel error -y -i \"%s\" -update 1 -frames:v 1 \"%s\"",tempPPM, output);
    convert = system(topng);
    if(convert != 0)
    {
        printf("ffmpeg failed to convert to png");
        remove(tempPPM);
        return 1;
    }

    remove(tempPPM);

    return 0;
}
