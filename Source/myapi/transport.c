/***************************************************************************
* File         : transport.c
* Summary      : 
* Version      : v0.1
* Author       : veahow
* Date         : 2019/4/17
* Change Logs  : 
***************************************************************************/

#include "global.h"

SlSockAddrIn_t  sAddr, sLocalAddr;
int iAddrSize = sizeof(SlSockAddrIn_t);;
int sockServ;    // 服务器端套接字
int sockCli;    // 客户端套接字
long            lLoopCount = 0;
char dataBuf[BUF_SIZE];

int led_sock, tmp_sock, hum_sock, pwm_sock;
SlSockAddrIn_t  led_addr, tmp_addr, hum_addr, pwm_addr;
SlSockAddrIn_t  led_des_addr, tmp_des_addr, hum_des_addr, pwm_des_addr;

unsigned short led_port = 8031;
unsigned short tmp_port = 8032;
unsigned short hum_port = 8033;
unsigned short pwm_port = 8034;

unsigned char led_status;
unsigned int temp_int, temp_dec;
unsigned int hum_int, hum_dec;


  // Application specific status/error codes
typedef enum{
    // Choosing -0x7D0 to avoid overlap w/ host-driver's error codes
    LAN_CONNECTION_FAILED = -0x7D0,
    SOCKET_CREATE_ERROR = -0x7D0,
    BIND_ERROR = SOCKET_CREATE_ERROR - 1,
    LISTEN_ERROR = BIND_ERROR -1,
    SOCKET_OPT_ERROR = LISTEN_ERROR -1,
    CONNECT_ERROR = SOCKET_OPT_ERROR -1,
    ACCEPT_ERROR = CONNECT_ERROR - 1,
    SEND_ERROR = ACCEPT_ERROR -1,
    RECV_ERROR = SEND_ERROR -1,
    SOCKET_CLOSE_ERROR = RECV_ERROR -1,
    DEVICE_NOT_IN_STATION_MODE = SOCKET_CLOSE_ERROR - 1,
    STATUS_CODE_MAX = -0xBB8,
    INTERNET_CONNECTION_FAILED = LAN_CONNECTION_FAILED - 1
}e_AppStatusCodes;

/****************************************************************************/
/*				   TCP				            */
/****************************************************************************/

/*******************************************************************************
 函数名：TcpClientCreate
 功  能: 创建TCP客户端套接字
 参  数：usPort 端口号
 返回值：无
*******************************************************************************/
int TcpClientCreate(unsigned short usPort)
{
    //filling the TCP server socket address
    sAddr.sin_family = SL_AF_INET;    // AF_INET协议栈选择
    sAddr.sin_port = sl_Htons((unsigned short)usPort);
    sAddr.sin_addr.s_addr = sl_Htonl((unsigned int)g_ulDestinationIp);

    iAddrSize = sizeof(SlSockAddrIn_t);

    // creating a TCP socket
    // 创建TCP套接字
    sockCli = sl_Socket(SL_AF_INET,SL_SOCK_STREAM, 0);
    if( sockCli < 0 )
    {
        ASSERT_ON_ERROR(SOCKET_CREATE_ERROR);
    }


//    // sending multiple packets to the TCP server
//    while (lLoopCount < g_ulPacketCount)
//    {
//        // sending packet
//        iRetVal = sl_Send(iSockID, dataBuf, sTestBufLen, 0 );
//        if( iRetVal < 0 )
//        {
//            // error
//            sl_Close(iSockID);
//            ASSERT_ON_ERROR(SEND_ERROR);
//        }
//        lLoopCount++;
//    }
//
//    Report("Sent %u packets successfully\n\r",g_ulPacketCount);
//
//    iRetVal = sl_Close(iSockID);
//    //closing the socket after sending 1000 packets
//    ASSERT_ON_ERROR(iRetVal);

    return SUCCESS;
}

