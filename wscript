def options(opt):
    opt.tool_options("compiler_cxx")

def configure(conf):
    conf.check_tool("compiler_cxx")
    conf.env.append_value('CXXFLAGS', ['-O2', '-g', '-std=c++0x'])
    conf.check(compiler='cxx',
               cxxflags=['-Wc++11-extensions'],
	       define_name='HAVE_C11_EXTENSIONS',
               mandatory=False)
    if conf.env['HAVE_C11_EXTENSIONS']:
        conf.env.append_value('CXXFLAGS', ['-Wc++11-extensions'])

    conf.check(compiler='cxx',
               lib='cryptopp',
               mandatory=True, 
               uselib_store='CRYPTOPP')
    conf.check(compiler='cxx',
               lib='ndn-cxx',
               mandatory=True,
               uselib_store='NDN-CXX')
    
    conf.check_cfg(package='libndn-cxx',
                   uselib_store = 'NDN-CXX',
                   args=['--cflags', '--libs'],
                   mandatory=False)

def build(bld):
    libs = ['cryptopp', 'ndn-cxx', 'ssl']
    bld.program(source=bld.path.ant_glob('src/*.cpp'),
                includes=". src",
                target='torrentParser',
                stlib=libs,
		use=['CRYPTOPP', 'NDN-CXX'])