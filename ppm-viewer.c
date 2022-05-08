//
// Created by licha on 2022/5/4.
//

#ifdef __EMSCRIPTEN__

#include <emscripten.h>
#define KEEPALIVE EMSCRIPTEN_KEEPALIVE

#else

#include <SDL2/SDL.h>
#define KEEPALIVE

#endif
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <ctype.h>
#include <stdint.h>

#define PNM_UNKNOWN     0
#define PNM_P1          1
#define PNM_P2          2
#define PNM_P3          3
#define PNM_P4          4
#define PNM_P5          5
#define PNM_P6          6


KEEPALIVE int getPnmType(int8_t *buf)
{
    if (buf[0] == 'P')
    {
        if (buf[1] >= '1' && buf[1] <= '6')
            return buf[1] - '0';
    }
    return PNM_UNKNOWN;
}

KEEPALIVE int getMetaData(const int8_t *file, int *width, int *height, int *maxv, int fileSize)
{
    int fi = 2;
    int8_t skippingComment = 0;
    int8_t numParsed = 0;
    int8_t parsingNumber = 0;
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
        if (!isdigit((uint8_t) file[fi]))
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
        if (isdigit((uint8_t) file[fi]))
        {
            if (parsingNumber)
            {
                num *= 10;
                num += (uint8_t) file[fi] - '0';
            } else
            {
                num = (uint8_t) file[fi] - '0';
                parsingNumber = 1;
            }
            fi++;
            continue;
        }
    }
    return fi;
}

