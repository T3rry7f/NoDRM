#!/usr/bin/env python

from distutils.core import setup, Extension

setup(
	name = "cdmapi",
	version = "1.0",
	ext_modules = [Extension("cdmapi", ["bind.cpp", "codelift.cpp"],extra_objects = ['libcryptopp.a'])]
	);
