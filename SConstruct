e = Environment()

e.AppendUnique(CPPPATH=["/usr/lib/llvm-3.2/include"])
e.AppendUnique(CPPDEFINES={"__STDC_LIMIT_MACROS": 1,
                           "__STDC_CONSTANT_MACROS": 1})
e.AppendUnique(CCFLAGS=['-g', '-fno-rtti'])

e.SharedLibrary('nwa-output', 'nwa-output.cc')
