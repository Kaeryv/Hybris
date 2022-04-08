include config.mk
#
# Makefile for unity build (single translation unit) of hybris library.
#
ifeq ($(OS),Windows_NT) 
    DETECTED_OS := Windows
    TARGET:=win64
    CFLAGS+= -mconsole
    DIR_CMD:= mkdir
else
    DETECTED_OS := $(shell sh -c 'uname 2>/dev/null || echo Unknown')
    TARGET:=unix
    DIR_CMD:= mkdir -p
endif

ifeq ($(RELMODE), Release)
	CFLAGS+= -O3 -march=native -DNDEBUG
else ifeq ($(RELMODE), RelWDbgInfo)
	CFLAGS+= -O3 -march=native -DNDEBUG -DLOGLEVEL_INFO
endif


prepare:
	${DIR_CMD} ./bin/${TARGET}/obj/
	${DIR_CMD} ./bin/${TARGET}/obj/

shared: prepare
	gcc -shared lib/lib.c -fPIC -o ./bin/${TARGET}/lib${LIBNAME}.so -fmax-errors=1 -Wall -DPSO_EVEN_MORE_COMBINATIONS

static: prepare
	gcc -c lib/lib.c -fPIC -o ./bin/${TARGET}/obj/temp.o -fmax-errors=1 -Wall -DPSO_EVEN_MORE_COMBINATIONS ${CFLAGS}
	ar rcs ./bin/${TARGET}/lib${LIBNAME}.a ./bin/${TARGET}/obj/temp.o

standalone: static
	gcc standalones/prog_${PROGRAM}.c ./bin/${TARGET}/lib${LIBNAME}.a -o ./bin/${TARGET}/${PROGRAM}.exe -I ./lib/

install:
	${DIR_CMD} ${PREFIX}/include/
	${DIR_CMD} ${PREFIX}/lib/
	cp lib/*.h ${PREFIX}/include/
	cp bin/unix/libhybris.a ${PREFIX}/lib/


clean:
	rm -fr ./bin/
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

.PHONY: lib stage
