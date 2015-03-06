## -*- Mode: python; py-indent-offset: 4; indent-tabs-mode: nil; coding: utf-8; -*-

import os, copy

def options(opt):
    opt.tool_options("compiler_cxx")

def configure(conf):
    conf.check_tool("compiler_cxx")
    conf.env.append_value('CXXFLAGS', ['-O2', '-g', '-std=c++0x'])#, '-Wall', '-Wextra'])
    conf.env.LIBPATH_MYLIB = ['/usr/local/lib']
    conf.check(compiler='cxx',
               lib='cryptopp',
               mandatory=True, 
               uselib_store='CRYPTOPP')
    conf.check(compiler='cxx',
               lib='ndn-cxx',
               mandatory=True,
               uselib_store='NDN-CXX')
    if not os.environ.has_key('PKG_CONFIG_PATH'):
           os.environ['PKG_CONFIG_PATH'] = ':'.join([
               '/usr/local/lib/pkgconfig',
               '/opt/local/lib/pkgconfig'])

    conf.check_cfg(compiler='cxx',
                   package='libndn-cxx',
                   uselib_store = 'NDN-CXX',
                   args=['--cflags', '--libs'],
                   mandatory=True)

def build(bld):
    libs = ['cryptopp', 'ndn-cxx', 'ssl']
    tests = []
    impls = []
    for s in bld.path.ant_glob('src/*.cpp'):
        ending = str(s)[-6:]
        if ".t.cpp" == ending:
          tests.append(s)
        else:
          impls.append(s)
    for t in tests:
          sources =  copy.copy(impls)
          sources.append(t)
          #print sources
          bld.program(source=sources,
                features='test',
                includes=". src",
                target=str(t)[:-6],
                stlib=libs,
                use=['CRYPTOPP', 'NDN-CXX'])

    from waflib.Tools import waf_unit_test
    bld.add_post_fun(waf_unit_test.summary)