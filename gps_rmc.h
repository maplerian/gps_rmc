/*
 * @Author: maplerian
 * @Date: 2020-07-06 17:05:19
 * @LastEditors: maplerian
 * @LastEditTime: 2020-07-09 13:20:40
 * @Description: file content
 */
#ifndef __GPS_RMC_H__
#define __GPS_RMC_H__

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

//  日期转字符串格式
#ifndef GPS_DATE_TO_STRING_FORMAT
//                               lg:    2020/07/09 13:15:47
#define GPS_DATE_TO_STRING_FORMAT "%d/%02d/%02d %02d:%02d:%02d"
#endif // !GPS_DATE_TO_STRING_FORMAT

//  时间结构体
struct gps_date
{
    uint16_t year;   //  年
    uint8_t month;   //  月
    uint8_t day;     //  日
    uint8_t hour;    //  时
    uint8_t minute;  //  分
    uint8_t second;  //  秒
    char string[20]; //  字符串
};
typedef struct gps_date gps_date_t;

//  gps float 数据
struct gps_float
{
    float value;      //  float值
    uint32_t value_h; //  float 整数
    uint32_t value_l; //  float 小数
    char string[15];  //  float值的字符串
};
typedef struct gps_float gps_float_t;

//  坐标结构体
struct gps_coord
{
    gps_float_t longitude;  //  经度
    gps_float_t latitude; //  维度
};
typedef struct gps_coord gps_coord_t;

#ifdef GPS_RMC_USE_DMS_FORMAT
//  度分秒格式使用的结构体
struct gps_dms_format
{
    uint16_t d;      //  度
    uint16_t m;      //  分
    uint16_t s;      //  秒
    char string[15]; //  度分秒字符串
};
typedef struct gps_dms_format gps_dms_format_t;
//  坐标度分秒格式结构体
struct gps_coord_dms_format
{
    gps_dms_format_t latitude;  //  经度
    gps_dms_format_t longitude; //  维度
};
typedef struct gps_coord_dms_format gps_coord_dms_format_t;
#endif //   GPS_RMC_USE_DMS_FORMAT

//  GPS 坐标信息结构体
struct gps_coord_info
{
#ifdef GPS_RMC_USE_DMS_FORMAT
    gps_coord_dms_format_t dms; //  度分秒格式
#endif //   GPS_RMC_USE_DMS_FORMAT

    gps_coord_t source;   //  原始数据
    gps_coord_t location; //  地图使用的原始GPS坐标
};
typedef struct gps_coord_info gps_coord_info_t;

//  GPS RMC信息解析数据存储结构体
struct gps_info
{
    gps_coord_info_t coord; //  坐标数据
    char AV;                //  'A' / 'V'
    char NS;                //  'N' / 'S'
    char EW;                //  'E' / 'W'
    char ADEN;              //  'A' / 'D' / 'E' / 'N'
    gps_float_t speed;      //  速度
    gps_float_t direction;  //  方向
    gps_date_t date;        //  时间
};
typedef struct gps_info *gps_info_t;

/**
 * @name:   gps_rmc_parse
 * @msg:    gps rmc类型信息解析
 * @param   {info: 待存储解析结构结构体指针, buff: rmc类型信息字符串} 
 * @return: void
 */
char gps_rmc_parse(gps_info_t info, char *buff);

/**
 * @name:   gps_print_info
 * @msg:    解析结构体信息,并打印出来
 * @param   {info: 待解析结构体指针} 
 * @return: void
 */
void gps_print_info(gps_info_t info);

#endif //    __GPS_RMC_H__
