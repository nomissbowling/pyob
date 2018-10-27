/*
  test_fractal.cpp

// set PYTHONHOME=...\Anaconda3

// set QT_QPA_PLATFORM_PLUGIN_PATH=%PYTHONHOME%/Library/plugins/platforms
// (OR)
// cp %PYTHONHOME%/Library/plugins/platforms/qwindows.dll ./platforms/

// Menu -> V (View) -> E (Extra Window) -> N (Property Manager)
//   -> Release | x64 (new property sheet) -> python36lib
//   -> Release | Win32 (add) -> python36lib
//   -> Property -> Page
//     (include path) %PYTHONHOME%/include
//     (include path) %PYTHONHOME%/lib/site-packages/numpy/core/include
//     (link dll import lib) %PYTHONHOME%/libs/python36.lib

// not found "numpy/arrayobject.h"
// numpy.get_include() # %PYTHONHOME%/lib/site-packages/numpy/core/include

// This application failed to start because
//   it could not find or load the Qt platform plugin "windows" in "".
// Reinstalling the application may fix this problem.

// (VC) add to project: from ...\matplotlib_pyob
// include/matplotlibcpp.h, include/pyob.h, src/pyob.cpp, test/test_fractal.cpp
*/

//#ifdef _CONSOLE || _WINDOWS
#include "stdafx.h"
//#endif

#define _USE_MATH_DEFINES // M_PI = 3.14159265358979323846
#include <cmath>
#include <vector>
#include <tuple>
#include <map>
#include <unordered_map>
#include <functional>
#include <iomanip>
#include <iostream>
#include <stdio.h>

#include "pyob.h"
// #pragma comment(lib, "python36.lib")

using namespace std;
using namespace pyob;

void fractal_hata(void){
  PyMod np("numpy");
  PyMod plt("matplotlib.pyplot");

  PyBase fig = (plt|"figure")();

  PyBase ax = (fig|"add_subplot")(MKTPL(PYLNG(231)), {});
  PyBase x = (np|"arange")(MKTPL(PYDBL(1), PYDBL(4), PYDBL(3)), {});
  PyBase y = (np|"arange")(MKTPL(PYDBL(1), PYDBL(5), PYDBL(3)), {});
//  (ax|"plot")(MKTPL(x, y, PYSTR("g.")), {});
//  (plt|"pause")(MKTPL(PYDBL(0.005)), {});

//  (plt|"show")();
}

void test_fractal(void){
try{
//  PyBase::version();
//  PyBase::begin(L"fractal");
  fprintf(stdout, "test fractal\n");
  fprintf(stdout, "int [%08X]\n", sizeof(int));
  fprintf(stdout, "long [%08X]\n", sizeof(long));
  fprintf(stdout, "float [%08X]\n", sizeof(float));
  fprintf(stdout, "double [%08X]\n", sizeof(double));
  fprintf(stdout, "long double [%08X]\n", sizeof(long double));
  fractal_hata();
//  PyBase::end();
}catch(const std::exception &e){
  fprintf(stderr, "excaption[%s]\n", e.what());
}
}
