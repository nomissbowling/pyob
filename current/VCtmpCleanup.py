#!/usr/local/bin/python
# -*- coding: utf-8 -*-
'''VCtmpCleanup
'''

import sys, os

D = [
  ('matplotlib_con', 'test_matplotlib_con'),
  ('wincon_0001', 'ConsoleApplication_0001'),
  ('winapp_0001', 'WindowsProject_0001')]

def rmpch(q, s):
  print(q)
  try:
    os.unlink(os.path.join(q, f'{s}.pch'))
  except FileNotFoundError as e:
    print(f'{e}')

def VCtmpCleanup():
  for d, s in D:
    p = os.path.join(d, s)
    print(os.path.join(p, 'x64', 'Release')) # exe
    print(os.path.join(p, 'Release')) # exe
    p = os.path.join(p, s)
    print(p)
    rmpch(os.path.join(p, 'x64', 'Release'), s)
    rmpch(os.path.join(p, 'Release'), s)

if __name__ == '__main__':
  VCtmpCleanup()