/*******************************************************************************
 函数名：TcpClientConnect
 功  能: 使用TCP客户端套接字连接服务器套接字
 参  数：无
 返回值：无
*******************************************************************************/
int TcpClientConnect()
{
    int iRetVal = 0;
    // 连接到TCP服务器
    iRetVal = sl_Connect(sockCli, ( SlSockAddr_t *)&sAddr, iAddrSize);
    if( iRetVal < 0 )
    {
        // error
      // 连接失败关闭socket
        sl_Close(sockCli);       
        ASSERT_ON_ERROR(CONNECT_ERROR);
    }
    
    return SUCCESS;
}

/*******************************************************************************
 函数名：TcpClientSend
 功  能: 使用TCP客户端套接字发送数据
 参  数：str 字符串
 返回值：SUCCESS 成功 | FAILURE 失败
*******************************************************************************/
int TcpClientSend(char str[])
{
  memset(dataBuf, 0, sizeof(dataBuf));    // 字符串清空
  strncpy(dataBuf, str, sizeof(str));
  sl_Send(sockCli, dataBuf, BUF_SIZE, 0 );
      // sending multiple packets to the TCP server
//    while (lLoopCount < g_ulPacketCount)
//    {
//        // sending packet
//        iRetVal = sl_Send(sockCli, dataBuf, sTestBufLen, 0 );
//        if( iRetVal < 0 )
//        {
//            // error
//            sl_Close(sockCli);
//            ASSERT_ON_ERROR(SEND_ERROR);
//        }
//        lLoopCount++;
//    }
//   UART_PRINT("OK???? %d \n", iRetVal);
  return SUCCESS;
}

/*******************************************************************************
 函数名：TcpClientRecv
 功  能: 使用TCP客户端套接字接收数据
 参  数：无
 返回值：SUCCESS 成功 | FAILURE 失败
*******************************************************************************/
int TcpClientRecv()
{
  memset(dataBuf, 0, sizeof(dataBuf));    // 字符串清空
  sl_Recv(sockCli, dataBuf, BUF_SIZE, 0);
  return SUCCESS;
}

/*******************************************************************************
 函数名：TcpClientClose
 功  能: 关闭TCP客户端套接字
 参  数：无
 返回值：无
*******************************************************************************/
void TcpClientClose()
{
   sl_Close(sockCli);
   UART_PRINT("TCP client socket is closed. \n\r");
}

