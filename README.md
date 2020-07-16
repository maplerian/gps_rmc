# GPS_RMC

#### 1.简介

用于解析GPS模块的 $XXRMC 类型数据，并放到结构体中。

##### 1.1 目录结构

|  名称   |       说明       |
| :-----: | :--------------: |
| example |   示例文件目录   |
| 根目录  | 源码及头文件目录 |

##### 1.2 许可证

gps_rmc package 遵循 Apachev2.0许可，详见 `LICENSE` 文件。

##### 1.3 依赖

-  依赖 C语言标准库

------

#### 2. 获取方式

###### 通过 git 克隆

```shell
git clone https://github.com/maplerian/gps_rmc.git
```

###### 在 ENV 中的开启

```
RT-Thread online packages  --->
      tools packages  --->
        -*- GPS RMC: Analysis of GPS RMC information
```

#### 3. 示例

```c
#define GPS_DATE_TO_STRING_FORMAT "%d-%02d-%02d %02d:%02d:%02d"
//#define GPS_DATE_TO_STRING_FORMAT "%d/%02d/%02d %02d:%02d:%02d"
#include "gps_rmc.h"

int main(int args, char ** argv)
{
    char *buff[] = {
    	"$GNRMC,230503.000,A,2468.94009,N,11878.72983,E,1.86,256.22,060720,,,A*60"
        "$GNRMC,154123.000,V,,,,,,,060720,,,M*53",
        "$GPRMC,023011.00,V,,,,,,,080720,,,N*71",
        "$GNRMC,,V,,,,,,,,,,M*4E"
	};
    struct gps_info info;
  	uint8_t len = sizeof(buff) / sizeof(uint8_t *), i;
  	printf("\r\nstruct size: %ld\tlen: %d\r\n\r\n", sizeof(struct gps_info),len);
    for (i = 0; i < len; i++) {
    	memset(&info, 0, sizeof(struct gps_info));
        if (strstr((const char *)buff[i], "RMC")) {
          gps_rmc_parse(&info, buff[i]);
          gps_print_info(&info);
        }
    	printf("---------------------------------------------------\r\n");
  	}
  	printf("\r\n");
    return 0;
}
```



#### 4. 联系方式

- 维护：maplerian
- 主页：<https://github.com/maplerian/gps_rmc>

