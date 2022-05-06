//
// Created by licha on 2022/5/4.
//

#include <SDL2/SDL.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>

#define u8              unsigned char

#define PNM_UNKNOWN     0
#define PNM_P1          1
#define PNM_P2          2
#define PNM_P3          3
#define PNM_P4          4
#define PNM_P5          5
#define PNM_P6          6

int getPnmType(char *buf)
{
    if (buf[0] == 'P')
    {
        if (buf[1] >= '1' && buf[1] <= '6')
            return buf[1] - '0';
    }
    return PNM_UNKNOWN;
}

int getMetaData(const char *file, int *width, int *height, int *maxv, int fileSize)
{
    int fi = 2;
    char skippingComment = 0;
    char numParsed = 0;
    char parsingNumber = 0;
    int num = 0;
    while (fi < fileSize)
    {
        if (file[fi] == '#')
        {
            skippingComment = 1;
            if (parsingNumber == 1)
            {
                if (numParsed == 0)
                {
                    *width = num;
                    numParsed++;
                } else if (numParsed == 1)
                {
                    *height = num;
                    numParsed++;
                    if (maxv == NULL) break;
                } else
                {
                    *maxv = num;
                    break;
                }
                parsingNumber = 0;
            }
            fi++;
            continue;
        }
        if (skippingComment == 1)
        {
            if (file[fi] == '\n')
                skippingComment = 0;
            fi++;
            continue;
        }
        if (!isdigit((u8) file[fi]))
        {
            if (parsingNumber == 1)
            {
                if (numParsed == 0)
                {
                    *width = num;
                    numParsed++;
                } else if (numParsed == 1)
                {
                    *height = num;
                    numParsed++;
                    if (maxv == NULL) break;
                } else
                {
                    *maxv = num;
                    break;
                }
                parsingNumber = 0;
            }
            fi++;
            continue;
        }
        if (isdigit((u8) file[fi]))
        {
            if (parsingNumber)
            {
                num *= 10;
                num += (u8) file[fi] - '0';
            } else
            {
                num = (u8) file[fi] - '0';
                parsingNumber = 1;
            }
            fi++;
            continue;
        }
    }
    return fi;
}

void decodeP1(u8 *out, char *file, long fileSize)
{
    int width, height;
    int fi = getMetaData(file, &width, &height, NULL, fileSize);
    int oi = 0;
    char jumpingComment = 0;
    while (fi < fileSize && oi < width * height * 3)
    {
        if (file[fi] == '#')
        {
            jumpingComment = 1;
            fi++;
            continue;
        }
        if (jumpingComment)
        {
            if (file[fi] == '\n')
                jumpingComment = 0;
            fi++;
            continue;
        }
        if (file[fi] == '0')
        {
            out[oi] = 0;
            out[oi + 1] = 0;
            out[oi + 2] = 0;
            oi += 3;
            fi++;
            continue;
        } else if (file[fi] == '1')
        {
            out[oi] = 255;
            out[oi + 1] = 255;
            out[oi + 2] = 255;
            oi += 3;
            fi++;
            continue;
        } else
        {
            fi++;
            continue;
        }
    }
}

void decodeP2(u8 *out, char *file, long fileSize)
{
    int width, height;
    int maxv;
    int fi = getMetaData(file, &width, &height, &maxv, fileSize);
    int oi = 0;
    int num = 0;
    char jumpingComment = 0;
    char numberPushed = 0;
    while (fi < fileSize && oi < width * height * 3)
    {
        if (file[fi] == '#')
        {
            jumpingComment = 1;
            fi++;
            if (numberPushed)
            {
                u8 pixVal = (u8) ((float) num / (float) maxv * 255);
                out[oi] = pixVal;
                out[oi + 1] = pixVal;
                out[oi + 2] = pixVal;
                oi += 3;
                numberPushed = 0;
            }
            continue;
        }
        if (jumpingComment)
        {
            if (file[fi] == '\n')
                jumpingComment = 0;
            fi++;
            continue;
        }
        if (!isdigit((u8) file[fi]))
        {
            if (numberPushed)
            {
                u8 pixVal = (u8) ((float) num / (float) maxv * 255);
                out[oi] = pixVal;
                out[oi + 1] = pixVal;
                out[oi + 2] = pixVal;
                oi += 3;
                numberPushed = 0;
            }
            fi++;
            continue;
        }
        if (isdigit((u8) file[fi]))
        {
            if (!numberPushed)
            {
                num = (u8) file[fi] - '0';
                numberPushed = 1;
            } else
            {
                num *= 10;
                num += (u8) file[fi] - '0';
            }
            fi++;
            continue;
        }
    }
}

void decodeP3(u8 *out, char *file, long fileSize)
{
    int width, height;
    int maxv;
    int fi = getMetaData(file, &width, &height, &maxv, fileSize);
    int oi = 0;
    int num = 0;
    char jumpingComment = 0;
    char numberPushed = 0;
    while (fi < fileSize && oi < width * height * 3)
    {
        if (file[fi] == '#')
        {
            jumpingComment = 1;
            fi++;
            if (numberPushed)
            {
                out[oi++] = (u8) ((float) num / (float) maxv * 255);
                numberPushed = 0;
            }
            continue;
        }
        if (jumpingComment)
        {
            if (file[fi] == '\n')
                jumpingComment = 0;
            fi++;
            continue;
        }
        if (!isdigit((u8) file[fi]))
        {
            if (numberPushed)
            {
                out[oi++] = (u8) ((float) num / (float) maxv * 255);
                numberPushed = 0;
            }
            fi++;
            continue;
        }
        if (isdigit((u8) file[fi]))
        {
            if (!numberPushed)
            {
                num = (u8) file[fi] - '0';
                numberPushed = 1;
            } else
            {
                num *= 10;
                num += (u8) file[fi] - '0';
            }
            fi++;
            continue;
        }
    }
}

