from distutils.core import setup, Extension

controller = Extension('controller', sources = ['client_py.c', 'client.c'])

setup (name = 'Controller',
       version = '1.0',
       description = 'DS4 Controller Interface',
       ext_modules = [controller],
       include_dirs = ['../controller', '../ipc'])
