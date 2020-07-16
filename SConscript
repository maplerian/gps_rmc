'''
@Author: maplerian
@Date: 2020-07-09 11:45:06
@LastEditors: maplerian
@LastEditTime: 2020-07-09 12:14:53
@Description: file content
'''

from building import *
import rtconfig

cwd     = GetCurrentDir()
src     = Glob('*.c')
CPPPATH = [cwd]
LOCAL_CCFLAGS = ''

#if GetDepend(['GPS_RMC_USING_SAMPLE']):
src += Glob('sample/rtt_gps_rmc_sample.c')

group = DefineGroup('gps_rmc', src, depend = ['PKG_USING_GPS_RMC'], CPPPATH = CPPPATH, LOCAL_CCFLAGS = LOCAL_CCFLAGS)

Return('group')
