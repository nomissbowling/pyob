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

typedef struct _AREA {
  int l, t, r, b;
} AREA;

AREA DISPLAY_MAX = {0, 0, 640, 480}; // (640x480) x N
// AREA DISPLAY_MAX = {0, 0, 2560, 1920}; // (640x480) x N

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
  PyBase &ax, int brk, double r1, double r2, PyBase f1, PyBase f2, int lr,
  PyBase &np, PyBase &mpl){
//  if(c >= brk - 1) fprintf(stdout, "%06d, %20.17lf\n", c, s); fflush(stdout);
  if(c >= brk){
    PyBase xyz = m & ((np|"array")(MKTPL(PYTPL(
      PYTPL(PYDBL(0), PYDBL(1)),
      PYTPL(PYDBL(0), PYDBL(0)),
      PYTPL(PYDBL(1), PYDBL(1)))), {}));
//    PYREPR(stdout, xyz);
    PyBase x = xyz[0];
    PyBase y = xyz[1];
    PyBase z = y; // xyz[2]; // copy from y
    (ax|"plot")(MKTPL(x, y, z,
      PYSTR(lr < 0 ? "b-" : lr > 0 ? "g-" : "r-")), {});
//    (mpl|"pause")(MKTPL(PYDBL(0.005)), {}); // ** will be slowly ***
    return;
  }
  fractal(f1 & m, s * r1, c + 1, ax, brk, r1, r2, f1, f2, 1 + lr, np, mpl);
  fractal(f2 & m, s * r2, c + 1, ax, brk, r1, r2, f1, f2, 1 - lr, np, mpl);
}

void scale(int n, const char *title, int w, int h, AREA a,
  PyBase &ax, PyBase &np, PyBase &mpl){
  PyMod image("PIL.Image"); // import PIL.Image (same as below)
//  PyBase image = PyMod("PIL", "Image")|"Image"; // from PIL import Image
  PyLst imglst;
  for(int i = 0; i < n; ++i){
    double j = pow(10., - i / 50.);
    (ax|"set_xlim")(MKTPL(PYTPL(PYDBL(-0.2 * j), PYDBL(1.2 * j))));
    (ax|"set_ylim")(MKTPL(PYTPL(PYDBL(-0.3 * j), PYDBL(1.1 * j))));
    (mpl|"pause")(MKTPL(PYDBL(0.005)), {});

#if 1 // (.avi) .shape: (478, 640, 3)
    PyBase szi = (ax|"figure"|"get_size_inches")();
    PyBase dpi = (ax|"figure"|"get_dpi")();
    PyBase wh = szi * dpi;
    PyTpl iwh(MKTPL(PYLNG((double)wh[0]), PYLNG((double)wh[1])));
    PyBase im = (np|"fromstring")(
      MKTPL((ax|"figure"|"canvas"|"tostring_rgb")()),
      {{"dtype", PYSTR("uint8")}});
    PyBase ar = (im|"reshape")(MKTPL(iwh[1], iwh[0], PYLNG(3)), {});
#else // (.gif) .shape: (478, 640, 4)
    PyBase ar = (np|"array")(
      MKTPL(ax|"figure"|"canvas"|"renderer"|"_renderer"),
      {{"dtype", PYSTR("uint8")}});
#endif
//PYREPR(stdout, (ar|"shape"));
    PyBase img = (((image|"fromarray")(MKTPL(ar), {})
      |"crop")(
        MKTPL(PYTPL(PYLNG(a.l), PYLNG(a.t), PYLNG(a.r), PYLNG(a.b))), {})
      |"resize")(
        MKTPL(PYTPL(PYLNG(w), PYLNG(h)), image|"BICUBIC"), {});
#if 0
    char imfn[256]; // *** be careful ***
    sprintf_s(imfn, sizeof(imfn), "c:\\tmp\\_%s_%06d.png", title, i);
    (img|"save")(MKTPL(PYSTR(imfn)), {});
#endif
    (imglst|"append")(MKTPL(img), {}); // too many copy
  }
#if 1
  char ofn[256]; // *** be careful ***
  sprintf_s(ofn, sizeof(ofn), "c:\\tmp\\_anim_%s.gif", title); // .avi
  (imglst[0]|"save")(MKTPL(PYSTR(ofn)), {
    {"save_all", PYLNG(1)}, // Py_True
    {"append_images", imglst}, // duplicate imglst[0]
    {"optimize", PYLNG(0)}, // Py_False
    {"duration", PYLNG(120)},
    {"loop", PYLNG(0)}}); // 0: infinit loop
#endif
}

