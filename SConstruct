import subprocess

def getGitHash():
    return subprocess.Popen('git rev-parse --short HEAD', stdout=subprocess.PIPE, shell=True).stdout.read().decode('utf-8').strip()

env = Environment()
env['CCFLAGS']	= '-Wall -Wextra -std=c++11 -O2 -g -DGIT_HASH=' + getGitHash()
env['CPPPATH']	= 'src'
env['LIBS']	= 'asound'

env.VariantDir('build', 'src', duplicate = 0)
env.AlwaysBuild('build/version.o')
qdx_play = env.Program('build/qdx-play', Glob('build/*.cpp'))

env.Install('/usr/local/bin', qdx_play)
env.Alias('install', '/usr/local/bin')
