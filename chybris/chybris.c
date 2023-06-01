#include <Python.h>
static char module_docstring[] = "This module provides an interface for doing vector subtraction.";
static char vector_sub_docstring[] = "Perform vector subtraction.";

static PyMethodDef module_methods[] = {{NULL, NULL, 0, NULL}};

static struct PyModuleDef module = {
    PyModuleDef_HEAD_INIT,
    "chybris",
    module_docstring,
    -1,
    module_methods,
};

PyMODINIT_FUNC
PyInit_chybris(void)
{
    return PyModule_Create(&module);
}
#include "lib.c"