void fractal_hata(int pos, const char *title,
  PyBase &np, PyBase &mpl, PyBase &fig){
  PyBase ax = (fig|"add_subplot")(MKTPL(PYLNG(pos)),
    {{"projection", PYSTR("3d")}});
  (ax|"set_xlim")(MKTPL(PYTPL(PYDBL(-0.2), PYDBL(1.2))), {});
  (ax|"set_ylim")(MKTPL(PYTPL(PYDBL(-0.3), PYDBL(1.1))), {});
  (ax|"set_zlim")(MKTPL(PYTPL(PYDBL(-0.3), PYDBL(1.1))), {});
  (ax|"set_xlabel")(MKTPL(PYSTR("X")), {});
  (ax|"set_ylabel")(MKTPL(PYSTR("Y")), {});
  (ax|"set_zlabel")(MKTPL(PYSTR("Z")), {});
  (ax|"set_title")(MKTPL(PYSTR(title)), {});
  (ax|"set_aspect")(MKTPL(PYSTR("equal")), {});

  double pi = np|"pi";
  double a = pi / 8.;
  double r1 = 1. / (double)(np|"sqrt")(MKTPL(PYDBL(3.)), {});
  double r2 = 2. / 3.;
  PyBase f1 = ROT(a) & SC1(r1) & SC2(1., -1.);
  PyBase f2 = SFT(1., 0.) & SC1(r2) & SC2(1., -1.) & SFT(-1., 0.);
  PyBase id = (np|"identity")(MKTPL(PYLNG(3)), {});
  int brk = 10; // recommend odd value (setting huge value caused to be late)
  fractal(id, 1., 0, ax, brk, r1, r2, f1, f2, 0, np, mpl);
  scale(100, title, 640, 480, DISPLAY_MAX, ax, np, mpl);
}

void fractal_koch(int pos, const char *title,
  PyBase &np, PyBase &mpl, PyBase &fig){
  PyBase ax = (fig|"add_subplot")(MKTPL(PYLNG(pos)),
    {{"projection", PYSTR("3d")}});
  (ax|"set_xlim")(MKTPL(PYTPL(PYDBL(-0.2), PYDBL(1.2))), {});
  (ax|"set_ylim")(MKTPL(PYTPL(PYDBL(-0.3), PYDBL(1.1))), {});
  (ax|"set_zlim")(MKTPL(PYTPL(PYDBL(-0.3), PYDBL(1.1))), {});
  (ax|"set_xlabel")(MKTPL(PYSTR("X")), {});
  (ax|"set_ylabel")(MKTPL(PYSTR("Y")), {});
  (ax|"set_zlabel")(MKTPL(PYSTR("Z")), {});
  (ax|"set_title")(MKTPL(PYSTR(title)), {});
  (ax|"set_aspect")(MKTPL(PYSTR("equal")), {});

  double pi = np|"pi";
  double a = pi / 6.;
  double r1 = 1. / (double)(np|"sqrt")(MKTPL(PYDBL(3.)), {});
  double r2 = 1. / (double)(np|"sqrt")(MKTPL(PYDBL(3.)), {});
  PyBase f1 = SC1(r1) & ROT(a) & SC2(1., -1.);
  PyBase f2 = SFT(1., 0.) & SC1(r2) & SC2(1., -1.) & ROT(a) & SFT(-1., 0.);
  PyBase id = (np|"identity")(MKTPL(PYLNG(3)), {});
  int brk = 10; // recommend odd value (setting huge value caused to be late)
  fractal(id, 1., 0, ax, brk, r1, r2, f1, f2, 0, np, mpl);
  scale(100, title, 640, 480, DISPLAY_MAX, ax, np, mpl);
}

