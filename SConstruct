e = Environment()

e.AppendUnique(CPPPATH=["/usr/lib/llvm-3.0/include"])
e.AppendUnique(CPPDEFINES={"__STDC_LIMIT_MACROS": 1,
                           "__STDC_CONSTANT_MACROS": 1})
e.AppendUnique(CCFLAGS=['-g'])

e.SharedLibrary('hello', 'Hello.cc')
