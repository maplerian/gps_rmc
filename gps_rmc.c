/*
 * @Author: maplerian
 * @Date: 2020-07-06 17:05:11
 * @LastEditors: maplerian
 * @LastEditTime: 2020-07-09 21:48:30
 * @Description: file content
 */ 

#include "gps_rmc.h"
// #define GPS_RMC_SHOW_DEBUG

/**
 * @name:   UTC2BTC
 * @msg:    utc日期和时间 转 北京时间
 * @param   {date: 待转换日期结构体指针} 
 * @return: void
 */
void UTC2BTC(gps_date_t *date)
{
    date->hour += 8;
    if (date->hour >= 24)
    {
        date->day++;
        date->hour -= 24;
        if (date->day >= 29)
        {
            if (date->month == 2)
            {
                date->month++;
                if (date->year / 4 != 0 && date->year / 100 == 0)
                    date->day -= 28;
                else
                    date->day -= 29;
            }
            else if (date->day >= 31 && (date->month == 4 || date->month == 6 || date->month == 9 || date->month == 11))
            {
                date->day -= 30;
                date->month += 1;
            }
            else if (date->day >= 32)
            {
                date->day -= 31;
                date->month += 1;
                if (date->month > 12)
                {
                    date->month -= 12;
                    date->year += 1;
                }
            }
        }
    }

    sprintf(
        date->string
        ,GPS_DATE_TO_STRING_FORMAT
        ,date->year
        ,date->month
        ,date->day
        ,date->hour
        ,date->minute
        ,date->second
    );
}
/**
 * @name:   gps_date_parse
 * @msg:    GPS RMC 时间解析
 * @param   {date:  解析结果存储结构体指针, utc_time: utc格式的时间, utc_date: utc格式的日期} 
 * @return: void
 */
void gps_date_parse(gps_date_t *date, uint32_t utc_time, uint32_t utc_date)
{
    if (!date) return;
    date->year = utc_date % 100 + 2000;
    date->month = utc_date % 10000 / 100;
    date->day = utc_date / 10000;
    date->second = utc_time % 100;
    date->minute = utc_time % 10000 / 100;
    date->hour = utc_time / 10000;
    UTC2BTC(date);
}

/**
 * @name:   gps_float_parse
 * @msg:    解析字符串中的float数据
 * @param   {info: 需要解析的结构体, 前提是这个结构体中的 string 必须要有值} 
 * @return: void
 */
void gps_float_parse(gps_float_t *info)
{
    if (!info) return;
    info->value = atof(info->string);
    info->value_h = atol(info->string);
    info->value_l = 0;
    char *p = strstr(info->string,".");
    if (p)
        info->value_l = atol(++p);
#ifdef GPS_RMC_SHOW_DEBUG
    printf("string: %s --> %d.%d\r\n", info->string, info->value_h, info->value_l);
#endif // GPS_RMC_SHOW_DEBUG
}

/**
 *  坐标处理相关函数
 * ------------------------------------------------------------------------
*/
#ifdef GPS_RMC_USE_DMS_FORMAT
/**
 * @name:   gps_source_to_dms_format
 * @msg:    经纬度格式 转换成 度分秒格式
 * @param   {coord: 度分秒存储结构体, source: 经纬度信息结构体} 
 * @return: void
 */
void gps_source_to_dms_format(gps_dms_format_t *coord, gps_float_t *source)
{
    if (!source || !coord) return;
    if (!source->value)
        gps_float_parse(source);
    double temp = source->value;
    coord->d = temp / 100;
    coord->m = temp - coord->d * 100;
    coord->s = ((temp - coord->d * 100) - coord->m) * 60;
    sprintf(coord->string,"%d`%d\'%d\"", coord->d, coord->m, coord->s);
#ifdef GPS_RMC_SHOW_DEBUG
    printf("source: %s --> dms: %s\r\n", source->string, coord->string);
#endif // GPS_RMC_SHOW_DEBUG
}
/**
 * @name:   gps_coord_dms_format_transform
 * @msg:    坐标度分秒格式转换
 * @param   {coord: 待存储转换结构的信息的结构体, source: 从这个结构体中解析转换} 
 * @return: void
 */
void gps_coord_dms_format_transform(gps_coord_dms_format_t *coord, gps_source_coord_t *source)
{
    if (!coord || !source) return ;
    gps_source_to_dms_format(&coord->latitude, &source->latitude);
    gps_source_to_dms_format(&coord->longitude, &source->longitude);
}
#endif //   GPS_RMC_USE_DMS_FORMAT

/**
 * @name:   gps_source_to_location_format
 * @msg:    原始坐标 转 10进制坐标
 * @param   {location: 待存储结构体, source: 从这个结构体中解析转换} 
 * @return: void
 */
void gps_source_to_location_format(gps_float_t *location, gps_float_t *source)
{
    if (!location || !source) return ;
    if (source->value == 0)
        gps_float_parse(source);
    //                      (dddmm / 100 = ddd)    +   (dddmm.mmmmm   -   (dddmm / 100 = ddd) * 100 = ddd00) = mm.mmmmm / 60
    //  ddd + mm.mmmmm/60
    location->value = (double)(source->value_h / 100) + (source->value - (source->value_h / 100) * 100) / 60;
    location->value_h = (uint32_t)location->value;
    location->value_l = (uint32_t)((location->value - (uint32_t)location->value) * 10000000);
    sprintf(location->string,"%d.%d", location->value_h, location->value_l);
#ifdef GPS_RMC_SHOW_DEBUG
    printf("source:%s -> location:%s\r\n", source->string, location->string);
#endif // GPS_RMC_SHOW_DEBUG
}