void fractal_mesh(int pos, const char *title,
  PyBase &np, PyBase &mpl, PyBase &fig){
  PyBase ax = (fig|"add_subplot")(MKTPL(PYLNG(pos)),
    {{"projection", PYSTR("3d")}});
  (ax|"set_xlim")(MKTPL(PYTPL(PYDBL(-9.5), PYDBL(9.5))), {});
  (ax|"set_ylim")(MKTPL(PYTPL(PYDBL(-9.5), PYDBL(9.5))), {});
  (ax|"set_zlim")(MKTPL(PYTPL(PYDBL(-3.5), PYDBL(3.5))), {});
  (ax|"set_xlabel")(MKTPL(PYSTR("X")), {});
  (ax|"set_ylabel")(MKTPL(PYSTR("Y")), {});
  (ax|"set_zlabel")(MKTPL(PYSTR("Z")), {});
  (ax|"set_title")(MKTPL(PYSTR(title)), {});
  (ax|"set_aspect")(MKTPL(PYSTR("equal")), {});

  double pi = np|"pi";
  double wpi = pi * 3.;
  PyBase t = (np|"arange")(MKTPL(PYDBL(-wpi), PYDBL(wpi), PYDBL(.1)), {});
  PyBase m = (np|"meshgrid")(MKTPL(t, t), {});
  PyBase xx = m[0];
  PyBase yy = m[1];
  PyBase zz = (np|"cos")(MKTPL(xx), {}) * (np|"sin")(MKTPL(yy), {});
  (ax|"plot_wireframe")(MKTPL(xx, yy, zz), {{"color", PYSTR("b")}});
  (mpl|"pause")(MKTPL(PYDBL(0.005)), {});
}

void fractal_3d(int pos, const char *title,
  PyBase &np, PyBase &mpl, PyBase &fig){
  PyBase ax = (fig|"add_subplot")(MKTPL(PYLNG(pos)),
    {{"projection", PYSTR("3d")}});
  (ax|"set_xlim")(MKTPL(PYTPL(PYDBL(-1.5), PYDBL(1.5))), {});
  (ax|"set_ylim")(MKTPL(PYTPL(PYDBL(-1.5), PYDBL(1.5))), {});
  (ax|"set_zlim")(MKTPL(PYTPL(PYDBL(-1.5), PYDBL(1.5))), {});
  (ax|"set_xlabel")(MKTPL(PYSTR("X")), {});
  (ax|"set_ylabel")(MKTPL(PYSTR("Y")), {});
  (ax|"set_zlabel")(MKTPL(PYSTR("Z")), {});
  (ax|"set_title")(MKTPL(PYSTR(title)), {});
  (ax|"set_aspect")(MKTPL(PYSTR("equal")), {});

  double pi = np|"pi";
  double wpi = pi + .2;
  PyBase t = (np|"arange")(MKTPL(PYDBL(-wpi), PYDBL(wpi), PYDBL(.1)), {});
  PyBase x = (np|"cos")(MKTPL(t), {});
  PyBase y = (np|"sin")(MKTPL(t), {});
  PyBase z = y;
  (ax|"plot")(MKTPL(x, y, z, PYSTR("ro")), {});
  (mpl|"pause")(MKTPL(PYDBL(0.005)), {});
}

void fractal_2d(int pos, const char *title,
  PyBase &np, PyBase &mpl, PyBase &fig){
  PyBase ax = (fig|"add_subplot")(MKTPL(PYLNG(pos)), {});
  (ax|"set_xlim")(MKTPL(PYTPL(PYDBL(-1.5), PYDBL(1.5))), {});
  (ax|"set_ylim")(MKTPL(PYTPL(PYDBL(-1.5), PYDBL(1.5))), {});
  (ax|"set_xlabel")(MKTPL(PYSTR("X")), {});
  (ax|"set_ylabel")(MKTPL(PYSTR("Y")), {});
  (ax|"set_title")(MKTPL(PYSTR(title)), {});
  (ax|"set_aspect")(MKTPL(PYSTR("equal")), {});

  double pi = np|"pi";
  double wpi = pi + .2;
  PyBase t = (np|"arange")(MKTPL(PYDBL(-wpi), PYDBL(wpi), PYDBL(.1)), {});
  PyBase x = (np|"cos")(MKTPL(t), {});
  PyBase y = (np|"sin")(MKTPL(t), {});
  (ax|"plot")(MKTPL(x, y, PYSTR("g-")), {});
  (mpl|"pause")(MKTPL(PYDBL(0.005)), {});
  scale(1, title, 640, 480, DISPLAY_MAX, ax, np, mpl);
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

  PyMod np("numpy");
  PyMod mpl("matplotlib.pyplot");
  PyMod p3d("mpl_toolkits.mplot3d", "Axes3D");
  PyBase fig = (mpl|"figure")();
  fractal_2d(231, "2D_CIRCLE", np, mpl, fig);
  fractal_3d(232, "3D_CIRCLE", np, mpl, fig);
  fractal_mesh(233, "3D_MESH", np, mpl, fig);
  fractal_koch(234, "2D_Koch", np, mpl, fig);
  fractal_hata(236, "2D_Hata", np, mpl, fig);
  (mpl|"show")();
//  PyBase::end();
}catch(const std::exception &e){
  fprintf(stderr, "excaption[%s]\n", e.what());
}
}
