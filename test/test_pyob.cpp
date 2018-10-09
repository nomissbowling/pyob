/*
  test_pyob.cpp
*/

//#ifdef _CONSOLE || _WINDOWS
#include "stdafx.h"
//#endif

#include "pyob.h"

void test_ide(void){
try{
/*
  char *fn = "_test_.py"; // UTF-8
//PyRun_SimpleFile(); // not for Windows (may be caused FileSystemEncoding ?)
  PyObject* py_fo = PyFile_FromString(fn, "r");
  PyRun_SimpleFile(PyFile_AsFile(py_fo), fn);
*/
/*
  PyRun_SimpleString("TEST_GLOBAL = b'\\xe9\\xa1\\xb1'.decode('utf-8')");
  const wchar_t *argv[] = {L"fake_main.exe", L"_test_executed_.py"}; // , L""
  Py_Main(sizeof(argv) / sizeof(argv[0]) - 1, (wchar_t **)argv); // IDE after W
*/
}catch(const std::exception &e){
  fprintf(stderr, "exception[%s]\n", e.what());
}
}

void test_base(void){
try{
  pyob::PyObj o;
  pyob::PyObj p = o; // copy
  pyob::PyObj q = pyob::PyObj(); // not copy
  pyob::PyObj &r = o;
  pyob::PyTpl t = pyob::PyTpl();
}catch(const std::exception &e){
  fprintf(stderr, "exception[%s]\n", e.what());
}
}

void test_item(void){
try{
  pyob::PyLng a(3);
  pyob::PyLng b(5);
  pyob::PyLng c(7);
  pyob::PyLst lst = pyob::PyLst(std::tie(a, b, c), {});
  pyob::PyTpl tpl = pyob::PyTpl(std::tie(a, b, c), {});

PyObject_Print(PyObject_Repr(lst.o()), stdout, 0); fprintf(stdout, "\n");
PyObject_Print(PyObject_Repr(lst[1].o()), stdout, 0); fprintf(stdout, "\n");
//  lst[1] = PYLNG(-10); // not change
PyObject_SetItem(lst.o(), PYLNG(1).o(), PYLNG(-10).o());
PyObject_Print(PyObject_Repr(lst[1].o()), stdout, 0); fprintf(stdout, "\n");
PyObject_Print(PyObject_Repr(lst.o()), stdout, 0); fprintf(stdout, "\n");

PyObject_Print(PyObject_Repr(tpl.o()), stdout, 0); fprintf(stdout, "\n");
PyObject_Print(PyObject_Repr(tpl[1].o()), stdout, 0); fprintf(stdout, "\n");
//  tpl[1] = PYLNG(-99); // not change
PyObject_SetItem(tpl.o(), PYLNG(1).o(), PYLNG(-99).o()); // through not allowed
PyObject_Print(PyObject_Repr(tpl[1].o()), stdout, 0); fprintf(stdout, "\n");
PyObject_Print(PyObject_Repr(tpl.o()), stdout, 0); fprintf(stdout, "\n");
}catch(const std::exception &e){
  fprintf(stderr, "exception[%s]\n", e.what());
}
}

