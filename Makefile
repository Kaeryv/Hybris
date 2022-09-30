#
# Makefile for unity build of hybris library.
#

include config.mk

CFLAGS+=-Wall -Wno-gnu-designator -DPSO_EVEN_MORE_COMBINATIONS 

ifeq ($(OS),Windows_NT) 
    TARGET:=Windows
    CFLAGS+= -mconsole
else
    TARGET := $(shell sh -c 'uname 2>/dev/null || echo Unknown')
endif

ifeq ($(CC),clang)
    CFLAGS+= -ferror-limit=1
else
    CFLAGS+= -fmax-errors=1
endif

ifeq ($(RELMODE), Release)
	CFLAGS+= -O3 -march=native -DNDEBUG
else ifeq ($(RELMODE), RelWDbgInfo)
	CFLAGS+= -O3 -march=native -DNDEBUG -DLOGLEVEL_INFO
else ifeq ($(RELMODE), RelPortable)
	CFLAGS+= -O2 -DNDEBUG 
endif
ifeq ($(CEC_BECNHMARK), True)
	CFLAGS+=-DADD_CECBENCHMARK
endif
.PHONY: prepare


all: prepare static shared

prepare: 
	@echo "Building for $(TARGET)"
	mkdir -p ./bin/${TARGET}/obj
	

shared: prepare
	$(CC) -shared src/lib.c -fPIC -o ./bin/${TARGET}/lib${LIBNAME}.so ${CFLAGS}

static: prepare
	$(CC) -c src/lib.c -fPIC -o ./bin/${TARGET}/obj/temp.o ${CFLAGS}
	ar rcs ./bin/${TARGET}/lib${LIBNAME}.a ./bin/${TARGET}/obj/temp.o

standalone: static
	$(CC) standalones/prog_${PROGRAM}.c ./bin/${TARGET}/lib${LIBNAME}.a -o ./bin/${TARGET}/${PROGRAM}.exe -I ./src/ -lm $(CFLAGS)

install:
	mkdir -p ${PREFIX}/include
	mkdir -p ${PREFIX}/lib
	cp src/*.h ${PREFIX}/include/
	cp bin/${TARGET}/libhybris.a ${PREFIX}/lib/

tarball:
	mkdir -p hybris_tarball
	cp -r hybris           hybris_tarball
	cp -r src              hybris_tarball
	cp    Makefile         hybris_tarball
	cp    config.mk        hybris_tarball
	cp    requirements.txt hybris_tarball
	cp -r tests            hybris_tarball
	cp -r standalones      hybris_tarball
	cp    setup.py         hybris_tarball

	tar -czvf hybris.tar.gz hybris_tarball

pip_package: shared
	python setup.py bdist_wheel --universal

clean:
	rm -rf hybris_py.egg-info
	rm -rf dist bin build

