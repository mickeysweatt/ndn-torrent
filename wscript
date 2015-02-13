## -*- Mode: python; py-indent-offset: 4; indent-tabs-mode: nil; coding: utf-8; -*-

import os

def options(opt):
    opt.tool_options("compiler_cxx")

def configure(conf):
    conf.check_tool("compiler_cxx")
    conf.env.append_value('CXXFLAGS', ['-O2', '-g', '-std=c++0x'])
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
    bld.program(source=bld.path.ant_glob('src/*.cpp'),
                includes=". src",
                target='torrentParser',
                stlib=libs,
                use=['CRYPTOPP', 'NDN-CXX'])