void test_pyplot(const char *title){
try{
  pyob::PyMod np("numpy");
  fprintf(stdout, "%20.17lf\n", double(np|"pi")); // 3.14159265358979323846...
  fprintf(stdout, "%20.17lf\n", double(np|"e")); // 2.71828182845904523536...
  pyob::PyBase x = (np|"arange")(std::tie(PYLNG(-99), PYLNG(99), PYDBL(.2)), {});
  pyob::PyBase y = (np|"arange")(std::tie(PYLNG(-30), PYLNG(30), PYDBL(.1)), {});
  pyob::PyBase m = (np|"meshgrid")(std::tie(x, y), {});
  pyob::PyBase zz = (np|"sqrt")(MKTPL((m[0] ^ 2) + (m[1] ^ 2)), {});
//PyObject_Print(PyObject_Repr(zz.o()), stdout, 0); fprintf(stdout, "\n");

  pyob::PyMod matplotlib("matplotlib");
  pyob::PyMod pyplot("matplotlib.pyplot");
  pyob::PyMod pylab("pylab");

  pyob::PyBase ax = (pyplot|"contour")(MKTPL(m[0] * 3.5, m[1] / 5.0, zz), {});

if(0){ // (char **) selffree=false
  pyob::PyBase res = pyplot.c("title", 1, false, "(s)", title);
}

if(0){ // selffree=false
  PyObject *args = PyTuple_New(1);
  PyTuple_SetItem(args, 0, pyob::PyStr(title, false).o());
  PyObject *kwargs = PyDict_New();
  PyObject *res = pyplot.call("title", args, kwargs);
  Py_DECREF(args);
  Py_DECREF(kwargs);
  if(res) Py_DECREF(res);
}

if(0){ // selffree=false
  pyob::PyTpl args(1);
  args.setitem(0, (pyob::PyBase &)pyob::PyStr(title, false));
  pyob::PyDct kwargs;
  pyob::PyBase res = pyplot.c("title", args, kwargs);
}

if(0){ // (char **)
  pyob::PyTpl &args = (pyob::PyTpl &)pyob::PyBase(true, "(s)", title);
  pyob::PyDct &kwargs = (pyob::PyDct &)pyob::PyBase(true, "{}");
  pyob::PyBase res = pyplot.c("title", args, kwargs);
}

if(0){ // (char **)
  pyob::PyTpl &args = (pyob::PyTpl &)pyob::PyBase(true, "(s)", title);
  pyob::PyBase res = pyplot.c("title", args);
}

if(0){ // (char **)
  pyob::PyBase res = pyplot.c("title",
    (pyob::PyTpl &)pyob::PyBase(true, "(s)", title));
}

if(0){ // selffree=false
  pyob::PyStr t(title, false);
  pyob::PyBase res = pyplot.c("title", std::tie(t), {});
}

if(0){ // selffree=false
  pyob::PyStr t(title);
  pyob::PyBase res = pyplot.c("title", std::tie(t), {});
}

if(0){ // selffree=false
  pyob::PyBase t = PYSTR(title, false);
  pyob::PyBase res = pyplot.c("title", std::tie(t), {});
}

if(0){ // selffree=false
  pyob::PyBase &t = PYSTR(title, false);
  pyob::PyBase res = pyplot.c("title", std::tie(t), {});
}

if(0){ // OK
  pyob::PyBase res = pyplot.c("title", std::tie(PYSTR(title)), {});
}

if(1){ // OK
  pyob::PyBase res = (pyplot|"title")(std::tie(PYSTR(title)), {});
}
}catch(const std::exception &e){
  fprintf(stderr, "exception[%s]\n", e.what());
}
}

