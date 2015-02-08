def options(opt):
    opt.tool_options("compiler_cxx")

def configure(conf):
    conf.check_tool("compiler_cxx")
    conf.env.append_value('CXXFLAGS', ['-O2', '-g', '-std=c++0x'])

def build(bld):
    bld.program(source=bld.path.ant_glob('src/*.cpp'),
                includes=". src",
                target='torrentParser')