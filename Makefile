include config.mk
#
# Makefile for unity build (single translation unit) of hybris library.
#

CFLAGS+=-Wno-gnu-designator

ifeq ($(OS),Windows_NT) 
    TARGET:=Windows
    CFLAGS+= -mconsole
else
    TARGET := $(shell sh -c 'uname 2>/dev/null || echo Unknown')
endif

ifeq ($(RELMODE), Release)
	CFLAGS+= -O3 -march=native -DNDEBUG
else ifeq ($(RELMODE), RelWDbgInfo)
	CFLAGS+= -O3 -march=native -DNDEBUG -DLOGLEVEL_INFO
else ifeq ($(RELMODE), RelPortable)
	CFLAGS+= -O2 -DNDEBUG 
endif

.PHONY: prepare


all: prepare static shared

prepare: 
	@echo "Building for $(TARGET)"
	mkdir -p ./bin/${TARGET}/obj
	

shared: prepare
	gcc -shared src/lib.c -fPIC -o ./bin/${TARGET}/lib${LIBNAME}.so -fmax-errors=1 -Wall -DPSO_EVEN_MORE_COMBINATIONS ${CFLAGS}

static: prepare
	gcc -c src/lib.c -fPIC -o ./bin/${TARGET}/obj/temp.o -fmax-errors=1 -Wall -DPSO_EVEN_MORE_COMBINATIONS ${CFLAGS}
	ar rcs ./bin/${TARGET}/lib${LIBNAME}.a ./bin/${TARGET}/obj/temp.o

standalone: static
	gcc standalones/prog_${PROGRAM}.c ./bin/${TARGET}/lib${LIBNAME}.a -o ./bin/${TARGET}/${PROGRAM}.exe -I ./src/ -lm $(CFLAGS)

install:
	$(call mkdir, ${PREFIX}/include/)
	$(call mkdir, ${PREFIX}/src/)
	cp src/*.h ${PREFIX}/include/
	cp bin/${TARGET}/libhybris.a ${PREFIX}/lib/


venv: venv/touchfile

venv/touchfile: requirements.txt
	test -d venv || virtualenv venv
	. venv/bin/activate; pip install -Ur requirements.txt
	touch venv/touchfile
	find -iname "*.pyc" -delete

python_dist:
	python setup.py bdist_wheel



distribute:
	python setup.py bdist_wheel --universal

clean:
	rm -rf hybris_py.egg-info
	rm -rf dist bin build

