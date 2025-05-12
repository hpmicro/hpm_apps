/*
 * Copyright (c) 2021 hpmicro
 *
 * Change Logs:
 * Date         Author          Notes
 * 2021-08-13   Fan YANG        first version
 *
 */
#include <stdio.h>
#include <rtthread.h>
#include <rtdevice.h>
#include "rtt_board.h"
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/errno.h>
#include <sys/time.h>
#include <sys/select.h>

void thread_entry(void *arg);
void dual_tcpclient1_thread_entry(void *arg);
void dual_tcpclient2_thread_entry(void *arg);
extern uint8_t get_phy_link_state(uint8_t phy_index);

int main(void)
{

    app_init_led_pins();

    static uint32_t led_thread_arg = 0;
    rt_thread_t led_thread = rt_thread_create("led_th", thread_entry, &led_thread_arg, 1024, 25, 10);
    rt_thread_startup(led_thread);

    static uint32_t tcp_thread1_arg = 0, tcp_thread2_arg = 0;

    rt_thread_t tcp1_thread = rt_thread_create("tcp_th1", dual_tcpclient1_thread_entry, &tcp_thread1_arg, 4096, 25, 10);
    rt_thread_startup(tcp1_thread);

    rt_thread_t tcp2_thread = rt_thread_create("tcp_th2", dual_tcpclient2_thread_entry, &tcp_thread2_arg, 4096, 25, 10);
    rt_thread_startup(tcp2_thread);

    return 0;
}

static const char send_data[] = "This is TCP Client from RT-Thread."; /* 发送用到的数据 */
#define TCP_SERVER_PORT (50002)
#define TCP_SERVER_IP1 "192.168.100.100"
#define TCP_SERVER_IP2 "192.168.100.102"

static uint8_t recv1_buf[1024] = {0};
static uint8_t recv2_buf[1024] = {0};

static int select_read(int fd, uint8_t *buf, int len, int timeout_us)
{
    fd_set rfd;
    struct timeval tv;
    int nfd = 0;

    tv.tv_usec = timeout_us;
    tv.tv_sec = 0;

    FD_ZERO(&rfd);
    FD_SET(fd, &rfd);

    nfd = select(fd+1, &rfd, NULL, NULL, &tv);
    if(nfd < 0)
        return -1;
    if(nfd == 0)
        return 0;
    nfd = recv(fd, buf, len, 0);
    return nfd;
}

static int select_write(int fd, uint8_t *buf, int len, int timeout_us)
{
    fd_set wfd;
    struct timeval tv;
    int nfd = 0;

    tv.tv_usec = timeout_us;
    tv.tv_sec = 0;

    FD_ZERO(&wfd);
    FD_SET(fd, &wfd);

    nfd = select(fd+1, NULL, &wfd, NULL, &tv);
    if(nfd < 0)
        return -1;
    if(nfd == 0)
        return 0;
    nfd = send(fd, buf, len, 0);
    return nfd;
}