/**
 * @name:   gps_coord_location_format_transform
 * @msg:    坐标10进制转换
 * @param   {location: 待存储结构体, source: 从这个结构体中解析转换} 
 * @return: void
 */
void gps_coord_location_format_transform(gps_coord_t *location, gps_coord_t * source)
{
    if (!location || !source) return ;
    gps_source_to_location_format(&location->latitude, &source->latitude);
    gps_source_to_location_format(&location->longitude, &source->longitude);
}

/**
 * @name:   gps_coord_handle
 * @msg:    坐标处理
 * @param   {location: 待处理存储结构体} 
 * @return: void
 */
void gps_coord_handle(gps_coord_info_t *location)
{
    if (!location) return ;
#ifdef GPS_RMC_USE_DMS_FORMAT
    gps_coord_dms_format_transform(&location->dms, &location->source);
#endif //   GPS_RMC_USE_DMS_FORMAT
    gps_coord_location_format_transform(&location->location, &location->source);
}
/**
 * ------------------------------------------------------------------------
 *  坐标处理相关函数
*/

/**
 * @name:   gps_print_info
 * @msg:    解析结构体信息,并打印出来
 * @param   {info: 待解析结构体指针} 
 * @return: void
 */
void gps_print_info(gps_info_t info)
{
    if (info->AV == 0 || (info->date.year == 0 || info->date.year == 2000))
    {
        printf("Null\r\n");
        return ;
    }
    printf("Mode: %c\r\n", info->ADEN);
    printf("State: %s\r\n", info->AV == 'A'?"Effective":"Invalid");
    printf("Time: %s\r\n", info->date.string);
    if (info->AV == 'A')
    {
        printf("Location coord: %s %c, %s %c\r\n", info->coord.location.latitude.string, info->EW, info->coord.location.longitude.string, info->NS);
        printf("Speed: %s\r\n",info->speed.string);
        printf("Direction: %s\r\n",info->direction.string);
    }
}
/**
 * @name:   gps_rmc_parse
 * @msg:    gps rmc类型信息解析
 * @param   {info: 待存储解析结构结构体指针, buff: rmc类型信息字符串} 
 * @return: void
 */
char gps_rmc_parse(gps_info_t info, char *buff)
{
    if (!info) return 0;
    if (strlen(buff) < 30)
    {
#ifdef GPS_RMC_SHOW_ERROR_INFO
        printf("[gps.rmc.parse] Invalid Data.\r\n");
#endif // GPS_RMC_SHOW_ERROR_INFO
#ifdef GPS_RMC_SHOW_DEBUG
        printf("无效数据.\r\n");
#endif // GPS_RMC_SHOW_DEBUG
        return 0;
    }
    static char hms_s[15] = {0}, ymd_s[15] = {0};
    static uint32_t hms, ymd;
    //  解析出状态
    sscanf(
        buff,
        "%*[^,],%*[^,],%c,,,,,,,%*[^,],,,%*c",
        &info->AV
    );
    if (info->AV == 'A')
    {
        //  进一步解析所有数据
        sscanf(
            buff,
            "%*[^,],%[^,],%*c,%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],,,%[^,]",
            hms_s,
            info->coord.source.longitude.string,
            &info->NS,
            info->coord.source.latitude.string,
            &info->EW,
            info->speed.string,
            info->direction.string,
            ymd_s,
            &info->ADEN
        );
        //  有效，处理所有数据
        gps_coord_handle(&info->coord);
        sprintf(info->speed.string, "%d.%d", (int)(atof(info->speed.string) * 1.85), (uint32_t)(atof(info->speed.string) * 1850) % 1000);
        gps_float_parse(&info->speed);
        gps_float_parse(&info->direction);
    }
    else
    //  解析正确的时间和模式
        sscanf(
            buff,
            "%*[^,],%[^,],%*c,,,,,,,%[^,],,,%c",
            hms_s,
            ymd_s,
            &info->ADEN
        );
    //  处理时间数据
    hms = atol(hms_s);
    ymd = atol(ymd_s);
    gps_date_parse(&info->date, hms, ymd);
#ifdef GPS_RMC_SHOW_DEBUG
    printf("%s\r\n",buff);
    printf(
        "时间:%s\r\n"
        "状态:%s\r\n"
        "经度:%s %c\r\n"
        "纬度:%s %c\r\n"
        "速度:%s\r\n"
        "方向:%s\r\n"
        "模式:%c\r\n"
        "\r\n"
        ,info->date.string
        ,info->AV == 'A'?"有效":"无效"
        ,info->coord.location.latitude.string,info->EW
        ,info->coord.location.longitude.string,info->NS
        ,info->speed.string
        ,info->direction.string
        ,info->ADEN
    );
#endif // GPS_RMC_SHOW_DEBUG
    return 1;
}
