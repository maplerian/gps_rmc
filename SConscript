from building import *
import rtconfig

cwd     = GetCurrentDir()
src     = Glob('*.c')
CPPPATH = [cwd]
LOCAL_CCFLAGS = ''

if GetDepend(['GPS_RMC_USING_SAMPLE']):
    src += Glob('example/rtt_gps_rmc_example.c')

group = DefineGroup('gps_rmc', src, depend = ['PKG_USING_GPS_RMC'], CPPPATH = CPPPATH, LOCAL_CCFLAGS = LOCAL_CCFLAGS)

Return('group')