void dual_tcpclient1_thread_entry(void *arg)
{
    int port = TCP_SERVER_PORT;
    const char *server_ip = TCP_SERVER_IP2;
    struct sockaddr_in server_addr, client1_addr;
    int fd1;
    int recv_len;
    int mode;
    uint32_t count = 0;
    uint16_t local_port = 10000;

    do
    {

        fd1 = socket(AF_INET, SOCK_STREAM, 0);
        if (fd1 < 0)
        {
            rt_kprintf("create socket fail:%d\r\n", fd1);
            rt_thread_mdelay(500);
            continue;
        }

        client1_addr.sin_family = AF_INET;
        client1_addr.sin_addr.s_addr = inet_addr(RT_LWIP_IPADDR1);
        client1_addr.sin_port = htons(local_port++); // 本地端口要变化，防止上一次bind的未释放导致bind失败，异常会在30s后释放
        if (local_port >= 20000)
        {
            local_port = 10000;
        }
        memset(&(client1_addr.sin_zero), 0, sizeof(server_addr.sin_zero));

        mode = 0;
        ioctlsocket(fd1, FIONBIO, &mode);

        if (bind(fd1, (struct sockaddr *)&client1_addr, sizeof(struct sockaddr)) == -1)
        {
            rt_kprintf("Bind 1 error\n");
            closesocket(fd1);
            fd1 = -1;
            rt_thread_mdelay(500);
            continue;
        }

//        while (!get_phy_link_state(0))
//        {
//            rt_thread_mdelay(200);
//        }

        server_ip = TCP_SERVER_IP1;
        /* 初始化预连接的服务端地址 */
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(TCP_SERVER_PORT);
        server_addr.sin_addr.s_addr = inet_addr(server_ip);
        rt_memset(&(server_addr.sin_zero), 0, sizeof(server_addr.sin_zero));

        /* 连接到服务端 */
        if (connect(fd1, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1)
        {
            rt_kprintf("connect1 fail, serverip:%s\r\n", server_ip);
            server_ip = TCP_SERVER_IP2;
            /* 初始化预连接的服务端地址 */
            server_addr.sin_family = AF_INET;
            server_addr.sin_port = htons(TCP_SERVER_PORT);
            server_addr.sin_addr.s_addr = inet_addr(server_ip);
            rt_memset(&(server_addr.sin_zero), 0, sizeof(server_addr.sin_zero));
            if (connect(fd1, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1)
            {
                rt_kprintf("connect1 fail, serverip:%s\r\n", server_ip);
                closesocket(fd1);
                fd1 = -1;
                rt_thread_mdelay(200);
                continue;
            }
        }

        rt_kprintf("connect1 success:%d, serverip:%s\r\n", fd1, server_ip);

        int keepalive = 1; // 开启keepalive属性

        int keepidle = 20; // 如该连接在20秒内没有任何数据往来,则进行探测

        int keepinterval = 5; // 探测时发包的时间间隔为5 秒

        int keepcount = 4; // 探测尝试的次数.如果第1次探测包就收到响应了,则后2次的不再发.

        setsockopt(fd1, SOL_SOCKET, SO_KEEPALIVE, (void *)&keepalive, sizeof(keepalive));

        setsockopt(fd1, IPPROTO_TCP, TCP_KEEPIDLE, (void *)&keepidle, sizeof(keepidle));

        setsockopt(fd1, IPPROTO_TCP, TCP_KEEPINTVL, (void *)&keepinterval, sizeof(keepinterval));

        setsockopt(fd1, IPPROTO_TCP, TCP_KEEPCNT, (void *)&keepcount, sizeof(keepcount));

        while (1)
        {
//            if (!get_phy_link_state(0))
//            {
//                rt_kprintf("link1 disconnect!\r\n");
//                closesocket(fd1);
//                fd1 = -1;
//                break;
//            }
            memset(recv1_buf, 0, sizeof(recv1_buf));
            recv_len = select_read(fd1, recv1_buf, sizeof(recv1_buf) - 1, 1000*10);
            if(recv_len == 0)
            {
                continue;
            }
            else if (recv_len < 0)
            {
                rt_kprintf("recv1 error:%d, errno:%d\r\n", recv_len, errno);
                closesocket(fd1);
                fd1 = -1;
                rt_thread_mdelay(200);
                break;
            }

            rt_kprintf("recv1 len:%d,:%s\r\n", recv_len, recv1_buf);
            recv_len = snprintf(recv1_buf, 1024, "ip:%s, send count:%d", RT_LWIP_IPADDR1, count++);
            recv_len = select_write(fd1, recv1_buf, recv_len, 1000*10);
            if(recv_len <= 0)
            {
                rt_kprintf("error! send1:%d\r\n", recv_len);
            }
        }
    } while (1);
}

void dual_tcpclient2_thread_entry(void *arg)
{
    int port = TCP_SERVER_PORT;
    const char *server_ip = TCP_SERVER_IP1;
    struct sockaddr_in server_addr, client2_addr;
    int fd2;
    int recv_len;
    int mode;
    uint32_t count = 0;
    uint16_t local_port = 20000;
    do
    {

        fd2 = socket(AF_INET, SOCK_STREAM, 0);
        if (fd2 < 0)
        {
            rt_kprintf("create2 socket fail:%d\r\n", fd2);
            rt_thread_mdelay(500);
            continue;
        }

        client2_addr.sin_family = AF_INET;
        client2_addr.sin_addr.s_addr = inet_addr(RT_LWIP_IPADDR2);
        client2_addr.sin_port = htons(local_port++);
        if (local_port >= 30000)
        {
            local_port = 20000;
        }
        memset(&(client2_addr.sin_zero), 0, sizeof(server_addr.sin_zero));

        mode = 0;
        ioctlsocket(fd2, FIONBIO, &mode);

        if (bind(fd2, (struct sockaddr *)&client2_addr, sizeof(struct sockaddr)) == -1)
        {
            rt_kprintf("Bind 2 error\n");
            closesocket(fd2);
            fd2 = -1;
            rt_thread_mdelay(500);
            continue;
        }

//        while (!get_phy_link_state(1))
//        {
//            rt_thread_mdelay(200);
//        }

        server_ip = TCP_SERVER_IP2;
        /* 初始化预连接的服务端地址 */
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(TCP_SERVER_PORT);
        server_addr.sin_addr.s_addr = inet_addr(server_ip);
        rt_memset(&(server_addr.sin_zero), 0, sizeof(server_addr.sin_zero));

        /* 连接到服务端 */
        if (connect(fd2, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1)
        {
            rt_kprintf("connect2 fail, serverip:%s\r\n", server_ip);
            server_ip = TCP_SERVER_IP1;
            /* 初始化预连接的服务端地址 */
            server_addr.sin_family = AF_INET;
            server_addr.sin_port = htons(TCP_SERVER_PORT);
            server_addr.sin_addr.s_addr = inet_addr(server_ip);
            rt_memset(&(server_addr.sin_zero), 0, sizeof(server_addr.sin_zero));
            if (connect(fd2, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1)
            {
                rt_kprintf("connect2 fail, serverip:%s\r\n", server_ip);
                closesocket(fd2);
                fd2 = -1;
                rt_thread_mdelay(200);
                continue;
            }
        }

        rt_kprintf("connect2 success:%d, serverip:%s\r\n", fd2, server_ip);
        int keepalive = 1; // 开启keepalive属性

        int keepidle = 20; // 如该连接在20秒内没有任何数据往来,则进行探测

        int keepinterval = 5; // 探测时发包的时间间隔为5 秒

        int keepcount = 4; // 探测尝试的次数.如果第1次探测包就收到响应了,则后2次的不再发.

        setsockopt(fd2, SOL_SOCKET, SO_KEEPALIVE, (void *)&keepalive, sizeof(keepalive));

        setsockopt(fd2, IPPROTO_TCP, TCP_KEEPIDLE, (void *)&keepidle, sizeof(keepidle));

        setsockopt(fd2, IPPROTO_TCP, TCP_KEEPINTVL, (void *)&keepinterval, sizeof(keepinterval));

        setsockopt(fd2, IPPROTO_TCP, TCP_KEEPCNT, (void *)&keepcount, sizeof(keepcount));
//        mode = 1;
//        ioctlsocket(fd2, FIONBIO, &mode);
        while (1)
        {
//            if (!get_phy_link_state(1))
//            {
//                rt_kprintf("link2 disconnect!\r\n");
//                closesocket(fd2);
//                fd2 = -1;
//                break;
//            }
            memset(recv2_buf, 0, sizeof(recv2_buf));
            recv_len = select_read(fd2, recv2_buf, sizeof(recv2_buf) - 1, 1000*10);
            if(recv_len == 0)
            {
                continue;
            }
            else if (recv_len < 0)
            {
                rt_kprintf("recv2 error:%d, errno:%d\r\n", recv_len, errno);
                closesocket(fd2);
                fd2 = -1;
                rt_thread_mdelay(200);
                break;
            }

            rt_kprintf("recv2:%s\r\n", recv2_buf);
            recv_len = snprintf(recv2_buf, 1024, "ip:%s, send count:%d", RT_LWIP_IPADDR2, count++);
            recv_len = select_write(fd2, recv2_buf, recv_len, 1000*10);
            if(recv_len <= 0)
            {
                rt_kprintf("error! send2:%d\r\n", recv_len);
            }
        }
    } while (1);
}

void thread_entry(void *arg)
{
    while (1)
    {
        app_led_write(0, APP_LED_ON);
        rt_thread_mdelay(500);
        app_led_write(0, APP_LED_OFF);
        rt_thread_mdelay(500);
        app_led_write(1, APP_LED_ON);
        rt_thread_mdelay(500);
        app_led_write(1, APP_LED_OFF);
        rt_thread_mdelay(500);
        app_led_write(2, APP_LED_ON);
        rt_thread_mdelay(500);
        app_led_write(2, APP_LED_OFF);
        rt_thread_mdelay(500);
    }
}