/*******************************************************************************
 函数名：TcpApp
 功  能: 基于TCP的应用程序
 参  数：无
 返回值：无
*******************************************************************************/
//void TcpApp()
//{
//    int iRetVal = 0;
//    
//    iRetVal = TcpClientCreate(PORT_NUM);
//    MAP_UtilsDelay(800000);
//    if(iRetVal < 0)
//    {
//        UART_PRINT("TCP Socket Create Failed \n\r");
//        LOOP_FOREVER();
//    }
//    UART_PRINT("TCP Socket Create Successfully \n\r");
//    
//    iRetVal = TcpClientConnect();
//    if(iRetVal < 0)
//    {
//        UART_PRINT("TCP Socket Connect Failed \n\r");
//        LOOP_FOREVER();
//    }
//    
//    UART_PRINT("TCP Socket Connect Successfully \n\r");
//    MAP_UtilsDelay(800000);
//    
//    
//    // 用完LED灯后使能I2C相关引脚 开发板演示时使用
//    MAP_PinTypeI2C(PIN_01, PIN_MODE_1);
//    MAP_PinTypeI2C(PIN_02, PIN_MODE_1);
//    
//    for(;;){
//          TcpClientRecv();    // 接收服务器发送的数据
//     
//          // led开
//          if(!strcmp(dataBuf, "led_open")){
//            GPIO_IF_LedOn(MCU_RED_LED_GPIO);
//            UART_PRINT("%s \n\r", dataBuf);
//            continue;
//          }
//          
//          // led关
//          if(!strcmp(dataBuf, "led_close")){
//            GPIO_IF_LedOff(MCU_RED_LED_GPIO);
//            UART_PRINT("%s \n\r", dataBuf);
//            continue;
//          }
//
//          
//          // led状态
//          if(!strcmp(dataBuf, "led_status")){
//            unsigned char led_status = GPIO_IF_LedStatus(MCU_RED_LED_GPIO);    // 获取红灯状态
//            UART_PRINT("%s:%d \n\r", dataBuf, led_status);
//            MAP_UtilsDelay(800000);
//            if(led_status == 1) UdpClientSend("1");
//            if(led_status == 0) UdpClientSend("0");
//            
//            continue;
//          }
//          
//          // 温度状态
//          if(!strcmp(dataBuf, "tmp_status")){
//            UART_PRINT("%s \n\r", dataBuf);
//            
//            unsigned int temp_int, temp_dec;
//            SHT2x_Read_Tempature(&temp_int, &temp_dec);    // 读取温度值
//            UART_PRINT("Temperature:%d.%d \n\r", temp_int, temp_dec);
//            MAP_UtilsDelay(800000);
//            
//            char str[10];
//            memset(str, 0, sizeof(str));
//            snprintf(str, sizeof(str), "%d.", temp_int);
//            TcpClientSend(str);
//            
//            MAP_UtilsDelay(800000);
//            MAP_UtilsDelay(800000);
//            
//            memset(str, 0, sizeof(str));
//            snprintf(str, sizeof(str), "%d", temp_dec);
//            TcpClientSend(str);
//            
//            continue;
//          }
//          
//          // 湿度状态
//          if(!strcmp(dataBuf, "hum_status")){
//            unsigned int humi_int, humi_dec;
//            SHT2x_Read_Humidity(&humi_int, &humi_dec);    // 读取湿度值
//            UART_PRINT("Humidity:%d.%d \n\r", humi_int, humi_dec);
//            MAP_UtilsDelay(800000);
//            TcpClientSend("1");
//            
//            continue;
//          }
//          
//          // pwm增加
//          if(!strcmp(dataBuf, "pwm_add")){
//            break;
//          }
//          
//          // pwm减少
//          if(!strcmp(dataBuf, "pwm_reduce")){
//            break;
//          }
//          
//          // pwm状态
//          if(!strcmp(dataBuf, "pwm_status")){
//            break;
//          }
//          
//          if(!strlen(dataBuf)) break;
//          
//          
//    }
//
//    TcpClientClose();
//}


/****************************************************************************/
/*				   UDP				            */
/****************************************************************************/
/*******************************************************************************
 函数名：UdpClientCreate
 功  能: 创建UDP客户端套接字
 参  数：usPort 端口号
 返回值：无
*******************************************************************************/
int UdpClientCreate(unsigned short usPort, int *udp_sock, SlSockAddrIn_t *addr, SlSockAddrIn_t *des_addr)
{
    int iRetVal = 0;
  
    (*addr).sin_family = SL_AF_INET;
    (*addr).sin_port = sl_Htons((unsigned short)usPort);
    (*addr).sin_addr.s_addr = 0;
    
    (*des_addr).sin_family = SL_AF_INET;
    (*des_addr).sin_port = sl_Htons((unsigned short)usPort);
    (*des_addr).sin_addr.s_addr = sl_Htonl((unsigned int)g_ulDestinationIp);
 
    *udp_sock = sl_Socket(SL_AF_INET,SL_SOCK_DGRAM, 0);   // 创建UDP套接字
    if( *udp_sock < 0 )
    {
        // error
        ASSERT_ON_ERROR(SOCKET_CREATE_ERROR);
    }

    // 绑定UDP套接字
    iRetVal = sl_Bind(*udp_sock, (SlSockAddr_t *)addr, iAddrSize);
    if( iRetVal < 0 )
    {
        // error
        sl_Close(*udp_sock);
        ASSERT_ON_ERROR(BIND_ERROR);
    }
    
    UART_PRINT("Device create a UDP socket: PortNum=%d\n\r", usPort);    

  return SUCCESS;
}

