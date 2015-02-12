def options(opt):
    opt.tool_options("compiler_cxx")

def configure(conf):
    conf.check_tool("compiler_cxx")
    conf.env.append_value('CXXFLAGS', ['-O2', '-g', '-std=c++0x', '-Wc++11-extensions'])
    conf.check(compiler='cxx',
               lib='cryptopp',
               mandatory=True, 
                uselib_store='CRYPTOPP')
    conf.check(compiler='cxx',
               lib='ndn-cxx',
               mandatory=True, 
                uselib_store='NDN-CXX')

def build(bld):
    libs = ['cryptopp', 'ndn-cxx', 'ssl']
    bld.program(source=bld.path.ant_glob('src/*.cpp'),
                includes=". src",
                target='torrentParser',
                stlib=libs)