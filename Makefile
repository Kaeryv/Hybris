#
# Makefile for unity build (single translation unit) of hybris library.
#
LIBNAME=hybris
ifeq ($(OS),Windows_NT) 
    DETECTED_OS := Windows
	TARGET=./bin/win64/
	CFLAGS+= -mconsole
else
    DETECTED_OS := $(shell sh -c 'uname 2>/dev/null || echo Unknown')
	TARGET=./bin/unix/
endif

ifeq ($(RELMODE), Release)
	CFLAGS+= -O3 -march=native -DNDEBUG
else ifeq ($(RELMODE), RelWDbgInfo)
	CFLAGS+= -O3 -march=native -DNDEBUG -DLOGLEVEL_INFO
endif



shared:
	gcc -shared lib/lib.c -fPIC -o ${TARGET}/lib${LIBNAME}.so -fmax-errors=1 -Wall -DPSO_EVEN_MORE_COMBINATIONS

static:
	gcc -c lib/lib.c -fPIC -o temp.o -fmax-errors=1 -Wall -DPSO_EVEN_MORE_COMBINATIONS ${CFLAGS}
	ar rcs ${TARGET}/lib${LIBNAME}.a temp.o

standalone: static
	gcc standalones/prog_${PROGRAM}.c ${TARGET}/lib${LIBNAME}.a -o ${TARGET}/${PROGRAM}.exe -I ./lib/


clean:
	rm -fr ${LIBNAME}
	rm -fr hybris/__pycache__/
	rm -fr __pycache__/


venv: venv/touchfile

venv/touchfile: requirements.txt
	test -d venv || virtualenv venv
	. venv/bin/activate; pip install -Ur requirements.txt
	touch venv/touchfile
	find -iname "*.pyc" -delete

purge: clean
	rm -fr env/ lib/*.so

.PHONY: lib