/*******************************************************************************
 函数名：UdpClientSend
 功  能: 使用UDP客户端套接字发送数据
 参  数：str 字符串
 返回值：无
*******************************************************************************/
void UdpClientSend(char str[], int udp_sock, SlSockAddrIn_t *des_addr)
{
  sl_SendTo(udp_sock, str, BUF_SIZE, 0, (SlSockAddr_t *)des_addr, iAddrSize);   
}

/*******************************************************************************
 函数名：UdpClientRecv
 功  能: 使用UDP客户端接收数据
 参  数：无
 返回值：无
*******************************************************************************/
void UdpClientRecv(char str[], int udp_sock, SlSockAddrIn_t *addr)
{
  sl_RecvFrom(udp_sock, str, BUF_SIZE, 0,
                     ( SlSockAddr_t *)addr, (SlSocklen_t*)&iAddrSize );   
}

/*******************************************************************************
 函数名：UdpClientClose
 功  能: 关闭UDP客户端套接字
 参  数：无
 返回值：无
*******************************************************************************/
void UdpClientClose(int udp_sock)
{
  sl_Close(udp_sock);
   UART_PRINT("UDP socket is closed. \n\r");
}

/*******************************************************************************
 函数名：UdpApp
 功  能: 基于UDP的应用程序
 参  数：无
 返回值：无
*******************************************************************************/
//void UdpApp()
//{
//    int iRetVal = 0;
//    iRetVal = UdpClientCreate(PORT_NUM);
//    MAP_UtilsDelay(800000);
//    
//    if(iRetVal < 0)
//    {
//        UART_PRINT("UDP Socket Create Failed \n\r");
//        LOOP_FOREVER();
//    }
//    UART_PRINT("UDP Socket Create Successfully \n\r");
//    
//
//    UdpClientSend("wx");     // 向服务器发送wx字符串 保证服务器得到该设备的IP和端口号以便进行通信
//    MAP_UtilsDelay(800000);
//
//    // 用完LED灯后使能I2C相关引脚 开发板演示时使用
//    MAP_PinTypeI2C(PIN_01, PIN_MODE_1);
//    MAP_PinTypeI2C(PIN_02, PIN_MODE_1);
//
//    for(;;){
//          UdpClientRecv();    // 接收服务器发送的数据
//     
//          // led开
//          if(!strcmp(dataBuf, "led_open")){
//            GPIO_IF_LedOn(MCU_RED_LED_GPIO);
//            UART_PRINT("%s \n\r", dataBuf);
//            
//            continue;
//          }
//          
//          // led关
//          if(!strcmp(dataBuf, "led_close")){
//            GPIO_IF_LedOff(MCU_RED_LED_GPIO);
//            UART_PRINT("%s \n\r", dataBuf);
//            
//            continue;
//          }
//
//          
//          // led状态
//          if(!strcmp(dataBuf, "led_status")){
//            unsigned char led_status = GPIO_IF_LedStatus(MCU_RED_LED_GPIO);    // 获取红灯状态
//            UART_PRINT("%s:%d \n\r", dataBuf, led_status);
//            if(led_status == 1) UdpClientSend("1");
//            if(led_status == 0) UdpClientSend("0");
//            MAP_UtilsDelay(800000);
//            continue;
//          }
//          
//          // 温度状态
//          if(!strcmp(dataBuf, "tmp_status")){
//            UART_PRINT("%s \n\r", dataBuf);
//            unsigned int temp_int, temp_dec;
//            SHT2x_Read_Tempature(&temp_int, &temp_dec);    // 读取温度值
//            
//            char str[10];
//            memset(str, 0, sizeof(str));
//            snprintf(str, sizeof(str), "%d.", temp_int);
//            UdpClientSend(str);
//            
//            MAP_UtilsDelay(800000);
//            MAP_UtilsDelay(800000);
//            
//            memset(str, 0, sizeof(str));
//            snprintf(str, sizeof(str), "%d", temp_dec);
//            UdpClientSend(str);
//          }
//          
//          // 湿度状态
//          if(!strcmp(dataBuf, "hum_status")){
//            unsigned int humi_int, humi_dec;
//            SHT2x_Read_Humidity(&humi_int, &humi_dec);    // 读取湿度值
//            UdpClientSend("1");
//          }
//          
//          // pwm增加
//          if(!strcmp(dataBuf, "pwm_add")){
//            break;
//          }
//          
//          // pwm减少
//          if(!strcmp(dataBuf, "pwm_reduce")){
//            break;
//          }
//          
//          // pwm状态
//          if(!strcmp(dataBuf, "pwm_status")){
//            break;
//          }
//          
//          if(!strlen(dataBuf)) break;
//    }
//
//    UdpClientClose();
//}

