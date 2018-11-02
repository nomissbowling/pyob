/*
  pyob.h
*/

#ifndef __PYOB_H__
#define __PYOB_H__

#include <array>
#include <vector>
#include <tuple>
#include <map>
#include <unordered_map>
#include <functional>
#include <utility>
#include <initializer_list>
#include <iomanip>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
// #include <Python.h>
#include "matplotlibcpp.h"

#define PYOB_TRACE_OFF
#define PYOB_VERSION (0.8)

#define MKTPL(...) (std::make_tuple(__VA_ARGS__))
#define PYNONE ((pyob::PyBase &)pyob::PyObj())
#define PYOBJ(X) ((pyob::PyBase &)(X))
#define PYDBL(X) ((pyob::PyBase &)pyob::PyDbl(X))
#define PYLNG(X) ((pyob::PyBase &)pyob::PyLng(X))
#define PYBIN(X) ((pyob::PyBase &)pyob::PyBin(X))
#define PYSTR(X) ((pyob::PyBase &)pyob::PyStr(X))
#define PYLST(...) ((pyob::PyBase &)pyob::PyLst(std::tie(__VA_ARGS__)))
#define PYTPL(...) ((pyob::PyBase &)pyob::PyTpl(std::tie(__VA_ARGS__)))
#define PYDCT(...) ((pyob::PyBase &)pyob::PyDct(__VA_ARGS__))
#define PYBV(F, ...) ((pyob::PyBase &)pyob::PyBase(true, (F), ## __VA_ARGS__))
#define PYREPR(S, X) do{ \
  PyObject_Print(PyObject_Repr((X).o()), (S), 0); \
  fprintf((S), "\n"); \
  fflush(S); \
}while(0)

