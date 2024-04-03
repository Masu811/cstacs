#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <limits.h>
#include <assert.h>

static PyObject*
sum_list(PyObject* self, PyObject* args)
{
    PyObject* list;

    if (!PyArg_ParseTuple(args, "O", &list)) {
        return NULL;
    }

    Py_ssize_t i, n;
    long total = 0, value;
    PyObject* item;

    n = PyList_Size(list);
    if (n < 0)
        return NULL;
    for (i = 0; i < n; i++) {
        item = PyList_GetItem(list, i);
        //if (!PyLong_Check(item)) continue;
        value = PyLong_AsLong(item);
        if (value == -1 && PyErr_Occurred()) {
            return NULL;
        }
        total += value;
    }
    return Py_BuildValue("l", total);
}

static void
swap(long* a, long* b)
{
    if (a != b) {
        *a ^= *b;
        *b ^= *a;
        *a ^= *b;
    }
}

static PyObject*
insertionSort(PyObject* self, PyObject* args)
{
    PyObject* list;

    if (!PyArg_ParseTuple(args, "O", &list)) {
        return NULL;
    }

    Py_ssize_t i, n;
    long total = 0, value;
    PyObject* item;

    n = PyList_Size(list);
    if (n < 0) {
        return NULL;
    }

    long* array = (long*)malloc(n * sizeof(long));

    for (i = 0; i < n; i++) {
        item = PyList_GetItem(list, i);
        if (!PyLong_Check(item)) {
            continue;
        }
        value = PyLong_AsLong(item);
        if (value == -1 && PyErr_Occurred()) {
            return NULL;
        }
        array[i] = value;
    }
    
    long j;

    for (i = 1; i < n; i++) {
        if (array[i] < array[i-1]) {
            j = i;
            do {
                swap(&array[j], &array[j - 1]);
                j--;
            } while (j > 0 && array[j] < array[j - 1]);
        }
    }

    PyObject* py_list = PyList_New(n);
    if (!py_list) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to create Python list");
        return NULL;
    }
    for (i = 0; i < n; i++) {
        PyObject* py_int = PyLong_FromLong(array[i]);
        if (!py_int) {
            Py_DECREF(py_list); // Clean up the list
            PyErr_SetString(PyExc_RuntimeError, "Failed to convert C long to Python integer");
            return NULL;
        }
        PyList_SetItem(py_list, i, py_int);
    }
    free(array);

    return py_list;
}

static PyMethodDef example_methods[] = {
    {"sum_list", sum_list, METH_VARARGS,
        "Calculate the sum of integers in a list"},
    {"sort", insertionSort, METH_VARARGS,
        "Sort a given list (a copy is returned)"},
    {NULL, NULL, 0, NULL}
};


static struct PyModuleDef test_module = {
    PyModuleDef_HEAD_INIT,
    "test",
    NULL,
    -1,
    example_methods
};

PyMODINIT_FUNC
PyInit_test(void)
{
    return PyModule_Create(&test_module);
}