void UdpLedStatus()
{
  int iRetVal = 0;
   iRetVal = UdpClientCreate(led_port, &led_sock, &led_addr, &led_des_addr);
    MAP_UtilsDelay(800000);
    
    if(iRetVal < 0)
    {
        UART_PRINT("LED UDP Socket Create Failed \n\r");
        LOOP_FOREVER();
    }
    UART_PRINT("LED UDP Socket Create Successfully \n\r");

    for(;;){
          led_status = GPIO_IF_LedStatus(MCU_RED_LED_GPIO);    // 获取红灯状态
          UART_PRINT("LED_STATUS:%d \n\r", led_status);
          if(led_status == 1) UdpClientSend("1", led_sock, &led_des_addr);
          if(led_status == 0) UdpClientSend("0", led_sock, &led_des_addr);
          MAP_UtilsDelay(800000);
          MAP_UtilsDelay(800000);
          MAP_UtilsDelay(800000);
          MAP_UtilsDelay(800000);
          MAP_UtilsDelay(800000);
          MAP_UtilsDelay(800000);
    }            
}

void UdpTmpStatus()
{
   int iRetVal = 0;
   iRetVal = UdpClientCreate(tmp_port, &tmp_sock, &tmp_addr, &tmp_des_addr);
    MAP_UtilsDelay(800000);
    
    if(iRetVal < 0)
    {
        UART_PRINT("Temperature UDP Socket Create Failed \n\r");
        LOOP_FOREVER();
    }
    UART_PRINT("Temperature UDP Socket Create Successfully \n\r");

    for(;;){
            SHT2x_Read_Tempature(&temp_int, &temp_dec);    // 读取温度值
            
            char str[10];
            memset(str, 0, sizeof(str));
            snprintf(str, sizeof(str), "%d.", temp_int);
            UdpClientSend(str, tmp_sock, &tmp_des_addr);
            
            MAP_UtilsDelay(800000);
            MAP_UtilsDelay(800000);
            
            memset(str, 0, sizeof(str));
            snprintf(str, sizeof(str), "%d", temp_dec);
            UdpClientSend(str, tmp_sock, &tmp_des_addr);
    }            
}

void UdpHumStatus()
{
     int iRetVal = 0;
   iRetVal = UdpClientCreate(hum_port, &hum_sock, &hum_addr, &hum_des_addr);
    MAP_UtilsDelay(800000);
    
    if(iRetVal < 0)
    {
        UART_PRINT("Humidity UDP Socket Create Failed \n\r");
        LOOP_FOREVER();
    }
    UART_PRINT("Humidity UDP Socket Create Successfully \n\r");

    for(;;){
            SHT2x_Read_Humidity(&hum_int, &hum_dec);    // 读取湿度值
            
            char str[10];
            memset(str, 0, sizeof(str));
            snprintf(str, sizeof(str), "%d.", hum_int);
            UdpClientSend(str, hum_sock, &hum_des_addr);
            
            MAP_UtilsDelay(800000);
            MAP_UtilsDelay(800000);
            
            memset(str, 0, sizeof(str));
            snprintf(str, sizeof(str), "%d", hum_dec);
            UdpClientSend(str, hum_sock, &hum_des_addr);
    }  
}

