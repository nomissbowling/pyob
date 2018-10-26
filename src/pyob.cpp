/*
  pyob.cpp
*/

//#ifdef _CONSOLE || _WINDOWS
#include "stdafx.h"
//#endif

#include "pyob.h"

namespace pyob {

PyObject *pyob::PyBase::mop = NULL; // dummy replaced after begin()

void PyBase::version(void){
  fprintf(stderr, "pyob[%lf]\n", PYOB_VERSION);
}

void PyBase::begin(const wchar_t *p){
try{
  // Py_SetProgramName(p);
  if(!Py_IsInitialized()) Py_Initialize();
  if(!pyob::PyBase::mop)
    pyob::PyBase::mop = PyImport_Import(pyob::PyStr("operator").o());
}catch(const std::exception &e){
  fprintf(stderr, "exception[%s]\n", e.what());
}
}

void PyBase::end(void){
try{
  if(Py_IsInitialized()) Py_Finalize();
}catch(const std::exception &e){
  fprintf(stderr, "exception[%s]\n", e.what());
}
}

}
