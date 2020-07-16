/*
 * @Author: maplerian
 * @Date: 2020-07-09 11:28:00
 * @LastEditors: maplerian
 * @LastEditTime: 2020-07-09 21:49:30
 * @Description: file content
 */ 
#include <rtthread.h>
#include <rtdevice.h>
#include "gps_rmc.h"

#ifndef GPS_RMC_SAMPLE_UART_NAME
#define GPS_RMC_SAMPLE_UART_NAME "uart6"
#endif // !GPS_RMC_SAMPLE_UART_NAME

/* 用于接收消息的信号量 */
static struct rt_semaphore rx_sem;
static rt_device_t uart;

/* 接收数据回调函数 */
static rt_err_t uart_input(rt_device_t dev, rt_size_t size)
{
    /* 串口接收到数据后产生中断，调用此回调函数，然后发送接收信号量 */
    rt_sem_release(&rx_sem);

    return RT_EOK;
}

void gps_rmc_sample_entry(void *p)
{
    char buff[128] = {0}, *buff_p = buff, ch = 0;
    struct gps_info info_data = {0};
    gps_info_t info = &info_data;
    while (1)
    {
        /* 从串口读取一个字节的数据，没有读取到则等待接收信号量 */
        while (rt_device_read(uart, -1, &ch, 1) != 1)
        {
            /* 阻塞等待接收信号量，等到信号量后再次读取数据 */
            rt_sem_take(&rx_sem, RT_WAITING_FOREVER);
        }
        if (ch == '\n')
        {
            if (rt_strstr((const char *)buff, "RMC"))
            {
                if (gps_rmc_parse(info, buff))
                    gps_print_info(info);
            }
            rt_memset(buff, 0, 128);
            rt_memset(info, 0, sizeof(struct gps_info));
            buff_p = buff;
            continue;
        }
        *buff_p++ = ch;
    }
}

int gps_rmc_sample_entry_init(void)
{

    uart = rt_device_find(GPS_RMC_SAMPLE_UART_NAME);
    if (uart == RT_NULL)
    {
        rt_kprintf("Not find %s device.\r\n", GPS_RMC_SAMPLE_UART_NAME);
        return RT_ERROR;
    }

    /* 初始化信号量 */
    rt_sem_init(&rx_sem, GPS_RMC_SAMPLE_UART_NAME"_rx", 0, RT_IPC_FLAG_FIFO);

    struct serial_configure config = RT_SERIAL_CONFIG_DEFAULT;
    config.baud_rate = BAUD_RATE_9600;
    rt_device_control(uart, RT_DEVICE_CTRL_CONFIG, &config);
    rt_device_open(uart, RT_DEVICE_FLAG_INT_RX);
    rt_device_set_rx_indicate(uart, uart_input);

    rt_thread_t t = rt_thread_create(
        "gps_rmc_p", gps_rmc_sample_entry, RT_NULL, 
        2048, 20, 10
    );
    if (t == RT_NULL)
    {
        rt_kprintf("Failde to create gps rmc info procees thread.\r\n");
        return RT_ERROR;
    }
    if (rt_thread_startup(t) != RT_EOK)
    {
        rt_kprintf("Failde to startup gps rmc info procees thread.\r\n");
        rt_thread_delete(t);
        return RT_ERROR;
    }
    return RT_EOK;
}
INIT_APP_EXPORT(gps_rmc_sample_entry_init);