void UdpData()
{
    // 用完LED灯后使能I2C相关引脚 开发板演示时使用
    MAP_PinTypeI2C(PIN_01, PIN_MODE_1);
    MAP_PinTypeI2C(PIN_02, PIN_MODE_1);
    
    int iRetVal = 0;
    iRetVal = UdpClientCreate(PORT_NUM, &sockCli, &sLocalAddr, &sAddr);
    MAP_UtilsDelay(800000);
    if(iRetVal < 0)
    {
        UART_PRINT("UDP Socket Create Failed \n\r");
        LOOP_FOREVER();
    }
    UART_PRINT("UDP Socket Create Successfully \n\r");
    
    
    for(;;){
          led_status = GPIO_IF_LedStatus(MCU_RED_LED_GPIO);    // 获取红灯状态
          UART_PRINT("LED_STATUS:%d \n\r", led_status);
          if(led_status == 1) UdpClientSend("1", sockCli, &sAddr);
          if(led_status == 0) UdpClientSend("0", sockCli, &sAddr);
          MAP_UtilsDelay(800000);
          MAP_UtilsDelay(800000);
          MAP_UtilsDelay(800000);
          MAP_UtilsDelay(800000);
          MAP_UtilsDelay(800000);
          
          char str[10];
          SHT2x_Read_Tempature(&temp_int, &temp_dec);    // 读取温度值
          memset(str, 0, sizeof(str));
          snprintf(str, sizeof(str), "%d.", temp_int);
          UdpClientSend(str, sockCli, &sAddr);
          MAP_UtilsDelay(800000);
          MAP_UtilsDelay(800000);
          MAP_UtilsDelay(800000);
          MAP_UtilsDelay(800000);
          MAP_UtilsDelay(800000);
          memset(str, 0, sizeof(str));
          snprintf(str, sizeof(str), "%d", temp_dec);
          UdpClientSend(str, sockCli, &sAddr);      
          
          SHT2x_Read_Humidity(&hum_int, &hum_dec);    // 读取湿度值
          memset(str, 0, sizeof(str));
          snprintf(str, sizeof(str), "%d.", hum_int);
          UdpClientSend(str, sockCli, &sAddr);
          MAP_UtilsDelay(800000);
          MAP_UtilsDelay(800000);
          MAP_UtilsDelay(800000);
          MAP_UtilsDelay(800000);
          MAP_UtilsDelay(800000);
          memset(str, 0, sizeof(str));
          snprintf(str, sizeof(str), "%d", hum_dec);
          UdpClientSend(str, sockCli, &sAddr);
          
          osi_Sleep(10000);
    }
}

void UdpOperation()
{
      for(;;){
          UART_PRINT("UDP Operation Task! \n\r", dataBuf);
          memset(dataBuf, 0, sizeof(dataBuf));
          UdpClientRecv(dataBuf, sockCli, &sLocalAddr);    // 接收服务器发送的数据
     
          // led开
          if(!strcmp(dataBuf, "led_open")){
            GPIO_IF_LedOn(MCU_RED_LED_GPIO);
            UART_PRINT("%s \n\r", dataBuf);
            continue;
          }
          
          // led关
          if(!strcmp(dataBuf, "led_close")){
            GPIO_IF_LedOff(MCU_RED_LED_GPIO);
            UART_PRINT("%s \n\r", dataBuf);
            continue;
          }
          
          // pwm增加
          if(!strcmp(dataBuf, "pwm_add")){
            break;
          }
          
          // pwm减少
          if(!strcmp(dataBuf, "pwm_reduce")){
            break;
          }
          
//          if(!strlen(dataBuf)) break;
            osi_Sleep(10000);
    }
}