void test_wx(const char *title){
try{
  pyob::PyMod wx("wx");
  pyob::PyBase app = (wx|"App")();
/*
  pyob::PyBase frm = wx.c("Frame",
    (pyob::PyTpl &)pyob::PyBase(true, "si", NULL, -1),
    (pyob::PyDct &)pyob::PyBase(true, "{sssOsO}",
      "title", title,
      "size", pyob::PyBase(true, "ii", 640, 480).o(),
      "pos", pyob::PyBase(true, "ii", 320, 240).o()));
*/
/*
  pyob::PyBase frm = wx.c("Frame",
    (pyob::PyDct &)pyob::PyBase(true, "{sssisssOsO}",
      "parent", NULL,
      "id", -1,
      "title", title,
      "size", pyob::PyBase(true, "ii", 640, 480).o(),
      "pos", pyob::PyBase(true, "ii", 320, 240).o()));
*/
/*
  pyob::PyBase frm = wx.c("Frame", 1, true, "si", NULL, -1);
*/
/*
  pyob::PyBase frm = wx.c("Frame", 1, true, "Oi",
    pyob::PyBase(true, "").o(), -1);
*/
/*
  pyob::PyBase frm = wx.c("Frame", 2, true, "{sssisssOsO}",
      "parent", NULL,
      "id", -1,
      "title", title,
      "size", pyob::PyBase(true, "ii", 640, 480).o(),
      "pos", pyob::PyBase(true, "ii", 320, 240).o());
*/
  pyob::PyBase frm = (wx|"Frame")(std::tie(PYNONE, PYLNG(-1)), {
      {"title", PYSTR(title)},
      {"size", PYBV("ii", 640, 480)}, {"pos", PYBV("ii", 320, 240)}});

  pyob::PyMod _builtins("builtins");
fprintf(stderr, "_builtins: %08X\n", _builtins.o());

/*
  PyRun_SimpleString("AZ = 'zzz'");
  pyob::PyBase _g = (_builtins|"globals")();
pyob::PyBase::p("_g: ", _g.len()); // !!!! not reach (exception)
fwprintf(stderr, L"[%s]\n", _g.ascii());
  pyob::PyBase _z = ((_builtins|"globals")()|"get")(std::tie(PYSTR("AZ")), {});
pyob::PyBase::p("_z: ", _z.len()); // !!!! not reach (exception)
fwprintf(stderr, L"[%s]\n", _z.ascii());
*/

  pyob::PyMod _main("__main__");
  pyob::PyBase _globals = pyob::PyBase(PyModule_GetDict(_main.o()));
  pyob::PyBase _locals = pyob::PyDct();
  const char *TESTCODE = "open('c:/tmp/_dummy_text.txt', 'wb').write(b'ABCD')";
  pyob::PyBase _b = (_builtins|"eval")(std::tie(PYSTR(TESTCODE), _globals, _locals), {});
/* OK
  pyob::PyBase _co(Py_CompileString(TESTCODE, "dummy", Py_eval_input));
  pyob::PyBase _b(PyEval_EvalCode(_co.o(), _globals.o(), _locals.o()));
*/
// pyob::PyBase::p("_b: ", _b.len()); // not str
fwprintf(stderr, L"[%06d]\n", (long)_b); // 000004 OK

  pyob::PyBase _h = (_builtins|"bytes"|"fromhex")(std::tie(PYSTR("55AA")), {});
pyob::PyBase::p("_h: ", _h.len()); // 0x0002
fwprintf(stderr, L"[%s]\n", _h.ascii()); // OK
fwprintf(stderr, L"[%s]\n", (wchar_t *)_h); // OK
fprintf(stderr, "[%s]\n", (char *)_h); // 55 AA OK
  pyob::PyBase::s((wchar_t *)_h); // OK

  pyob::PyBase _u = (_builtins|"bytes"|"fromhex")(std::tie(PYSTR("e9a1b1")), {});
fwprintf(stderr, L"[%s]\n", _u.repr()); // OK
fwprintf(stderr, L"[%s]\n", _u.ascii()); // OK
fwprintf(stderr, L"[%s]\n", (wchar_t *)_u); // OK (force bytes as str)
fprintf(stderr, "[%s]\n", (char *)_u); // OK
  pyob::PyBase _w = (_u|"decode")();
fwprintf(stderr, L"[%s]\n", _w.repr()); // ['?'] OK
fwprintf(stderr, L"[%s]\n", _w.ascii()); // ['\u9871'] OK
fwprintf(stderr, L"[%s]\n", (wchar_t *)_w); // [?] L.E. 71 98 OK
//fprintf(stderr, "[%s]\n", (char *)_w); // BAD (force str as bytes) no '\0' ?

  const char *ascicon =
    "789cc592310e8330100497e006450afe41a8229e41e187f184b4f9457a1a3f27\x0A"
    "657a8acbad6d029130a241597b59e6245b823ba0c0094d63350d5e27e006c0da\x0A"
    "c86d015c4aa0d55aa3eed4ac6fab0f1b7d1f834b5fbcf79aac48d81089c1c512\x0A"
    "1f07ab7a50f73c4be2f3403df32cbb395cbc60e35439d6d346f2ccab220fdbbc\x0A"
    "f8e6c0aedbcf6e9b8f94ac08e5648f55e7eefaba9455af88736e1167fd1f735e\x0A"
    "a571a999dacf902313212b9d966e649f6bd70d29e389d473f3d3fbecbf91e882\x0A"
    "ee672ff5a6afb3a733f407b4ecff08\x0A";
  pyob::PyStr ascdat(ascicon);
pyob::PyBase::p("ascdat", ascdat.len()); // 0x019e + LF x 7
fwprintf(stderr, L"%s\n", ascdat.ascii()); fflush(stderr);

  pyob::PyBase hex = (ascdat|"replace")(std::tie(PYSTR("\n"), PYSTR("")), {});
pyob::PyBase::p("hex", hex.len());
fwprintf(stderr, L"%s\n", hex.ascii()); fflush(stderr);

  pyob::PyBase _binicon = (_builtins|"bytes"|"fromhex")(std::tie(hex), {});
pyob::PyBase::p("_binicon", _binicon.len());
fwprintf(stderr, L"[%s]\n", _binicon.ascii()); fflush(stderr);

  pyob::PyMod binascii("binascii");
  pyob::PyBase binicon = (binascii|"a2b_hex")(std::tie(hex), {});
pyob::PyBase::p("binicon", binicon.len());
fwprintf(stderr, L"[%s]\n", binicon.ascii()); fflush(stderr);

  pyob::PyMod zlib("zlib");
  pyob::PyBase bindat = (zlib|"decompress")(std::tie(binicon), {});
pyob::PyBase::p("bindat", bindat.len()); // 0x0436
  pyob::PyMod pyio("io");
  pyob::PyBase strm = (pyio|"BytesIO")(std::tie(bindat), {});
  pyob::PyBase im = (wx|"Image")(std::tie(strm), {});
  pyob::PyBase bm = (wx|"Bitmap")(std::tie(im), {});
  pyob::PyBase icon = (wx|"Icon")();
  (icon|"CopyFromBitmap")(std::tie(bm), {});
  (frm|"SetIcon")(std::tie(icon), {});

  pyob::PyBase r0 = (frm|"Show")();
/*
  pyob::PyBase r1 = app.c("SetTopWindow",
    (pyob::PyTpl &)pyob::PyBase(true, "O", frm.o()));
*/
/*
  pyob::PyBase r1 = app.c("SetTopWindow", 1, true, "O", frm.o());
*/
  pyob::PyBase r1 = (app|"SetTopWindow")(std::tie(frm), {});
  pyob::PyBase r2 = (app|"MainLoop")();
}catch(const std::exception &e){
  fprintf(stderr, "exception[%s]\n", e.what());
}
}

