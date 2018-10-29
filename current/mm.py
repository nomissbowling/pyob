#!/usr/local/bin/python
# -*- coding: utf-8 -*-
'''mm
'''

class mc(object):
  '''test data overwritten by self.n after construction'''
  n = 9999

  def __init__(self, n):
    print(f'TEST before {self.n}')
    self.n = n
    print(f'TEST after {self.n}')
    print('TEST mc')
    # print(__builtins__) # nothing to display (<module 'builtins' (built-in)>)
    # print(__builtins__.__dict__) # {k, v}

  def pi(self, *args, **kwargs):
    print(f'TEST mc.pi ({len(args)}), {kwargs.keys()}, self.n={self.n}')
    # print(f'{args[0].__dict__}') # OK
    # print(f'{kwargs["a"].__dict__}') # OK
    for i, v in enumerate(args): print(f'{i}: [{v}]')
    for k in kwargs.keys(): print(f'[{k}]: [{kwargs[k]}]')
    return {'result': 'XYZ', 'args': args, 'kwargs': kwargs}