namespace pyob {

class PyBase;
class PyItem;
class PyStr;
class PyBin;
class PyLng;
class PyDbl;
class PyObj;
class PyLst;
class PyTpl;
class PyDct;
class PyMod;
class PyFnc;

class PyBase {
  friend class PyItem;
public:
  static void begin(const wchar_t *p);
  static void end(void);
  static void version(void);
  static void q(const char *s, int n=-1);
  static void p(const char *s, int n=-1);
  static void s(const wchar_t *w, int n=-1);
protected:
  static PyObject *mop;
protected:
  PyObject *po;
  bool selffree;
public:
  PyBase(PyObject *s, bool sf=true) : po(s), selffree(sf) {
    p("po(*)", r()); if(po) Py_INCREF(po); }
  PyBase(bool sf, const char *f, ...) : po(NULL), selffree(sf) {
    p("build(*)", r());
    va_list vargs;
    va_start(vargs, f);
    po = Py_VaBuildValue(f, vargs);
    va_end(vargs);
    if(!po){ throw std::runtime_error("Error build value: "); }
    else Py_INCREF(po);
  }
  PyBase(bool sf=true) : po(NULL), selffree(sf) { q("PyBase()", r()); }
  PyBase(const PyBase &s) : po(s.po), selffree(s.selffree) {
    p("copied PyBase()", r());
    if(!po){ throw std::runtime_error("Error no value src PyBase(<NULL>): "); }
    else Py_INCREF(po); // *** be careful ***
  }
  PyBase &operator=(const PyBase &s){
    if(po) Py_DECREF(po); // *** be careful ***
    po = s.po, selffree = s.selffree; // : ()
    p("assign PyBase &()", r());
    if(!po){ throw std::runtime_error("Error no value = PyBase(<NULL>): "); }
    else Py_INCREF(po); // *** be careful ***
    return *this;
  }
  virtual ~PyBase(){ p("~PyBase()", r()); if(po && selffree) Py_DECREF(po); }
  Py_ssize_t r(){
#ifndef PYOB_TRACE_OFF
    if(!po){
//      throw std::runtime_error("ERROR Py_REFCNT(NULL) wrong construction");
      return -1; // not 0 (be careful to fake size_t)
    }else{
      Py_ssize_t c = Py_REFCNT(po);
      if(!c) q("******** Py_REFCNT==0 ********\a"); // not throw to continue
      return c;
    }
#else
    return -1;
#endif
  }
  PyObject *o(){ return po; }
  struct HashKey {
    std::size_t operator()(const PyBase &b) const { return (std::size_t)b.po; }
  };
  struct HashEqualTo { // called it after when HashKey(L) == HashKey(R)
    bool operator()(const PyBase &a, const PyBase &b) const {
      // return (PyBase &)a == (PyBase &)b; // PyUnicode_(Rich)Compare for str?
      return false; // now always false, otherwise crash
    }
  };
  const wchar_t *repr(){ return (const wchar_t *)PyUnicode_AS_DATA(PyObject_Repr(po)); }
  const wchar_t *ascii(){ return (const wchar_t *)PyUnicode_AS_DATA(PyObject_ASCII(po)); }
  const char *bytes(){ return PyBytes_AsString(PyObject_Bytes(po)); }
  const wchar_t *str(){ return (const wchar_t *)PyUnicode_AS_DATA(PyObject_Str(po)); }
  Py_ssize_t len(){ return PyObject_Length(po); } // list tuple dict.keys()
  operator char *(){ return PyBytes_AsString(PyObject_Bytes(po)); }
  operator wchar_t *(){ return (wchar_t *)PyUnicode_AS_DATA(PyObject_Str(po)); }
  operator bool(){ return PyBool_Check(po); }
  operator int(){ return (int)(long)*this; } // not use it *** be careful ***
  operator long(){ return PyLong_AsLong(po); }
  operator double(){ return PyFloat_AsDouble(po); }
  PyBase operator<(const PyBase &b){ PyBase op(PyBase::mop, false);
    return (op|"lt")(std::tie(*this, b), {}); }
  PyBase operator<=(const PyBase &b){ PyBase op(PyBase::mop, false);
    return (op|"le")(std::tie(*this, b), {}); }
  PyBase operator==(const PyBase &b){ PyBase op(PyBase::mop, false);
    return (op|"eq")(std::tie(*this, b), {}); }
  PyBase operator!=(const PyBase &b){ PyBase op(PyBase::mop, false);
    return (op|"ne")(std::tie(*this, b), {}); }
  PyBase operator>=(const PyBase &b){ PyBase op(PyBase::mop, false);
    return (op|"ge")(std::tie(*this, b), {}); }
  PyBase operator>(const PyBase &b){ PyBase op(PyBase::mop, false);
    return (op|"gt")(std::tie(*this, b), {}); }
  PyBase operator+(){ PyBase op(PyBase::mop, false);
    return (op|"pos")(std::tie(*this), {}); }
  PyBase operator-(){ PyBase op(PyBase::mop, false);
    return (op|"neg")(std::tie(*this), {}); }
  PyBase operator+(const PyBase &b){ PyBase op(PyBase::mop, false);
    return (op|"add")(std::tie(*this, b), {}); }
  PyBase operator-(const PyBase &b){ PyBase op(PyBase::mop, false);
    return (op|"sub")(std::tie(*this, b), {}); }
  PyBase operator&(const PyBase &b){ PyBase op(PyBase::mop, false);
    return (op|"matmul")(std::tie(*this, b), {}); } // '@' exception if scalar
  PyBase operator*(const PyBase &b){ PyBase op(PyBase::mop, false);
    return (op|"mul")(std::tie(*this, b), {}); }
  PyBase operator/(const PyBase &b){ PyBase op(PyBase::mop, false);
    return (op|"truediv")(std::tie(*this, b), {}); } // '/' floating
  PyBase operator%(const PyBase &b){ PyBase op(PyBase::mop, false);
    return (op|"floordiv")(std::tie(*this, b), {}); } // '//' not mod
  PyBase operator^(const PyBase &b){ PyBase op(PyBase::mop, false);
    return (op|"pow")(std::tie(*this, b), {}); } // '**' not xor
  PyBase operator+(int b){ return *this + (long)b; }
  PyBase operator-(int b){ return *this - (long)b; }
  PyBase operator*(int b){ return *this * (long)b; }
  PyBase operator/(int b){ return *this / (long)b; }
  PyBase operator%(int b){ return *this % (long)b; }
  PyBase operator^(int b){ return *this ^ (long)b; }
  PyBase operator+(long b); // LA
  PyBase operator-(long b); // LA
  PyBase operator*(long b); // LA
  PyBase operator/(long b); // LA
  PyBase operator%(long b); // LA
  PyBase operator^(long b); // LA
  PyBase operator+(double b); // LA
  PyBase operator-(double b); // LA
  PyBase operator*(double b); // LA
  PyBase operator/(double b); // LA
  PyBase operator%(double b); // LA cast to long
  PyBase operator^(double b); // LA
  PyItem operator[](int n); // LA (not PyItem &operator[])
  PyItem operator[](long n); // LA (not PyItem &operator[])
  PyItem operator[](const char *k); // LA (not PyItem &operator[])
  PyItem operator[](PyBase k); // LA (not PyItem &operator[](const PyBase &k))
  PyObject *attr(const char *s, bool askv=false){
    if(askv) return PyDict_GetItemString(po, s);
    else return PyObject_GetAttrString(po, s);
  }
  PyBase a(const char *s, bool askv=false){ return PyBase(attr(s, askv)); }
  PyBase operator|(const char *s){ return a(s, false); }
//  PyBase operator->*(const char *s){ return a(s, false); }
  PyObject *call(PyObject *fnc, PyObject *args, PyObject *kwargs){
    if(!fnc){ throw std::runtime_error("Error call (NULL): "); }
    PyObject *tp = PyObject_Call(fnc, args, kwargs);
    if(!tp){ throw std::runtime_error("Error result call (NULL): "); }
    return tp;
  }
  PyObject *call(const char *f, PyObject *args, PyObject *kwargs){
    return call(attr(f), args, kwargs);
  }
  PyBase c(const PyBase &fnc, const PyTpl &args, const PyDct &kwargs); // LA
  PyBase c(const char *f, const PyTpl &args, const PyDct &kwargs); // LA
  PyBase c(const char *f, const PyTpl &args); // LA
  PyBase c(const char *f, const PyDct &kwargs); // LA
  PyBase c(const char *f); // LA
  PyBase c(const char *f, int m, bool sf, const char *fmt, ...); // LA
template< typename ... Ts >
  PyBase c(const char *f, const std::tuple< Ts... > &t, const std::unordered_map<const char *, PyBase &> &d, bool askv=false); // LA
template< typename ... Ts >
  PyBase operator()(const std::tuple< Ts... > &t, const std::unordered_map<const char *, PyBase &> &d); // LA
  PyBase operator()(const PyTpl &args, const PyDct &kwargs); // LA
  PyBase operator()(const PyTpl &args); // LA
  PyBase operator()(const PyDct &kwargs); // LA
  PyBase operator()(); // LA
};

class PyItem : public PyBase {
protected:
  PyBase parent;
  PyBase kw;
public:
  PyItem(PyBase &t, PyBase &k, bool sf=true) : PyBase(sf), parent(t), kw(k) {
    p("PyItem(*)", r());
    po = PyObject_GetItem(t.o(), k.o());
#if 0 // skip whenever NULL to set value later
    if(!po){ throw std::runtime_error("Error no key ? (GetItem): "); }
    else Py_INCREF(po); // *** be careful ***
#endif
    if(po) Py_INCREF(po); // *** be careful ***
  }
  PyItem &operator=(const PyBase &s){
    if(po) Py_DECREF(po); // *** be careful ***
    po = s.po, selffree = s.selffree; // : ()
    p("assign PyItem &()", r());
    if(!po){ throw std::runtime_error("Error no value PyItem(<NULL>): "); }
    else Py_INCREF(po); // *** be careful ***
    int t = PyObject_SetItem(parent.o(), kw.o(), po);
    if(t){ throw std::runtime_error("Error tuple or no key ? (SetItem): "); }
    return *this;
  }
  virtual ~PyItem(){ q("~PyItem()"); }
};

class PyStr : public PyBase {
protected:
public:
  PyStr(const char *s, bool sf=true) : PyBase(sf) { p("PyStr()");
    po = PyUnicode_FromString(s); // from UTF-8 encoded char *s
    if(!po){ throw std::runtime_error("Error can not create strings"); }
    else Py_INCREF(po);
  }
  virtual ~PyStr(){ q("~PyStr()"); }
};

class PyBin : public PyBase {
protected:
public:
  PyBin(const char *s, Py_ssize_t l, bool sf=true) : PyBase(sf) { p("PyBin()");
    po = PyBytes_FromStringAndSize(s, l); // only allocation when s == NULL
    if(!po){ throw std::runtime_error("Error can not create buffer"); }
    else Py_INCREF(po);
  }
  virtual ~PyBin(){ q("~PyBin()"); }
};

class PyLng : public PyBase {
protected:
public:
  PyLng(long i, bool sf=true) : PyBase(sf) { p("PyLng()");
    po = PyLong_FromLong(i);
    if(!po){ throw std::runtime_error("Error long:"); }
    else Py_INCREF(po);
  }
  virtual ~PyLng(){ q("~PyLng()"); }
};

class PyDbl : public PyBase {
protected:
public:
  PyDbl(double d, bool sf=true) : PyBase(sf) { p("PyDbl()");
    po = PyFloat_FromDouble(d);
    if(!po){ throw std::runtime_error("Error double:"); }
    else Py_INCREF(po);
  }
  virtual ~PyDbl(){ q("~PyDbl()"); }
};

class PyObj : public PyBase {
protected:
public:
  PyObj(bool sf=true) : PyBase(sf) { p("PyObj()");
    po = Py_BuildValue("");
    if(!po){ throw std::runtime_error("Error object: "); }
    else Py_INCREF(po);
  }
  virtual ~PyObj(){ q("~PyObj()"); }
};

class PyLst : public PyBase {
protected:
public:
  template< typename ... Ts >
  PyLst(const std::tuple< Ts... > &t, bool sf=true) : PyBase(sf) {
    p("PyLst(tuple)");
    int cnt = std::tuple_size< std::tuple< Ts... > >::value;
    po = PyList_New(cnt);
    if(!po){ throw std::runtime_error("Error list: "); }
    else Py_INCREF(po); // *** be careful ***
    PyObject *tp = po; // to capture variable [...] in lambda must be at local
    int i = 0;
    std::_For_each_tuple_element(t, [&tp, &i](auto&& v){
      PyList_SetItem(tp, i++, ((PyBase &)v).o());
    });
  }
  PyLst(int n=0, bool sf=true) : PyBase(sf) { p("PyLst()");
    po = PyList_New(n);
    if(!po){ throw std::runtime_error("Error list: "); }
    else Py_INCREF(po); // *** be careful ***
  }
  virtual ~PyLst(){ q("~PyLst()"); }
  PyLst &setitem(Py_ssize_t n, PyBase &s){
    PyList_SetItem(o(), n, s.o());
    return *this;
  }
};

class PyTpl : public PyBase {
protected:
public:
  template< typename ... Ts >
  PyTpl(const std::tuple< Ts... > &t, bool sf=true) : PyBase(sf) {
    p("PyTpl(tuple)");
    int cnt = std::tuple_size< std::tuple< Ts... > >::value; // decltype(t)
//    fprintf(stderr, "%d\n", cnt);
    po = PyTuple_New(cnt);
    if(!po){ throw std::runtime_error("Error tuple: "); }
//    else Py_INCREF(po); // *** be careful *** (must call after all setitems)
    PyObject *tp = po; // to capture variable [...] in lambda must be at local
    int i = 0;
//    for(i = 0; i < cnt; ++i){ ...((PyBase &)std::get<i>(t)).o()...; } // BAD
/*
    std::product_type::for_each([&tp, &i](auto v){
      PyTuple_SetItem(tp, i++, ((PyBase &)v).o());
      Py_INCREF(((PyBase &)v).o()); // *** be careful ***
    }, t);
*/
    std::_For_each_tuple_element(t, [&tp, &i](auto&& v){
//      fprintf(stderr, "%08X\n", ((PyBase &)v).o());
      PyTuple_SetItem(tp, i++, ((PyBase &)v).o());
      Py_INCREF(((PyBase &)v).o()); // *** be careful ***
    });
    Py_INCREF(po); // *** be careful *** (must call after all setitems)
  }
  PyTpl(int n=0, bool sf=true) : PyBase(sf) { p("PyTpl()");
    po = PyTuple_New(n);
    if(!po){ throw std::runtime_error("Error tuple: "); }
//    else Py_INCREF(po); // *** be careful *** (must call after all setitems)
    if(!n) Py_INCREF(po); // *** be careful *** (must call after all setitems)
  }
  virtual ~PyTpl(){ q("~PyTpl()"); }
  PyTpl &setitem(Py_ssize_t n, PyBase &s){
    PyTuple_SetItem(o(), n, s.o());
    Py_INCREF(s.o()); // *** be careful ***
    return *this;
  }
};

class PyDct : public PyBase {
protected:
public:
  PyDct(const std::unordered_map<const char *, PyBase &> &d, bool sf=true) : PyBase(sf) {
    p("PyDct(dict{char *})");
    po = PyDict_New();
    if(!po){ throw std::runtime_error("Error dict{char *}: "); }
    else Py_INCREF(po);
    for(auto it = d.begin(); it != d.end(); ++it){
//      fprintf(stderr, "[%s]", it->first);
//      fprintf(stderr, "=[%08X]\n", it->second.o());
      PyDict_SetItemString(po, it->first, it->second.o());
    }
  }
  // nest{} (to determinate ambiguous unordered_map<> type) not const PyBase &
  PyDct(const std::initializer_list< const std::unordered_map<const PyBase, PyBase &, PyBase::HashKey, PyBase::HashEqualTo> > &dd, bool sf=true) : PyBase(sf) {
    p("PyDct(dict{{PyBase &}})");
    po = PyDict_New();
    if(!po){ throw std::runtime_error("Error dict{{PyBase &}}: "); }
    else Py_INCREF(po);
    for(auto itdd = dd.begin(); itdd != dd.end(); ++itdd){
      for(auto it = itdd->begin(); it != itdd->end(); ++it){
//        fprintf(stderr, "[%08X]", ((PyBase &)it->first).o());
//        fprintf(stderr, "=[%08X]\n", it->second.o());
        PyDict_SetItem(po, ((PyBase &)it->first).o(), it->second.o());
      }
    }
  }
  PyDct(bool sf=true) : PyBase(sf) { p("PyDct()");
    po = PyDict_New();
    if(!po){ throw std::runtime_error("Error dict: "); }
    else Py_INCREF(po);
  }
  virtual ~PyDct(){ q("~PyDct()"); }
  PyDct &setitem(PyBase &k, PyBase &s){
    PyDict_SetItem(o(), k.o(), s.o());
    return *this;
  }
  PyDct &setitemstring(const char *k, PyBase &s){
    PyDict_SetItemString(o(), k, s.o());
    return *this;
  }
};

class PyMod : public PyBase {
protected:
public:
  PyMod(const char *s, bool sf=true) : PyBase(sf) { p("PyMod()");
    po = PyImport_Import(PyStr(s).o());
    if(!po){ throw std::runtime_error("Error loading module: "); }
  }
  PyMod(const char *from, const char *s, bool sf=true) : PyBase(sf) {
    p("PyMod(from)");
    PyMod _main("__main__");
    PyBase _globals = PyBase(PyModule_GetDict(_main.o()));
// PyBase::p("_globals: ", (int)_globals.o());
    PyBase _locals = PyDct();
// PyBase::p("_locals: ", (int)_locals.o());
    PyMod _builtins("builtins");
    po = (_builtins|"__import__")(MKTPL(
      PYSTR(from), PYOBJ(_globals), PYOBJ(_locals), PYLST(PYSTR(s)), PYLNG(0)),
      {}).o();
    if(!po){ throw std::runtime_error("Error loading module (from): "); }
  }
  virtual ~PyMod(){ q("~PyMod()"); }
};

class PyFnc : public PyBase {
protected:
public:
  PyFnc(bool sf=true) : PyBase(sf) { p("PyFnc()"); }
  virtual ~PyFnc(){ q("~PyFnc()"); }
};

// resolv LA (look ahead)

inline
PyBase PyBase::operator+(long b){ return *this + PYLNG(b); }

inline
PyBase PyBase::operator-(long b){ return *this - PYLNG(b); }

inline
PyBase PyBase::operator*(long b){ return *this * PYLNG(b); }

inline
PyBase PyBase::operator/(long b){ return *this / PYLNG(b); } // '/' floating

inline
PyBase PyBase::operator%(long b){ return *this % PYLNG(b); } // '//' not mod

inline
PyBase PyBase::operator^(long b){ return *this ^ PYLNG(b); } // '**' not xor

inline
PyBase PyBase::operator+(double b){ return *this + PYDBL(b); }

inline
PyBase PyBase::operator-(double b){ return *this - PYDBL(b); }

inline
PyBase PyBase::operator*(double b){ return *this * PYDBL(b); }

inline
PyBase PyBase::operator/(double b){ return *this / PYDBL(b); } // '/' floating

inline
PyBase PyBase::operator%(double b){ return *this % (long)b; } // '//' not mod

inline
PyBase PyBase::operator^(double b){ return *this ^ PYDBL(b); } // '**' not xor

inline
PyItem PyBase::operator[](int n){ return (*this)[(long)n]; }

inline
PyItem PyBase::operator[](long n){ return PyItem(*this, PYLNG(n)); } // Lst/Tpl

inline
PyItem PyBase::operator[](const char *k){ return PyItem(*this, PYSTR(k)); }

inline
PyItem PyBase::operator[](PyBase k){ return PyItem(*this, k); }

inline
PyBase PyBase::c(const PyBase &fnc, const PyTpl &args, const PyDct &kwargs){
  return PyBase(call(fnc.po, args.po, kwargs.po));
}

inline
PyBase PyBase::c(const char *f, const PyTpl &args, const PyDct &kwargs){
  return c(a(f), args, kwargs);
}

inline
PyBase PyBase::c(const char *f, const PyTpl &args){
  return c(a(f), args, PyDct());
}

inline
PyBase PyBase::c(const char *f, const PyDct &kwargs){
  return c(a(f), PyTpl(), kwargs);
}

inline
PyBase PyBase::c(const char *f){
  return c(a(f), PyTpl(), PyDct());
}

inline
PyBase PyBase::c(const char *f, int m, bool sf, const char *fmt, ...){
  // m:0=,1=T,2=D,BAD(3=TD),BAD(3<)
/* // BAD subporduction ? -> (must not use nested va_list ?)
  va_list vargs;
  va_start(vargs, fmt);
  PyTpl t = m & 1 ? (PyTpl &)PyBase(sf, fmt, vargs) : PyTpl();
  PyDct d = m & 2 ? (PyDct &)PyBase(sf, fmt, vargs) : PyDct();
  va_end(vargs);
  return c(a(f), t, d);
*/
  va_list vargs;
  va_start(vargs, fmt);
  PyBase r; // call Py_INCREF() later to separate here (outside of try...catch)
  switch(m){
  case 0: r = c(a(f), PyTpl(), PyDct()); break;
/* // BAD -> (must not use nested va_list ?)
  case 1: r = c(a(f), (PyTpl &)PyBase(sf, fmt, vargs), PyDct()); break;
  case 2: r = c(a(f), PyTpl(), (PyDct &)PyBase(sf, fmt, vargs)); break;
*/
  case 1:
try{
    PyObject *s = Py_VaBuildValue(fmt, vargs);
    if(!s){ throw std::runtime_error("Error build value (tpl): "); }
    r = c(a(f), (PyTpl &)PyBase(s, sf), PyDct());
}catch(const std::exception &e){
    p("EXCEPTION in c(f, PyTpl, void)\a");
    throw std::runtime_error(e.what());
}
    break;
  case 2:
try{
    PyObject *s = Py_VaBuildValue(fmt, vargs);
    if(!s){ throw std::runtime_error("Error build value (dct): "); }
    r = c(a(f), PyTpl(), (PyDct &)PyBase(s, sf));
}catch(const std::exception &e){
    p("EXCEPTION in c(f, void, PyDct)\a");
    throw std::runtime_error(e.what());
}
    break;
  default: r = c(a(f), PyTpl(), PyDct());
  }
  va_end(vargs);
  Py_INCREF(r.o()); // *** be careful ***
  return r;
}

template< typename ... Ts >
inline
PyBase PyBase::c(const char *f, const std::tuple< Ts... > &t, const std::unordered_map<const char *, PyBase &> &d, bool askv){
  return a(f, askv)(t, d);
}

template< typename ... Ts >
inline
PyBase PyBase::operator()(const std::tuple< Ts... > &t, const std::unordered_map<const char *, PyBase &> &d){
  PyBase r; // call Py_INCREF() later to separate here (outside of try...catch)
try{
  r = c(*this, PyTpl(t), PyDct(d));
}catch(const std::exception &e){
  p("EXCEPTION in c(f, std::tuple, std::unordered_map)\a");
  throw std::runtime_error(e.what());
}
  Py_INCREF(r.o()); // *** be careful ***
  return r;
}

inline
PyBase PyBase::operator()(const PyTpl &args, const PyDct &kwargs){
  return c(*this, args, kwargs);
}

inline
PyBase PyBase::operator()(const PyTpl &args){
  return c(*this, args, PyDct());
}

inline
PyBase PyBase::operator()(const PyDct &kwargs){
  return c(*this, PyTpl(), kwargs);
}

inline
PyBase PyBase::operator()(){
  return c(*this, PyTpl(), PyDct());
}

// static methods

inline
void PyBase::q(const char *s, int n){
#ifndef PYOB_TRACE_OFF
  if(n != -1) fprintf(stderr, "%s [%08X] ", s, n);
  else fprintf(stderr, "%s ", s);
#endif
}

inline
void PyBase::p(const char *s, int n){
#ifndef PYOB_TRACE_OFF
  if(n != -1) fprintf(stderr, "%s [%08X]\n", s, n);
  else fprintf(stderr, "%s\n", s);
  fflush(stderr);
#endif
}

inline
void PyBase::s(const wchar_t *w, int n){
#ifndef PYOB_TRACE_OFF
  if(n != -1) fwprintf(stderr, L"%s [%08X]\n", w, n);
  else fwprintf(stderr, L"%s\n", w);
  fflush(stderr);
#endif
}

}

#endif // __PYOB_H__
