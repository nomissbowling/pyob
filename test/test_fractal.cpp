/*
  test_fractal.cpp
*/

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

PyBase SC1(double r){
  PyMod np("numpy");
  return (np|"array")(MKTPL(PYTPL(
    PYTPL(PYDBL(r), PYDBL(0), PYDBL(0)),
    PYTPL(PYDBL(0), PYDBL(r), PYDBL(0)),
    PYTPL(PYDBL(0), PYDBL(0), PYDBL(1)))), {});
}

PyBase SC2(double rx, double ry){
  PyMod np("numpy");
  return (np|"array")(MKTPL(PYTPL(
    PYTPL(PYDBL(rx), PYDBL(0), PYDBL(0)),
    PYTPL(PYDBL(0), PYDBL(ry), PYDBL(0)),
    PYTPL(PYDBL(0), PYDBL(0), PYDBL(1)))), {});
}

PyBase SFT(double dx, double dy){
  PyMod np("numpy");
  return (np|"array")(MKTPL(PYTPL(
    PYTPL(PYDBL(1), PYDBL(0), PYDBL(dx)),
    PYTPL(PYDBL(0), PYDBL(1), PYDBL(dy)),
    PYTPL(PYDBL(0), PYDBL(0), PYDBL(1)))), {});
}

PyBase ROT(double a){
  PyMod np("numpy");
  PyBase c = (np|"cos")(MKTPL(PYDBL(a)), {});
  PyBase s = (np|"sin")(MKTPL(PYDBL(a)), {});
  return (np|"array")(MKTPL(PYTPL(
    PYTPL(PYOBJ(c), PYOBJ(-s), PYDBL(0)),
    PYTPL(PYOBJ(s), PYOBJ(c), PYDBL(0)),
    PYTPL(PYDBL(0), PYDBL(0), PYDBL(1)))), {});
}

void fractal(PyBase m, double s, int c,
  PyBase &ax, int brk, double r1, double r2, PyBase f1, PyBase f2,
  const char *col, PyBase &np, PyBase &mpl){
//  if(c >= brk - 1) fprintf(stdout, "%06d, %20.17lf\n", c, s); fflush(stdout);
  if(c >= brk){
    PyBase xyz = m & ((np|"array")(MKTPL(PYTPL(
      PYTPL(PYDBL(0), PYDBL(1)),
      PYTPL(PYDBL(0), PYDBL(0)),
      PYTPL(PYDBL(1), PYDBL(1)))), {}));
//    PYREPR(stdout, xyz);
    PyBase x = xyz[0];
    PyBase y = xyz[1];
    PyBase z = xyz[2];
    (ax|"plot")(MKTPL(x, y, PYSTR(col)), {});
//    (mpl|"pause")(MKTPL(PYDBL(0.005)), {}); // ** will be slowly ***
    return;
  }
  fractal(f1 & m, s * r1, c + 1, ax, brk, r1, r2, f1, f2, col, np, mpl);
  fractal(f2 & m, s * r2, c + 1, ax, brk, r1, r2, f1, f2, col, np, mpl);
}

void fractal_hata(void){
  PyMod np("numpy");
  PyMod mpl("matplotlib.pyplot");

  PyBase fig = (mpl|"figure")();

  PyBase ax = (fig|"add_subplot")(MKTPL(PYLNG(231)), {});
  (ax|"set_xlim")(MKTPL(PYTPL(PYDBL(-0.2), PYDBL(1.2))), {});
  (ax|"set_ylim")(MKTPL(PYTPL(PYDBL(-0.3), PYDBL(1.1))), {});
  (ax|"set_xlabel")(MKTPL(PYSTR("X")), {});
  (ax|"set_ylabel")(MKTPL(PYSTR("X")), {});
  (ax|"set_title")(MKTPL(PYSTR("2D_Hata")), {});
  (ax|"set_aspect")(MKTPL(PYSTR("equal")), {});

  double pi = np|"pi";
  double r1 = 1. / (double)(np|"sqrt")(MKTPL(PYDBL(3.)), {});
  double r2 = 2. / 3.;
  PyBase f1 = ROT(pi / 8.) & SC1(r1) & SC2(1., -1.);;
  PyBase f2 = SFT(1., 0.) & SC1(r2) & SC2(1., -1.) & SFT(-1., 0.);
  PyBase id = (np|"identity")(MKTPL(PYLNG(3)), {});
  fractal(id, 1., 0, ax, 10, r1, r2, f1, f2, "g-", np, mpl);
/*
  double wpi = pi + .2;
  PyBase x = (np|"arange")(MKTPL(PYDBL(-wpi), PYDBL(wpi), PYDBL(.1)), {});
  PyBase y = (np|"arange")(MKTPL(PYDBL(-wpi), PYDBL(wpi), PYDBL(.1)), {});
  (ax|"plot")(MKTPL(x, y, PYSTR("g.")), {});
//  (mpl|"pause")(MKTPL(PYDBL(0.005)), {});
*/

  (fig|"add_subplot")(MKTPL(PYLNG(236)), {});

  (mpl|"show")();
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