KEEPALIVE void decodeP1(uint8_t *out, int8_t *file, long fileSize)
{
    int width, height;
    int fi = getMetaData(file, &width, &height, NULL, fileSize);
    int oi = 0;
    int8_t jumpingComment = 0;
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

KEEPALIVE void decodeP2(uint8_t *out, int8_t *file, long fileSize)
{
    int width, height;
    int maxv;
    int fi = getMetaData(file, &width, &height, &maxv, fileSize);
    int oi = 0;
    int num = 0;
    int8_t jumpingComment = 0;
    int8_t numberPushed = 0;
    while (fi < fileSize && oi < width * height * 3)
    {
        if (file[fi] == '#')
        {
            jumpingComment = 1;
            fi++;
            if (numberPushed)
            {
                uint8_t pixVal = (uint8_t) ((float) num / (float) maxv * 255);
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
        if (!isdigit((uint8_t) file[fi]))
        {
            if (numberPushed)
            {
                uint8_t pixVal = (uint8_t) ((float) num / (float) maxv * 255);
                out[oi] = pixVal;
                out[oi + 1] = pixVal;
                out[oi + 2] = pixVal;
                oi += 3;
                numberPushed = 0;
            }
            fi++;
            continue;
        }
        if (isdigit((uint8_t) file[fi]))
        {
            if (!numberPushed)
            {
                num = (uint8_t) file[fi] - '0';
                numberPushed = 1;
            } else
            {
                num *= 10;
                num += (uint8_t) file[fi] - '0';
            }
            fi++;
            continue;
        }
    }
}

KEEPALIVE void decodeP3(uint8_t *out, int8_t *file, long fileSize)
{
    int width, height;
    int maxv;
    int fi = getMetaData(file, &width, &height, &maxv, fileSize);
    int oi = 0;
    int num = 0;
    int8_t jumpingComment = 0;
    int8_t numberPushed = 0;
    while (fi < fileSize && oi < width * height * 3)
    {
        if (file[fi] == '#')
        {
            jumpingComment = 1;
            fi++;
            if (numberPushed)
            {
                out[oi++] = (uint8_t) ((float) num / (float) maxv * 255);
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
        if (!isdigit((uint8_t) file[fi]))
        {
            if (numberPushed)
            {
                out[oi++] = (uint8_t) ((float) num / (float) maxv * 255);
                numberPushed = 0;
            }
            fi++;
            continue;
        }
        if (isdigit((uint8_t) file[fi]))
        {
            if (!numberPushed)
            {
                num = (uint8_t) file[fi] - '0';
                numberPushed = 1;
            } else
            {
                num *= 10;
                num += (uint8_t) file[fi] - '0';
            }
            fi++;
            continue;
        }
    }
}

KEEPALIVE void decodeP4(uint8_t *out, int8_t *file, long fileSize)
{
    int width, height;
    int fi = getMetaData(file, &width, &height, NULL, fileSize);
    int oi = 0;
    for (fi++; fi < fileSize - 1 && oi < width * height * 3 - 24; fi++, oi += 24)
    {
        for (int b = 0; b < 8; b++)
        {
            if (file[fi] & (uint8_t) (0b10000000 >> b))
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
        if (file[fi] & (uint8_t) (0b10000000 >> b))
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

KEEPALIVE void decodeP5(uint8_t *out, int8_t *file, long fileSize)
{
    int width, height;
    int maxv;
    int fi = getMetaData(file, &width, &height, &maxv, fileSize);
    int oi = 0;
    for (fi++; fi < fileSize && oi < width * height * 3; fi++, oi += 3)
    {
        uint8_t pixVal = (uint8_t) ((float) file[fi] / maxv * 255);
        out[oi] = pixVal;
        out[oi + 1] = pixVal;
        out[oi + 2] = pixVal;
    }
}

KEEPALIVE void decodeP6(uint8_t *out, int8_t *file, long fileSize)
{
    int width, height;
    int maxv;
    int fi = getMetaData(file, &width, &height, &maxv, fileSize);
    int oi = 0;
    for (fi++; fi < fileSize && oi < width * height * 3; fi++, oi++)
    {
        out[oi] = (uint8_t) ((float) file[fi] / maxv * 255);
    }
}

#ifndef __EMSCRIPTEN__

long getFileSize(FILE *pf)
{
    long initPos = ftell(pf);
    fseek(pf, 0, SEEK_END);
    long fileSize = ftell(pf);
    fseek(pf, initPos, SEEK_SET);
    return fileSize;
}

#endif

KEEPALIVE void decode(int8_t *fileBuf, uint8_t *out, long fileSize)
{
    static void (*decoders[6])(uint8_t *, int8_t *, long) = {
            decodeP1, decodeP2, decodeP3, decodeP4, decodeP5, decodeP6
    };
    decoders[getPnmType(fileBuf) - 1](out, fileBuf, fileSize);
}

#ifndef __EMSCRIPTEN__

int uiMain(int argc, char *argv[])
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
    int8_t *fileBuf = (int8_t *) malloc(fileSize + 1);
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
    getMetaData(fileBuf, &width, &height, NULL, fileSize);

    // decode
    long imgSize = width * height * 3;
    uint8_t *out = (uint8_t *) malloc(imgSize);
    if (out == NULL)
    {
        return 1;
    }

    decode(fileBuf, out, fileSize);

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
    uint8_t *surfaceBuf = (uint8_t *) surface->pixels;
    SDL_LockSurface(surface);
    for (int i = 0; i < width * height; ++i)
    {
        surfaceBuf[i * 4] = out[i * 3 + 2];
        surfaceBuf[i * 4 + 1] = out[i * 3 + 1];
        surfaceBuf[i * 4 + 2] = out[i * 3];
    }
    SDL_UnlockSurface(surface);
    SDL_UpdateWindowSurface(window);
    int8_t quit = 0;
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
#else
KEEPALIVE void pnmDecode(int file, int out, int fileSize)
{
    int8_t *pFile = (int8_t*) file;
    uint8_t *pOut = (uint8_t*) out;
    int width, height;
    getMetaData(pFile, &width, &height, NULL, fileSize);
    decode(pFile, pOut, fileSize);
    for (int i = width * height - 1; i >= 0; i--)
    {
        pOut[i * 4 + 3] = 255;
        pOut[i * 4 + 2] = pOut[i * 3 + 2];
        pOut[i * 4 + 1] = pOut[i * 3 + 1];
        pOut[i * 4] = pOut[i * 3];
    }
}

KEEPALIVE int getWidth(int file, int fileSize)
{
    int width, height;
    getMetaData((int8_t *)file, &width, &height, NULL, fileSize);
    return width;
}

KEEPALIVE int getHeight(int file, int fileSize)
{
    int width, height;
    getMetaData((int8_t *)file, &width, &height, NULL, fileSize);
    return height;
}
#endif

int main(int argc, char *argv[])
{
#ifndef __EMSCRIPTEN__
    return uiMain(argc, argv);
#else
    return 0;
#endif
}
