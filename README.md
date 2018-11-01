pyob
====

Python PyObject wrapper for C++ matplotlib numpy wx etc

(web) [ https://nomissbowling.github.io/pyob ]( https://nomissbowling.github.io/pyob )

(repository) [ https://github.com/nomissbowling/pyob ]( https://github.com/nomissbowling/pyob )


ScreenShot
----------

[pyob_fractal_hata_00]: https://github.com/nomissbowling/pyob/blob/master/docs/img/pyob_fractal_hata_00.png?raw=true
![screenshot][pyob_fractal_hata_00]
 * https://github.com/nomissbowling/pyob/blob/master/docs/img/pyob_fractal_hata_00.png?raw=true


Requirements
------------

matplotlibcpp.h [ https://github.com/lava/matplotlib-cpp ]( https://github.com/lava/matplotlib-cpp )

tested matplotlib-cpp version (hash 00a4766) 2018-10-06

tested with Python 3.6.1 (64-bit)

tested with Visual C++ 2017


Environments
------------

set QT_QPA_PLATFORM_PLUGIN_PATH=%PYTHONHOME%/Library/plugins/platforms


Sample
------

``` c++
#define _USE_MATH_DEFINES // M_PI = 3.14159265358979323846...
#include <cmath>
#include <vector>
#include <iomanip>
#include <iostream>
#include <stdio.h>

#include "pyob.h"
// #include "matplotlibcpp.h"
#pragma comment(lib, "python36.lib")

using namespace std;
using namespace pyob;
namespace plt = matplotlibcpp;

int main(int argc, char **argv)
{
  PyBase::version();

  int n = 500;
  vector<double> x(n), y(n);
  for(int i = 0; i < n; ++i){
    x[i] = i;
    y[i] = sin(i * M_PI / 180.0);
  }
  plt::plot(x, y, "--r");

  PyBase::begin(L"dummy");

try{
  PyLng a(3);
  PyLng b(5);
  PyLng c(7);
  PyLst lst(tie(a, b, c));
  PyTpl tpl(tie(a, b, c));
  PyDct dct({ {"X", PYLNG(123)}, {"Y", PYDBL(4.56)}, {"Z", PYSTR("789")} });

  lst[1] = PYLNG(-10);
  try{
    lst[9] = PYLNG(-99); // SKIP: exception - invalid index
  }catch(const std::exception &e){
    fprintf(stderr, "SKIP: exception[%s]\n", e.what());
  }
PYREPR(stdout, lst);

  try{
    tpl[1] = PYLNG(-99); // SKIP: exception - not allowed to change tuple item
  }catch(const std::exception &e){
    fprintf(stderr, "SKIP: exception[%s]\n", e.what());
  }
PYREPR(stdout, tpl);

  dct["Y"] = PYDBL(-99);
PYREPR(stdout, dct);

PYREPR(stdout, b); // kept

  PyTpl keytpl(tie(PYLNG(3), PYLNG(4))); // tuple as an instance
  PyTpl tmptpl(tie(PYLNG(5), PYLNG(6)));
/*
  PyDct dcttpl;
  dcttpl[PYTPL(PYLNG(1), PYLNG(2))] = PYLNG(11);
*/
  PyDct dcttpl{ { {PYTPL(PYLNG(1), PYLNG(2)), PYLNG(22)} } };
  dcttpl[PYTPL(PYLNG(3), PYLNG(4))] = PYLNG(44); // tuple as another instance
PYREPR(stdout, dcttpl);
PYREPR(stdout, dcttpl[keytpl]);
  dcttpl[keytpl] = PYDBL(33);
  try{
    PyBase t = dcttpl[tmptpl]; // SKIP: exception - reference to NULL
  }catch(const std::exception &e){
    fprintf(stderr, "SKIP: exception[%s]\n", e.what());
  }
  dcttpl[tmptpl] = PYLNG(55);
PYREPR(stdout, dcttpl[keytpl]);
PYREPR(stdout, dcttpl);

  PyDct dctstr({ {"a", PYLNG(10)}, {"b", PYLNG(20)}, {"c", PYLNG(30)} });
  dctstr[PYSTR("b")] = PYDBL(99.99);
PYREPR(stdout, dctstr);

  PyDct dctobj{ { {PYSTR("x"), PYLNG(11)}, {PYSTR("y"), PYLNG(22)}, {PYSTR("z"), PYLNG(33)} } };
  dctobj["y"] = PYDBL(22.22);
PYREPR(stdout, dctobj);
}catch(const std::exception &e){
  fprintf(stderr, "exception[%s]\n", e.what());
}

try{
  PyMod np("numpy");
  PyBase a = (np|"array")(MKTPL(PYTPL(PYTPL(PYLNG(1), PYLNG(2)), PYTPL(PYLNG(3), PYLNG(4)))), {});
PYREPR(stdout, a);
  PyBase b = (np|"identity")(MKTPL(PYLNG(2)), {});
PYREPR(stdout, b);
  PyBase c = a * b;
PYREPR(stdout, c);
  PyBase d = a & b;
PYREPR(stdout, d);
PYREPR(stdout, a & a);
}catch(const std::exception &e){
  fprintf(stderr, "exception[%s]\n", e.what());
}

try{
  PyMod np("numpy");
  fprintf(stdout, "%20.17lf\n", double(np|"pi")); // 3.14159265358979323846...
  fprintf(stdout, "%20.17lf\n", double(np|"e")); // 2.71828182845904523536...

  PyBase x = (np|"arange")(tie(PYLNG(-99), PYLNG(99), PYDBL(.2)), {});
  PyBase y = (np|"arange")(tie(PYLNG(-30), PYLNG(30), PYDBL(.1)), {});
  PyBase m = (np|"meshgrid")(tie(x, y), {});
  PyBase zz = (np|"cos")(MKTPL(m[0]), {}) * (np|"sin")(MKTPL(m[1]), {});
//  PyBase zz = (np|"sqrt")(MKTPL((m[0] ^ 2) + (m[1] ^ 2)), {});

  PyMod pyplot("matplotlib.pyplot");
  PyBase ax = (pyplot|"contour")(MKTPL(m[0] * 3.5, m[1] / 5.0, zz), {});
}catch(const std::exception &e){
  fprintf(stderr, "exception[%s]\n", e.what());
}

  plt::show();

try{
  PyMod _builtins("builtins");
  PyBase _h = (_builtins|"bytes"|"fromhex")(tie(PYSTR("55AA")), {});
  PyBase::s((wchar_t *)_h);

  PyMod wx("wx");
  PyBase app = (wx|"App")();
  PyBase frm = (wx|"Frame")(tie(PYNONE, PYLNG(-1)), {
      {"title", PYSTR("TEST wx")},
      {"size", PYBV("ii", 640, 480)}, {"pos", PYBV("ii", 320, 240)} });
  const char *ascicon =
    "789cc592310e8330100497e006450afe41a8229e41e187f184b4f9457a1a3f27\x0A"
    "657a8acbad6d029130a241597b59e6245b823ba0c0094d63350d5e27e006c0da\x0A"
    "c86d015c4aa0d55aa3eed4ac6fab0f1b7d1f834b5fbcf79aac48d81089c1c512\x0A"
    "1f07ab7a50f73c4be2f3403df32cbb395cbc60e35439d6d346f2ccab220fdbbc\x0A"
    "f8e6c0aedbcf6e9b8f94ac08e5648f55e7eefaba9455af88736e1167fd1f735e\x0A"
    "a571a999dacf902313212b9d966e649f6bd70d29e389d473f3d3fbecbf91e882\x0A"
    "ee672ff5a6afb3a733f407b4ecff08\x0A";
  PyStr ascdat(ascicon);
  PyBase hex = (ascdat|"replace")(tie(PYSTR("\n"), PYSTR("")), {});
  PyMod binascii("binascii");
  PyBase binicon = (binascii|"a2b_hex")(tie(hex), {});
  PyMod zlib("zlib");
  PyBase bindat = (zlib|"decompress")(tie(binicon), {});
  PyMod pyio("io");
  PyBase strm = (pyio|"BytesIO")(tie(bindat), {});
  PyBase im = (wx|"Image")(tie(strm), {});
  PyBase bm = (wx|"Bitmap")(tie(im), {});
  PyBase icon = (wx|"Icon")();
  (icon|"CopyFromBitmap")(tie(bm), {});
  (frm|"SetIcon")(tie(icon), {});
  (frm|"Show")();
  (app|"SetTopWindow")(tie(frm), {});
  (app|"MainLoop")();

  PyMod sys("sys");
  (sys|"path"|"append")(tie(PYSTR(".")), {});

  PyMod mm("mm"); // mm.py (test module) must be at '.'
  PyBase mc = (mm|"mc")(tie(PYLNG(1234)), {});
  PyBase r = (mc|"pi")(tie(mm, mc), { {"a", mm}, {"b", mc} });
}catch(const std::exception &e){
  fprintf(stderr, "exception[%s]\n", e.what());
}

  PyBase::end();
  return 0;
}
```


mm.py (test module)

``` python
class mc(object):
  '''test data overwritten by self.n after construction'''
  n = 9999

  def __init__(self, n):
    self.n = n
    print('TEST mc')

  def pi(self, *args, **kwargs):
    print(f'TEST mc.pi ({len(args)}), {kwargs.keys()}, self.n={self.n}')
    # print(f'{args[0].__dict__}') # OK
    # print(f'{kwargs["a"].__dict__}') # OK
    for i, v in enumerate(args): print(f'{i}: [{v}]')
    for k in kwargs.keys(): print(f'[{k}]: [{kwargs[k]}]')
    return {'result': 'XYZ', 'args': args, 'kwargs': kwargs}
```


License
-------

MIT license