void decodeP4(u8 *out, char *file, long fileSize)
{
    int width, height;
    int fi = getMetaData(file, &width, &height, NULL, fileSize);
    int oi = 0;
    for (fi++; fi < fileSize - 1 && oi < width * height * 3 - 24; fi++, oi += 24)
    {
        for (int b = 0; b < 8; b++)
        {
            if (file[fi] & (u8) (0b10000000 >> b))
            {
                out[oi + 2 + 3 * b] = 255;
                out[oi + 1 + 3 * b] = 255;
                out[oi + 3 * b] = 255;
            } else
            {
                out[oi + 2 + 3 * b] = 0;
                out[oi + 1 + 3 * b] = 0;
                out[oi + 3 * b] = 0;
            }
        }
    }
    int lastBits = (width * height) % 8;
    if (lastBits == 0) lastBits = 8;
    for (int b = 0; b < lastBits && oi < width * height * 3; b++)
    {
        if (file[fi] & (u8) (0b10000000 >> b))
        {
            out[oi + 2 + 3 * b] = 255;
            out[oi + 1 + 3 * b] = 255;
            out[oi + 3 * b] = 255;
        } else
        {
            out[oi + 2 + 3 * b] = 0;
            out[oi + 1 + 3 * b] = 0;
            out[oi + 3 * b] = 0;
        }
    }
}

void decodeP5(u8 *out, char *file, long fileSize)
{
    int width, height;
    int maxv;
    int fi = getMetaData(file, &width, &height, &maxv, fileSize);
    int oi = 0;
    for (fi++; fi < fileSize && oi < width * height * 3; fi++, oi += 3)
    {
        u8 pixVal = (u8) ((float) file[fi] / maxv * 255);
        out[oi] = pixVal;
        out[oi + 1] = pixVal;
        out[oi + 2] = pixVal;
    }
}

void decodeP6(u8 *out, char *file, long fileSize)
{
    int width, height;
    int maxv;
    int fi = getMetaData(file, &width, &height, &maxv, fileSize);
    int oi = 0;
    for (fi++; fi < fileSize && oi < width * height * 3; fi++, oi++)
    {
        out[oi] = (u8) ((float) file[fi] / maxv * 255);
    }
}

long getFileSize(FILE *pf)
{
    long initPos = ftell(pf);
    fseek(pf, 0, SEEK_END);
    long fileSize = ftell(pf);
    fseek(pf, initPos, SEEK_SET);
    return fileSize;
}

int main(int argc, char *argv[])
{
    if (argc == 1)
    {
        printf("usage: ppm-viewer <FILE>\n");
        return 0;
    }
    FILE *pFile = fopen(argv[1], "r");
    if (pFile == NULL)
    {
        printf("unable to open file %s.\n", argv[1]);
        return 1;
    }

    // read file to buffer
    long fileSize = getFileSize(pFile);
    char *fileBuf = (char *) malloc(fileSize + 1);
    if (fileBuf == NULL)
    {
        return 1;
    }
    fread(fileBuf, 1, fileSize, pFile);

    // get file type
    int fileType = getPnmType(fileBuf);
    if (fileType == PNM_UNKNOWN)
    {
        return 1;
    }

    // get file size
    int width, height;
    int beginIndex = 0;
    getMetaData(fileBuf, &width, &height, NULL, fileSize);

    // decode
    long imgSize = width * height * 3;
    u8 *out = (u8 *) malloc(imgSize);
    if (out == NULL)
    {
        return 1;
    }

    void (*decoders[6])(u8 *, char *, long) = {
            decodeP1, decodeP2, decodeP3, decodeP4, decodeP5, decodeP6
    };

    decoders[fileType - 1](out, fileBuf, fileSize);

    // display
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window *window = SDL_CreateWindow(
            "Press Q to quit",
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            width,
            height,
            SDL_WINDOW_SHOWN);

    SDL_Surface *surface = SDL_GetWindowSurface(window);
    u8 *surfaceBuf = (u8 *) surface->pixels;
    SDL_LockSurface(surface);
    for (int i = 0; i < width * height; ++i)
    {
        surfaceBuf[i * 4] = out[i * 3 + 2];
        surfaceBuf[i * 4 + 1] = out[i * 3 + 1];
        surfaceBuf[i * 4 + 2] = out[i * 3];
    }
    SDL_UnlockSurface(surface);
    SDL_UpdateWindowSurface(window);
    char quit = 0;
    while (!quit)
    {
        SDL_Event e;
        if (SDL_PollEvent(&e) != 0)
        {
            if (e.type == SDL_QUIT)
            {
                quit = 1;
            } else if (e.type == SDL_KEYDOWN)
            {
                if (e.key.keysym.sym == SDLK_q)
                    quit = 1;
            }
        }
    }
    SDL_FreeSurface(surface);
    SDL_DestroyWindow(window);

    free(fileBuf);
    free(out);

    return 0;
}
