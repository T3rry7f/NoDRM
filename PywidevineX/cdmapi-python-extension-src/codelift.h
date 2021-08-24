
#ifndef __LIBMYPY_H__
#define __LIBMYPY_H__

#include <Python.h>
 extern "C" {
PyObject * _decrypt(PyObject *, PyObject *);
PyObject * _encrypt(PyObject *, PyObject *);
}
#endif

// extern "C" {
//     const char* guessInput(const char* input);
//     const char* getOutput(const char* input);
//     const char* getDeoaep(const char* input);
//     void freeStr(void* str);
// }