void test_mm(void){
try{
  pyob::PyMod sys("sys");
  pyob::PyBase a = (sys|"path"|"append")(std::tie(PYSTR(".")), {});

  pyob::PyMod mm("mm"); // mm.py must be at '.'
  pyob::PyBase mc = (mm|"mc")(std::tie(PYLNG(1234)), {});
  pyob::PyBase r = (mc|"pi")(std::tie(mm, mc), {{"a", mm}, {"b", mc}});
pyob::PyBase::p("r: ", r.len());
fwprintf(stderr, L"[%s]\n", r.ascii());
  pyob::PyBase o = (mc|"pi")(
    std::tie(PYSTR("a-z"), PYLNG(-1), PYLST(PYLNG(-3)), PYTPL(PYLNG(-5))),
    {{"a", PYSTR("A-Z")}, {"b", PYLNG(-2)}, {"F", PYDBL(1.5/15.0)},
      {"I", PYDCT({{"-", PYLNG(-9)}, {"+", PYLNG(9)}})},
      {"c", PYLST(PYLNG(-4), PYSTR("xy"))}, {"d", PYTPL(PYLNG(-6), PYNONE)}});
pyob::PyBase::p("o: ", o.len());
fwprintf(stderr, L"[%s]\n", o.ascii());
}catch(const std::exception &e){
  fprintf(stderr, "exception[%s]\n", e.what());
}
}
