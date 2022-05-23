include config.mk
#
# Makefile for unity build (single translation unit) of hybris library.
#
ifeq ($(OS),Windows_NT) 
    DETECTED_OS := Windows
    TARGET:=win64
    CFLAGS+= -mconsole
else
    DETECTED_OS := $(shell sh -c 'uname 2>/dev/null || echo Unknown')
    TARGET:=unix
endif

ifeq ($(RELMODE), Release)
	CFLAGS+= -O3 -march=native -DNDEBUG
else ifeq ($(RELMODE), RelWDbgInfo)
	CFLAGS+= -O3 -march=native -DNDEBUG -DLOGLEVEL_INFO
else ifeq ($(RELMODE), RelPortable)
	CFLAGS+= -O2 -DNDEBUG -DLOGLEVEL_INFO
endif


prepare:
	$(call mkdir, ./bin/${TARGET}/obj) 
	

shared: prepare
	gcc -shared lib/lib.c -fPIC -o ./bin/${TARGET}/lib${LIBNAME}.so -fmax-errors=1 -Wall -DPSO_EVEN_MORE_COMBINATIONS ${CFLAGS}

static: prepare
	gcc -c lib/lib.c -fPIC -o ./bin/${TARGET}/obj/temp.o -fmax-errors=1 -Wall -DPSO_EVEN_MORE_COMBINATIONS ${CFLAGS}
	ar rcs ./bin/${TARGET}/lib${LIBNAME}.a ./bin/${TARGET}/obj/temp.o

standalone: static
	gcc standalones/prog_${PROGRAM}.c ./bin/${TARGET}/lib${LIBNAME}.a -o ./bin/${TARGET}/${PROGRAM}.exe -I ./lib/ -lm

install:
	$(call mkdir, ${PREFIX}/include/)
	$(call mkdir, ${PREFIX}/lib/)
	cp lib/*.h ${PREFIX}/include/
	cp bin/unix/libhybris.a ${PREFIX}/lib/


venv: venv/touchfile

venv/touchfile: requirements.txt
	test -d venv || virtualenv venv
	. venv/bin/activate; pip install -Ur requirements.txt
	touch venv/touchfile
	find -iname "*.pyc" -delete

python_dist:
	python setup.py bdist_wheel

.PHONY: lib stage


distribute:
	python setup.py bdist_wheel --universal