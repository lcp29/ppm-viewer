
CC := gcc
EMCC := emcc

all: win html

win:
	$(CC) ppm-viewer.c -o ppm-viewer -lSDL2 -O2

html:
	$(EMCC) ppm-viewer.c -o web/ppm-viewer.js \
		-sEXPORTED_FUNCTIONS=_pnmDecode,_getWidth,_getHeight,_malloc,_free \
		-sEXPORTED_RUNTIME_METHODS=ccall,cwrap \
		-sUSE_SDL=2 \
		-sALLOW_MEMORY_GROWTH
