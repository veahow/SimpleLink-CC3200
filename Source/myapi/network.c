/***************************************************************************
* File         : network.c
* Summary      : 
* Version      : v0.1
* Author       : veahow
* Date         : 2019/4/17
* Change Logs  : 
***************************************************************************/

#include "global.h"


unsigned long  g_ulStatus = 0;//SimpleLink Status 设备状态
unsigned long  g_ulPingPacketsRecv = 0; //Number of Ping Packets received 接收数据包数
unsigned long  g_ulStaIp = 0;    // 客户端IP地址
unsigned long  g_ulGatewayIP = 0; //Network Gateway IP address 网关IP地址
unsigned char  g_ucConnectionSSID[SSID_LEN_MAX+1]; //Connection SSID 接入点名称
unsigned char  g_ucConnectionBSSID[BSSID_LEN_MAX]; //Connection BSSID 接入点MAC地址
volatile unsigned char g_ucProfileAdded = 1;    // 配置文件已添加
unsigned char g_ucConnectedToConfAP = 0;    // 已连接到配置接入点
volatile unsigned char g_ucProvisioningDone = 0;    // 是否AP配置完成 完成-1 未完成-0
unsigned char g_ucPriority = 0;    // ?
signed char g_cWlanSSID[AP_SSID_LEN_MAX];
signed char g_cWlanSecurityKey[50];
SlSecParams_t g_SecParams;
Sl_WlanNetworkEntry_t g_NetEntries[SCAN_TABLE_SIZE];
char g_token_get [TOKEN_ARRAY_SIZE][STRING_TOKEN_SIZE] = {
  // 6个GET标记
  "__SL_G_US0",    // 检查g_ucConnectedToConfAP的状态
  // 剩下5个用于从g_NetEntries中获取5个接入点的名称
  "__SL_G_US1", 
  "__SL_G_US2",
  "__SL_G_US3",           
  "__SL_G_US4",
  "__SL_G_US5"};
unsigned long  g_ulDestinationIp = IP_ADDR;
unsigned int   g_uiPortNum = PORT_NUM;

volatile unsigned long  g_ulPacketCount = TCP_PACKET_COUNT;
unsigned char  g_ucConnectionStatus = 0;
unsigned char  g_ucSimplelinkstarted = 0;
unsigned long  g_ulIpAddr = 0;



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



//*****************************************************************************
// SimpleLink Asynchronous Event Handlers -- Start
//*****************************************************************************

////*****************************************************************************
////
////! \brief This function serves as first level handler for HTTP GET/POST tokens
////!        It runs under driver context and performs only operation that can run
////!        from this context. For operations that can't is sets an indication of
////!        received token and preempts the provisioning context.
////!
////! \param pSlHttpServerEvent Pointer indicating http server event
////! \param pSlHttpServerResponse Pointer indicating http server response
////!
////! \return None
////!
////*****************************************************************************
//_SlEventPropogationStatus_e sl_Provisioning_HttpServerEventHdl(
//                            SlHttpServerEvent_t    *apSlHttpServerEvent,
//                            SlHttpServerResponse_t *apSlHttpServerResponse)
//{
//	// Unused in this application
//	return EVENT_PROPAGATION_CONTINUE;
//}
//
////*****************************************************************************
////
////! \brief This function serves as first level network application events handler.
////!        It runs under driver context and performs only operation that can run
////!        from this context. For operations that can't is sets an indication of
////!        received token and preempts the provisioning context.
////!
////! \param apEventInfo Pointer to the net app event information
////!
////! \return None
////!
////*****************************************************************************
//_SlEventPropogationStatus_e sl_Provisioning_NetAppEventHdl(SlNetAppEvent_t *apNetAppEvent)
//{
//	// Unused in this application
//	return EVENT_PROPAGATION_CONTINUE;
//}
//
////*****************************************************************************
////
////! \brief This function serves as first level WLAN events handler.
////!        It runs under driver context and performs only operation that can run
////!        from this context. For operations that can't is sets an indication of
////!        received token and preempts the provisioning context.
////!
////! \param apEventInfo Pointer to the WLAN event information
////!
////! \return None
////!
////*****************************************************************************
//_SlEventPropogationStatus_e sl_Provisioning_WlanEventHdl(SlWlanEvent_t *apEventInfo)
//{
//	// Unused in this application
//	return EVENT_PROPAGATION_CONTINUE;
//}

//*****************************************************************************
// SimpleLink Asynchronous Event Handlers -- End
//*****************************************************************************





/*******************************************************************************
 函数名：SimpleLinkWlanEventHandler
 功  能: Wlan成功连接或断开异步事件处理函数（类似事件中断函数）
 参  数：*pSlWlanEvent 事件类型的pSlWlanEvent指针
 返回值：无
*******************************************************************************/
void SimpleLinkWlanEventHandler(SlWlanEvent_t *pWlanEvent)
{
  // WlanConnect()运行时调用WLAN事件处理程序和NETAPP事件处理程序
    switch(pWlanEvent->Event)
    {
        case SL_WLAN_CONNECT_EVENT:    // WLAN连接事件
        {
          // 将设备状态设置为连接
            SET_STATUS_BIT(g_ulStatus, STATUS_BIT_CONNECTION);
            
            //
            // Information about the connected AP (like name, MAC etc) will be
            // available in 'slWlanConnectAsyncResponse_t'-Applications
            // can use it if required
            //
            //  slWlanConnectAsyncResponse_t *pEventData = NULL;
            // pEventData = &pWlanEvent->EventData.STAandP2PModeWlanConnected;
            //
            
            // Copy new connection SSID and BSSID to global parameters
            // WLAN事件数据STAandP2PModeWlanConnected主要包括ssid_name（接入点名称）和bssid（接入点MAC地址）
            memcpy(g_ucConnectionSSID,pWlanEvent->EventData.
                   STAandP2PModeWlanConnected.ssid_name,
                   pWlanEvent->EventData.STAandP2PModeWlanConnected.ssid_len);
            memcpy(g_ucConnectionBSSID,
                   pWlanEvent->EventData.STAandP2PModeWlanConnected.bssid,
                   SL_BSSID_LENGTH);
            
            // 向串口打印连接AP的相关信息
            UART_PRINT("[WLAN EVENT] STA Connected to the AP: %s ,"
                        "BSSID: %x:%x:%x:%x:%x:%x\n\r",
                      g_ucConnectionSSID,g_ucConnectionBSSID[0],
                      g_ucConnectionBSSID[1],g_ucConnectionBSSID[2],
                      g_ucConnectionBSSID[3],g_ucConnectionBSSID[4],
                      g_ucConnectionBSSID[5]);
        }
        break;

        case SL_WLAN_DISCONNECT_EVENT:    // WLAN断开事件
        {
            slWlanConnectAsyncResponse_t*  pEventData = NULL;

            CLR_STATUS_BIT(g_ulStatus, STATUS_BIT_CONNECTION);
            CLR_STATUS_BIT(g_ulStatus, STATUS_BIT_IP_AQUIRED);

            pEventData = &pWlanEvent->EventData.STAandP2PModeDisconnected;

            // If the user has initiated 'Disconnect' request, 
            //'reason_code' is SL_WLAN_DISCONNECT_USER_INITIATED_DISCONNECTION 
            if(SL_WLAN_DISCONNECT_USER_INITIATED_DISCONNECTION == pEventData->reason_code)
            {
                UART_PRINT("[WLAN EVENT]Device disconnected from the AP: %s,"
                "BSSID: %x:%x:%x:%x:%x:%x on application's request \n\r",
                           g_ucConnectionSSID,g_ucConnectionBSSID[0],
                           g_ucConnectionBSSID[1],g_ucConnectionBSSID[2],
                           g_ucConnectionBSSID[3],g_ucConnectionBSSID[4],
                           g_ucConnectionBSSID[5]);
            }
            else
            {
                UART_PRINT("[WLAN ERROR]Device disconnected from the AP AP: %s,"
                "BSSID: %x:%x:%x:%x:%x:%x on an ERROR..!! \n\r",
                           g_ucConnectionSSID,g_ucConnectionBSSID[0],
                           g_ucConnectionBSSID[1],g_ucConnectionBSSID[2],
                           g_ucConnectionBSSID[3],g_ucConnectionBSSID[4],
                           g_ucConnectionBSSID[5]);
            }
            memset(g_ucConnectionSSID,0,sizeof(g_ucConnectionSSID));
            memset(g_ucConnectionBSSID,0,sizeof(g_ucConnectionBSSID));
        }
        break;
        
        
        case SL_WLAN_STA_CONNECTED_EVENT:
        {
            // when device is in AP mode and any client connects to device cc3xxx
            SET_STATUS_BIT(g_ulStatus, STATUS_BIT_CONNECTION);
            UART_PRINT("[WLAN EVENT] SL_WLAN_STA_CONNECTED_EVENT \n");
            //
            // Information about the connected client (like SSID, MAC etc) will be
            // available in 'slPeerInfoAsyncResponse_t' - Applications
            // can use it if required
            //
            // slPeerInfoAsyncResponse_t *pEventData = NULL;
            // pEventData = &pSlWlanEvent->EventData.APModeStaConnected;
            //

        }
        break;

        case SL_WLAN_STA_DISCONNECTED_EVENT:
        {
            // when client disconnects from device (AP)
            CLR_STATUS_BIT(g_ulStatus, STATUS_BIT_CONNECTION);
            CLR_STATUS_BIT(g_ulStatus, STATUS_BIT_IP_LEASED);
            UART_PRINT("[WLAN EVENT] SL_WLAN_STA_DISCONNECTED_EVENT \n");
            //
            // Information about the connected client (like SSID, MAC etc) will
            // be available in 'slPeerInfoAsyncResponse_t' - Applications
            // can use it if required
            //
            // slPeerInfoAsyncResponse_t *pEventData = NULL;
            // pEventData = &pSlWlanEvent->EventData.APModestaDisconnected;
            //            
        }
        break;

        case SL_WLAN_CONNECTION_FAILED_EVENT:
        {
            // If device gets any connection failed event
            SET_STATUS_BIT(g_ulStatus, STATUS_BIT_CONNECTION_FAILED);
        }
        break;

        default:
        {
            UART_PRINT("[WLAN EVENT] Unexpected event [0x%x]\n\r",
                       pWlanEvent->Event);
        }
        break;
    }
}

/*******************************************************************************
 函数名：SimpleLinkNetAppEventHandler
 功  能: NetApp异步事件处理函数（类似事件中断函数）
 参  数：*pNetAppEvent 事件类型的SlNetAppEvent指针
 返回值：无
*******************************************************************************/
void SimpleLinkNetAppEventHandler(SlNetAppEvent_t *pNetAppEvent)
{
  // WlanConnect()运行时调用WLAN事件处理程序和NETAPP事件处理程序
  
    if(!pNetAppEvent)
    {
        return;
    }
  
  
    switch(pNetAppEvent->Event)
    {
        case SL_NETAPP_IPV4_IPACQUIRED_EVENT:    // 获得IPv4事件
        {
            SlIpV4AcquiredAsync_t *pEventData = NULL;
            
            // 设备状态为获取IP状态
            SET_STATUS_BIT(g_ulStatus, STATUS_BIT_IP_AQUIRED);
            
            //Ip Acquired Event Data
            // 获取IP数据
            pEventData = &pNetAppEvent->EventData.ipAcquiredV4;
             g_ulIpAddr = pEventData->ip;
            
            //Gateway IP address
            // 获取网关IP地址
            g_ulGatewayIP = pEventData->gateway;
            
            // 向串口打印IP相关信息
            UART_PRINT("[NETAPP EVENT] IP Acquired: IP=%d.%d.%d.%d , "
            "Gateway=%d.%d.%d.%d\n\r", 
            SL_IPV4_BYTE(pNetAppEvent->EventData.ipAcquiredV4.ip,3),
            SL_IPV4_BYTE(pNetAppEvent->EventData.ipAcquiredV4.ip,2),
            SL_IPV4_BYTE(pNetAppEvent->EventData.ipAcquiredV4.ip,1),
            SL_IPV4_BYTE(pNetAppEvent->EventData.ipAcquiredV4.ip,0),
            SL_IPV4_BYTE(pNetAppEvent->EventData.ipAcquiredV4.gateway,3),
            SL_IPV4_BYTE(pNetAppEvent->EventData.ipAcquiredV4.gateway,2),
            SL_IPV4_BYTE(pNetAppEvent->EventData.ipAcquiredV4.gateway,1),
            SL_IPV4_BYTE(pNetAppEvent->EventData.ipAcquiredV4.gateway,0));
        }
        break;
        
        case SL_NETAPP_IPV6_IPACQUIRED_EVENT:    // 获得IPv6事件
        {
            SET_STATUS_BIT(g_ulStatus, STATUS_BIT_IP_AQUIRED);
        }
        break;
        
        case SL_NETAPP_IP_LEASED_EVENT:
        {
            SET_STATUS_BIT(g_ulStatus, STATUS_BIT_IP_LEASED);
        
            g_ulStaIp = (pNetAppEvent)->EventData.ipLeased.ip_address;
            
            UART_PRINT("[NETAPP EVENT] IP Leased to Client: IP=%d.%d.%d.%d , ",
                        SL_IPV4_BYTE(g_ulStaIp,3), SL_IPV4_BYTE(g_ulStaIp,2),
                        SL_IPV4_BYTE(g_ulStaIp,1), SL_IPV4_BYTE(g_ulStaIp,0));
        }
        break;
        
        case SL_NETAPP_IP_RELEASED_EVENT:
        {
            CLR_STATUS_BIT(g_ulStatus, STATUS_BIT_IP_LEASED);

            UART_PRINT("[NETAPP EVENT] IP Released for Client: IP=%d.%d.%d.%d , ",
                        SL_IPV4_BYTE(g_ulStaIp,3), SL_IPV4_BYTE(g_ulStaIp,2),
                        SL_IPV4_BYTE(g_ulStaIp,1), SL_IPV4_BYTE(g_ulStaIp,0));

        }
        break;
        

        default:
        {
            UART_PRINT("[NETAPP EVENT] Unexpected event [0x%x] \n\r",
                       pNetAppEvent->Event);
        }
        break;
    }
}


/*******************************************************************************
 函数名：SimpleLinkHttpServerCallback
 功  能: HttpServer回调函数
 参  数：*pSlHttpServerEvent HTTP服务器事件指针 包括事件号和事件数据
         *pSlHttpServerResponse HTTP服务器响应数据指针 用户的响应信息
 返回值：无
*******************************************************************************/
void SimpleLinkHttpServerCallback(SlHttpServerEvent_t *pSlHttpServerEvent,
        SlHttpServerResponse_t *pSlHttpServerResponse)
{
  // provisioning_ap模式来配置wifi的信息
  // 使用http来进行配置
  // 暂未理解
    switch (pSlHttpServerEvent->Event)
    {
        case SL_NETAPP_HTTPGETTOKENVALUE_EVENT:    // GET标记值事件
        {

            if(0== memcmp(pSlHttpServerEvent->EventData.httpTokenName.data, \
                            g_token_get [0], \
                            pSlHttpServerEvent->EventData.httpTokenName.len))
            {
                if(g_ucConnectedToConfAP == 1)
                {
                    // Important - Connection Status
                    memcpy(pSlHttpServerResponse->ResponseData.token_value.data, \
                            "TRUE",strlen("TRUE"));
                    pSlHttpServerResponse->ResponseData.token_value.len = \
                                                                strlen("TRUE");
                }
                else
                {
                    // Important - Connection Status
                    memcpy(pSlHttpServerResponse->ResponseData.token_value.data, \
                            "FALSE",strlen("FALSE"));
                    pSlHttpServerResponse->ResponseData.token_value.len = \
                                                                strlen("FALSE");
                }
            }

            if(0== memcmp(pSlHttpServerEvent->EventData.httpTokenName.data, \
                            g_token_get [1], \
                            pSlHttpServerEvent->EventData.httpTokenName.len))
            {
                // Important - Token value len should be < MAX_TOKEN_VALUE_LEN
                memcpy(pSlHttpServerResponse->ResponseData.token_value.data, \
                        g_NetEntries[0].ssid,g_NetEntries[0].ssid_len);
                pSlHttpServerResponse->ResponseData.token_value.len = \
                                                      g_NetEntries[0].ssid_len;
            }
            if(0== memcmp(pSlHttpServerEvent->EventData.httpTokenName.data, \
                            g_token_get [2], \
                            pSlHttpServerEvent->EventData.httpTokenName.len))
            {
                // Important - Token value len should be < MAX_TOKEN_VALUE_LEN
                memcpy(pSlHttpServerResponse->ResponseData.token_value.data, \
                        g_NetEntries[1].ssid,g_NetEntries[1].ssid_len);
                pSlHttpServerResponse->ResponseData.token_value.len = \
                                                       g_NetEntries[1].ssid_len;
            }
            if(0== memcmp(pSlHttpServerEvent->EventData.httpTokenName.data, \
                            g_token_get [3], \
                            pSlHttpServerEvent->EventData.httpTokenName.len))
            {
                // Important - Token value len should be < MAX_TOKEN_VALUE_LEN
                memcpy(pSlHttpServerResponse->ResponseData.token_value.data, \
                        g_NetEntries[2].ssid,g_NetEntries[2].ssid_len);
                pSlHttpServerResponse->ResponseData.token_value.len = \
                                                       g_NetEntries[2].ssid_len;
            }
            if(0== memcmp(pSlHttpServerEvent->EventData.httpTokenName.data, \
                            g_token_get [4], \
                            pSlHttpServerEvent->EventData.httpTokenName.len))
            {
                // Important - Token value len should be < MAX_TOKEN_VALUE_LEN
                memcpy(pSlHttpServerResponse->ResponseData.token_value.data, \
                        g_NetEntries[3].ssid,g_NetEntries[3].ssid_len);
                pSlHttpServerResponse->ResponseData.token_value.len = \
                                                       g_NetEntries[3].ssid_len;
            }
            if(0== memcmp(pSlHttpServerEvent->EventData.httpTokenName.data, \
                            g_token_get [5], \
                            pSlHttpServerEvent->EventData.httpTokenName.len))
            {
                // Important - Token value len should be < MAX_TOKEN_VALUE_LEN
                memcpy(pSlHttpServerResponse->ResponseData.token_value.data, \
                        g_NetEntries[4].ssid,g_NetEntries[4].ssid_len);
                pSlHttpServerResponse->ResponseData.token_value.len = \
                                                      g_NetEntries[4].ssid_len;
            }

            else
                break;

        }
        break;

        case SL_NETAPP_HTTPPOSTTOKENVALUE_EVENT:    // POST标记值事件
        {
          
          // 网页中定义6个POST标记
          
          // __SL_P_USC 发送标记值
            if((0 == memcmp(pSlHttpServerEvent->EventData.httpPostData.token_name.data, \
                          "__SL_P_USC", \
                 pSlHttpServerEvent->EventData.httpPostData.token_name.len)) && \
            (0 == memcmp(pSlHttpServerEvent->EventData.httpPostData.token_value.data, \
                     "Connect", \
                     pSlHttpServerEvent->EventData.httpPostData.token_value.len)))
            {
                g_ucProfileAdded = 0;

            }
            
            // __SL_P_USD 发送接入点名称
            if(0 == memcmp(pSlHttpServerEvent->EventData.httpPostData.token_name.data, \
                     "__SL_P_USD", \
                     pSlHttpServerEvent->EventData.httpPostData.token_name.len))
            {
                memcpy(g_cWlanSSID,  \
                pSlHttpServerEvent->EventData.httpPostData.token_value.data, \
                pSlHttpServerEvent->EventData.httpPostData.token_value.len);
                g_cWlanSSID[pSlHttpServerEvent->EventData.httpPostData.token_value.len] = 0;
            }
            
            // __SL_P_USE 发送安全类型
            if(0 == memcmp(pSlHttpServerEvent->EventData.httpPostData.token_name.data, \
                         "__SL_P_USE", \
                         pSlHttpServerEvent->EventData.httpPostData.token_name.len))
            {

                if(pSlHttpServerEvent->EventData.httpPostData.token_value.data[0] \
                                                                        == '0')
                {
                    g_SecParams.Type =  SL_SEC_TYPE_OPEN;//SL_SEC_TYPE_OPEN
    
                }
                else if(pSlHttpServerEvent->EventData.httpPostData.token_value.data[0] \
                                                                        == '1')
                {
                    g_SecParams.Type =  SL_SEC_TYPE_WEP;//SL_SEC_TYPE_WEP

                }
                else if(pSlHttpServerEvent->EventData.httpPostData.token_value.data[0] == '2')
                {
                    g_SecParams.Type =  SL_SEC_TYPE_WPA;//SL_SEC_TYPE_WPA

                }
                else
                {
                    g_SecParams.Type =  SL_SEC_TYPE_OPEN;//SL_SEC_TYPE_OPEN
                }
            }
            
            // __SL_P_USF 发送安全密码
            if(0 == memcmp(pSlHttpServerEvent->EventData.httpPostData.token_name.data, \
                         "__SL_P_USF", \
                         pSlHttpServerEvent->EventData.httpPostData.token_name.len))
            {
                memcpy(g_cWlanSecurityKey, \
                    pSlHttpServerEvent->EventData.httpPostData.token_value.data, \
                    pSlHttpServerEvent->EventData.httpPostData.token_value.len);
                g_cWlanSecurityKey[pSlHttpServerEvent->EventData.httpPostData.token_value.len] = 0;
                g_SecParams.Key = g_cWlanSecurityKey;
                g_SecParams.KeyLen = pSlHttpServerEvent->EventData.httpPostData.token_value.len;
            }
            
            // __SL_P_USG 发送配置文件优先级
//            if(0 == memcmp(pSlHttpServerEvent->EventData.httpPostData.token_name.data, \
//                        "__SL_P_USG", \
//                        pSlHttpServerEvent->EventData.httpPostData.token_name.len))
//            {
//                g_ucPriority = pSlHttpServerEvent->EventData.httpPostData.token_value.data[0] - 48;
//            }
            
            // __SL_P_US0 通知设备配置完成
            if(0 == memcmp(pSlHttpServerEvent->EventData.httpPostData.token_name.data, \
                         "__SL_P_US0", \
                         pSlHttpServerEvent->EventData.httpPostData.token_name.len))
            {
                g_ucProvisioningDone = 1;
            }
        }
        break;
        
      default:
          break;
    }
}

/*******************************************************************************
 函数名：SimpleLinkGeneralEventHandler
 功  能: 一般事件的回调函数
 参  数：*pDevEvent 相关一般事件信息指针
 返回值：无
*******************************************************************************/
void SimpleLinkGeneralEventHandler(SlDeviceEvent_t *pDevEvent)
{
    //
    // Most of the general errors are not FATAL are are to be handled
    // appropriately by the application
    //
    UART_PRINT("[GENERAL EVENT] - ID=[%d] Sender=[%d]\n\n",
               pDevEvent->EventData.deviceEvent.status, 
               pDevEvent->EventData.deviceEvent.sender);
    
    // 该函数暂未理解
}


/*******************************************************************************
 函数名：SimpleLinkSockEventHandler
 功  能: Socket事件的回调函数
 参  数：*pSock Socket事件信息指针
 返回值：无
*******************************************************************************/
void SimpleLinkSockEventHandler(SlSockEvent_t *pSock)
{
  // 该函数暂未理解
  
    //
    // This application doesn't work w/ socket - Events are not expected
    //
  
    if(!pSock)
    {
        return;
    }
    
    switch( pSock->Event )
    {
        case SL_SOCKET_TX_FAILED_EVENT:
            switch( pSock->socketAsyncEvent.SockTxFailData.status)
            {
                case SL_ECLOSE: 
                    UART_PRINT("[SOCK ERROR] - close socket (%d) operation "
                                "failed to transmit all queued packets\n\n", 
                                    pSock->socketAsyncEvent.SockTxFailData.sd);
                    break;
                default: 
                    UART_PRINT("[SOCK ERROR] - TX FAILED  :  socket %d , reason "
                                "(%d) \n\n",
                                pSock->socketAsyncEvent.SockTxFailData.sd, pSock->socketAsyncEvent.SockTxFailData.status);
                  break;
            }
            break;
            
            // 也要多添点case
            

        default:
        	UART_PRINT("[SOCK EVENT] - Unexpected Event [%x0x]\n\n",pSock->Event);
          break;
    }

}


/*******************************************************************************
 函数名：SimpleLinkPingReport
 功  能: Ping测试
 参  数：pPingReport
 返回值：无
*******************************************************************************/
static void SimpleLinkPingReport(SlPingReport_t *pPingReport)
{
    SET_STATUS_BIT(g_ulStatus, STATUS_BIT_PING_DONE);
    g_ulPingPacketsRecv = pPingReport->PacketsReceived;
}

//*****************************************************************************
// SimpleLink Asynchronous Event Handlers -- End
//*****************************************************************************



/*******************************************************************************
 函数名：InitializeAppVariables
 功  能: 初始化应用变量函数
 参  数：无
 返回值：无
*******************************************************************************/
static void InitializeAppVariables()
{
    g_ulStatus = 0;    // 设备状态为0
    g_ulStaIp = 0;    // 站点的IP地址为0
    g_ulPingPacketsRecv = 0;    // 接收数据包数为0
    g_ulGatewayIP = 0;    // 网关IP地址为0
    
    g_ucProfileAdded = 1;
    g_ucConnectedToConfAP = 0;
    g_ucProvisioningDone = 0;
    
    g_ulDestinationIp = IP_ADDR;
    g_uiPortNum = PORT_NUM;
    g_ulPacketCount = TCP_PACKET_COUNT;

    // 少了几个变量
    memset(g_ucConnectionSSID,0,sizeof(g_ucConnectionSSID));
    memset(g_ucConnectionBSSID,0,sizeof(g_ucConnectionBSSID));
}


/*******************************************************************************
 函数名：ConfigureSimpleLinkToDefaultState
 功  能: 器件配置成初始状态
         - 将模式设置为STATION
         - 将连接策略配置为Auto和AutoSmartConfig
         - 删除所有存储的配置文件
         - 启用DHCP
         - 禁用扫描策略
         - 将Tx功率设置为最大值
         - 将电源策略设置为正常
         - 取消注册mDNS服务
         - 删除所有过滤器
 参  数：无
 返回值：0-成功 负值-错误
*******************************************************************************/
static long ConfigureSimpleLinkToDefaultState()
{
    SlVersionFull   ver = {0};
    _WlanRxFilterOperationCommandBuff_t  RxFilterIdMask = {0};

    unsigned char ucVal = 1;
    unsigned char ucConfigOpt = 0;
    unsigned char ucConfigLen = 0;
    unsigned char ucPower = 0;

    long lRetVal = -1;
    long lMode = -1;

    lMode = sl_Start(0, 0, 0);
    ASSERT_ON_ERROR(lMode);

    // If the device is not in station-mode, try configuring it in station-mode 
    if (ROLE_STA != lMode)
    {
        if (ROLE_AP == lMode)
        {
            // If the device is in AP mode, we need to wait for this event 
            // before doing anything 
            while(!IS_IP_ACQUIRED(g_ulStatus))
            {
#ifndef SL_PLATFORM_MULTI_THREADED
              _SlNonOsMainLoopTask(); 
#endif
            }
        }

        // Switch to STA role and restart 
        lRetVal = sl_WlanSetMode(ROLE_STA);
        ASSERT_ON_ERROR(lRetVal);

        lRetVal = sl_Stop(0xFF);
        ASSERT_ON_ERROR(lRetVal);

        // 启动SimpleLink 
        // 返回值：0-站点模式（ROLE_STA） 2-接入点模式（ROLE_AP） 3-点对点模式（P2P） 负值-错误
        lRetVal = sl_Start(0, 0, 0);
        ASSERT_ON_ERROR(lRetVal);

        // Check if the device is in station again 
        if (ROLE_STA != lRetVal)
        {
            // We don't want to proceed if the device is not coming up in STA-mode 
            ASSERT_ON_ERROR(DEVICE_NOT_IN_STATION_MODE);
            
        }
    }
    
    // Get the device's version-information
    ucConfigOpt = SL_DEVICE_GENERAL_VERSION;
    ucConfigLen = sizeof(ver);
    // 获取设备版本函数
    lRetVal = sl_DevGet(SL_DEVICE_GENERAL_CONFIGURATION, &ucConfigOpt, 
                                &ucConfigLen, (unsigned char *)(&ver));
    ASSERT_ON_ERROR(lRetVal);
    
    UART_PRINT("Host Driver Version: %s\n\r",SL_DRIVER_VERSION);
    UART_PRINT("Build Version %d.%d.%d.%d.31.%d.%d.%d.%d.%d.%d.%d.%d\n\r",
    ver.NwpVersion[0],ver.NwpVersion[1],ver.NwpVersion[2],ver.NwpVersion[3],
    ver.ChipFwAndPhyVersion.FwVersion[0],ver.ChipFwAndPhyVersion.FwVersion[1],
    ver.ChipFwAndPhyVersion.FwVersion[2],ver.ChipFwAndPhyVersion.FwVersion[3],
    ver.ChipFwAndPhyVersion.PhyVersion[0],ver.ChipFwAndPhyVersion.PhyVersion[1],
    ver.ChipFwAndPhyVersion.PhyVersion[2],ver.ChipFwAndPhyVersion.PhyVersion[3]);

    // Set connection policy to Auto + SmartConfig 
    //      (Device's default connection policy)
    // 设置连接策略函数
    lRetVal = sl_WlanPolicySet(SL_POLICY_CONNECTION, 
                                SL_CONNECTION_POLICY(1, 0, 0, 0, 1), NULL, 0);
    ASSERT_ON_ERROR(lRetVal);

    // Remove all profiles
    // 删除WLAN配置文件
    lRetVal = sl_WlanProfileDel(0xFF);
    ASSERT_ON_ERROR(lRetVal);

    

    //
    // Device in station-mode. Disconnect previous connection if any
    // The function returns 0 if 'Disconnected done', negative number if already
    // disconnected Wait for 'disconnection' event if 0 is returned, Ignore 
    // other return-codes
    //
    // 断开WLAN连接
    lRetVal = sl_WlanDisconnect();
    if(0 == lRetVal)
    {
        // Wait
        while(IS_CONNECTED(g_ulStatus))
        {
#ifndef SL_PLATFORM_MULTI_THREADED
              _SlNonOsMainLoopTask(); 
#endif
        }
    }

    // Enable DHCP client
    // 设置网络配置 允许DHCP
    lRetVal = sl_NetCfgSet(SL_IPV4_STA_P2P_CL_DHCP_ENABLE,1,1,&ucVal);
    ASSERT_ON_ERROR(lRetVal);

    // Disable scan
    // 禁止WLAN扫描
    ucConfigOpt = SL_SCAN_POLICY(0);
    lRetVal = sl_WlanPolicySet(SL_POLICY_SCAN , ucConfigOpt, NULL, 0);
    ASSERT_ON_ERROR(lRetVal);

    // Set Tx power level for station mode
    // Number between 0-15, as dB offset from max power - 0 will set max power
    ucPower = 0;
    // 设置发射功率
    lRetVal = sl_WlanSet(SL_WLAN_CFG_GENERAL_PARAM_ID, 
            WLAN_GENERAL_PARAM_OPT_STA_TX_POWER, 1, (unsigned char *)&ucPower);
    ASSERT_ON_ERROR(lRetVal);

    // Set PM policy to normal
    // 设置电源模式
    lRetVal = sl_WlanPolicySet(SL_POLICY_PM , SL_NORMAL_POLICY, NULL, 0);
    ASSERT_ON_ERROR(lRetVal);

    // Unregister mDNS services
    // 注销DNS服务
    lRetVal = sl_NetAppMDNSUnRegisterService(0, 0);
    ASSERT_ON_ERROR(lRetVal);

    // Remove  all 64 filters (8*8)
    memset(RxFilterIdMask.FilterIdMask, 0xFF, 8);
    // 删除接收滤波函数
    lRetVal = sl_WlanRxFilterSet(SL_REMOVE_RX_FILTER, (_u8 *)&RxFilterIdMask,
                       sizeof(_WlanRxFilterOperationCommandBuff_t));
    ASSERT_ON_ERROR(lRetVal);

    // 停止SimpleLink
    lRetVal = sl_Stop(SL_STOP_TIMEOUT);
    ASSERT_ON_ERROR(lRetVal);

    InitializeAppVariables();
    
    return lRetVal; // Success
}

/*******************************************************************************
 函数名：CheckLanConnection
 功  能: 检查LAN连接 调用sl_NetAppPingStart()实现对接入点的ping操作
 参  数：无
 返回值：0  SUCCESS
*******************************************************************************/
static long CheckLanConnection()
{
    SlPingStartCommand_t pingParams = {0};
    SlPingReport_t pingReport = {0};

    long lRetVal = -1;

    CLR_STATUS_BIT(g_ulStatus, STATUS_BIT_PING_DONE);
    g_ulPingPacketsRecv = 0;

    // Set the ping parameters
    pingParams.PingIntervalTime = PING_INTERVAL;
    pingParams.PingSize = PING_PKT_SIZE;
    pingParams.PingRequestTimeout = PING_TIMEOUT;
    pingParams.TotalNumberOfAttempts = NO_OF_ATTEMPTS;
    pingParams.Flags = 0;
    pingParams.Ip = g_ulGatewayIP;

    // Check for LAN connection
    lRetVal = sl_NetAppPingStart((SlPingStartCommand_t*)&pingParams, SL_AF_INET,
                            (SlPingReport_t*)&pingReport, SimpleLinkPingReport);
    ASSERT_ON_ERROR(lRetVal);

    // Wait for NetApp Event
    while(!IS_PING_DONE(g_ulStatus))
    {
        GPIO_IF_LedOff(MCU_ORANGE_LED_GPIO);
        MAP_UtilsDelay(800000);
        GPIO_IF_LedOn(MCU_ORANGE_LED_GPIO);
        MAP_UtilsDelay(800000);
        
#ifndef SL_PLATFORM_MULTI_THREADED
        _SlNonOsMainLoopTask(); 
#endif
    }

    if(0 == g_ulPingPacketsRecv)
    {
        //Problem with LAN connection
        ASSERT_ON_ERROR(LAN_CONNECTION_FAILED);
    }

    // LAN connection is successful
    return SUCCESS;
}


/*******************************************************************************
 函数名：CheckInternetConnection
 功  能: 通过ping外部主机（HOST_NAME）检查互联网连接
 参  数：无
 返回值：0 on success, negative error-code on error
*******************************************************************************/
static long CheckInternetConnection()
{
    SlPingStartCommand_t pingParams = {0};
    SlPingReport_t pingReport = {0};

    unsigned long ulIpAddr = 0;
    long lRetVal = -1;

    CLR_STATUS_BIT(g_ulStatus, STATUS_BIT_PING_DONE);
    g_ulPingPacketsRecv = 0;

    // Set the ping parameters
    pingParams.PingIntervalTime = PING_INTERVAL;
    pingParams.PingSize = PING_PKT_SIZE;
    pingParams.PingRequestTimeout = PING_TIMEOUT;
    pingParams.TotalNumberOfAttempts = NO_OF_ATTEMPTS;
    pingParams.Flags = 0;
    pingParams.Ip = g_ulGatewayIP;

    // Get external host IP address
    // 获取外部主机IP
    lRetVal = sl_NetAppDnsGetHostByName((signed char*)HOST_NAME, sizeof(HOST_NAME),
                                                &ulIpAddr, SL_AF_INET);
    ASSERT_ON_ERROR(lRetVal);

    // Replace the ping address to match HOST_NAME's IP address
    pingParams.Ip = ulIpAddr;

    // Try to ping HOST_NAME
    lRetVal = sl_NetAppPingStart((SlPingStartCommand_t*)&pingParams, SL_AF_INET,
                            (SlPingReport_t*)&pingReport, SimpleLinkPingReport);
    ASSERT_ON_ERROR(lRetVal);

    // Wait
    while(!IS_PING_DONE(g_ulStatus))
    { 
       GPIO_IF_LedOff(MCU_GREEN_LED_GPIO);
       MAP_UtilsDelay(800000);
       GPIO_IF_LedOn(MCU_GREEN_LED_GPIO);
       MAP_UtilsDelay(800000);
      // Wait for Ping Event 
#ifndef SL_PLATFORM_MULTI_THREADED
        _SlNonOsMainLoopTask(); 
#endif
    }

    if (0 == g_ulPingPacketsRecv)
    {
        // Problem with internet connection
        ASSERT_ON_ERROR(INTERNET_CONNECTION_FAILED);
    }

    // Internet connection is successful
    return SUCCESS;
}


/*******************************************************************************
 函数名：WlanConnect
 功  能: 通过安全参数连接到AP(SSID_NAME)
 参  数：无
 返回值：0 on success, negative error-code on error
*******************************************************************************/
static long WlanConnect()
{
    SlSecParams_t secParams = {0};
    long lRetVal = 0;

    secParams.Key = (signed char*)SECURITY_KEY;
    secParams.KeyLen = strlen(SECURITY_KEY);
    secParams.Type = SECURITY_TYPE;

    // sl_WlanConnect运行时会调用WLAN事件和NETAPP事件处理程序（异步运行）
    lRetVal = sl_WlanConnect((signed char*)SSID_NAME, strlen(SSID_NAME), 0, &secParams, 0);
    ASSERT_ON_ERROR(lRetVal);

    // 等待WLAN事件 如果一直没连就闪灯
    while((!IS_CONNECTED(g_ulStatus)) || (!IS_IP_ACQUIRED(g_ulStatus))) 
    { 
        // 红灯闪烁 收到WLAN事件后红灯常亮
        GPIO_IF_LedOff(MCU_RED_LED_GPIO);
        MAP_UtilsDelay(800000);
        GPIO_IF_LedOn(MCU_RED_LED_GPIO);
        MAP_UtilsDelay(800000);
    }

    return SUCCESS;
    
    
    // prosivisioning_ap的程序 需要消化理解
  
  
  
//        long lRetVal = -1;
////	unsigned int uiConnectTimeoutCnt =0;
//	
//    //Add Profile
////	lRetVal = sl_WlanProfileAdd(g_cWlanSSID, strlen((char*)g_cWlanSSID), 0, &g_SecParams,
////                        0,g_ucPriority,0);
////	ASSERT_ON_ERROR(lRetVal);
//
//    //Connecting to the Access point
//	lRetVal = sl_WlanConnect(g_cWlanSSID, strlen((char*)g_cWlanSSID), 0, &g_SecParams, 0);
//	ASSERT_ON_ERROR(lRetVal);
//
//    // Wait for WLAN Event
//        while((!IS_CONNECTED(g_ulStatus)) || (!IS_IP_ACQUIRED(g_ulStatus))) 
//    { 
//        // Toggle LEDs to Indicate Connection Progress
//        GPIO_IF_LedOff(MCU_IP_ALLOC_IND);
//        MAP_UtilsDelay(800000);
//        GPIO_IF_LedOn(MCU_IP_ALLOC_IND);
//        MAP_UtilsDelay(800000);
//    }
//	
//	return SUCCESS;

   
}

//****************************************************************************
//
//! \brief Start simplelink, connect to the ap and run the ping test
//!
//! This function starts the simplelink, connect to the ap and start the ping
//! test on the default gateway for the ap
//!
//! \param[in]  pvParameters - Pointer to the list of parameters that 
//!             can bepassed to the task while creating it
//!
//! \return  None
//
//****************************************************************************
/*******************************************************************************
 函数名：WlanStationMode
 功  能: STA模式函数 网络核心函数
 参  数：pvParameters
 返回值：无
*******************************************************************************/
void WlanStationMode()
//void WlanStationMode( void *pvParameters )
{

    long lRetVal = -1;
    InitializeAppVariables();    // 初始化程序变量

    lRetVal = ConfigureSimpleLinkToDefaultState();    // 配置SimpleLink为默认状态
    if(lRetVal < 0)
    {
      // 若没配置为默认状态 并且设备没在STA模式
        if (DEVICE_NOT_IN_STATION_MODE == lRetVal)
        {
          // 向串口打印“设备配置为默认状态失败”
            UART_PRINT("Failed to configure the device in its default state\n\r");
        }

        LOOP_FOREVER();
    }

    // 向串口打印“设备已经配置为默认状态”
    UART_PRINT("Device is configured in default state \n\r");

    // 假定设备已经为STA模式 启动SimpleLink
    lRetVal = sl_Start(0, 0, 0);
    if (lRetVal < 0 || ROLE_STA != lRetVal)
    {
      // 向串口打印“设备启动失败”
        UART_PRINT("Failed to start the device \n\r");
        LOOP_FOREVER();
    }
    
    // 向串口打印“设备已经处于STA模式”
    UART_PRINT("Device started as STATION \n\r");

    // 连接WLAN无线局域网 即某一个AP热点 在测试过程中我用的手机AP热点
    lRetVal = WlanConnect();
    if(lRetVal < 0)
    {
      // 向串口打印“设备建立连接失败”
        UART_PRINT("Failed to establish connection of an AP \n\r");
        LOOP_FOREVER();
    }

    // 向串口打印“建立AP连接且IP已获得” 红灯亮
    UART_PRINT("Connection established AP and IP is aquired \n\r");
    UART_PRINT("Pinging...! \n\r");

    // 检查AP模式的LAN局域网连接
    lRetVal = CheckLanConnection();
    if(lRetVal < 0)
    {
      // 向串口打印“设备无法ping通网关”
        UART_PRINT("Device couldn't ping the gateway \n\r");
        LOOP_FOREVER();
    }

    // 获得网关响应后表示连接成功 橘灯亮
    GPIO_IF_LedOn(MCU_GREEN_LED_GPIO);

    // 检查互联网连接 看看是否可以联网对外访问某一网页 可以和外部主机通信后 绿灯亮
    lRetVal = CheckInternetConnection();
    if(lRetVal < 0)
    {
       // 向串口打印“设备无法ping通外部主机”
        UART_PRINT("Device couldn't ping the external host \n\r");
        LOOP_FOREVER();
    }

    // 向串口打印“设备可以ping通网关和外部主机”
    UART_PRINT("Device pinged both the gateway and the external host \n\r");

    // 向串口打印“设备STA模式执行成功"
    UART_PRINT("WLAN STATION executed successfully \n\r");

    //
    // power off the network processor
    //
//    lRetVal = sl_Stop(SL_STOP_TIMEOUT);    // 停止SimpleLink
    
    
    // TCP部分
//    TcpApp();
    
    // UDP部分
//    UdpApp();
    
//      osi_TaskCreate( UdpLedStatus, \
//                                (const signed char*)"UDP LED Task", \
//                                OSI_STACK_SIZE, NULL, 1, NULL );
//      
//      osi_TaskCreate( UdpTmpStatus, \
//                                (const signed char*)"UDP Temperature Task", \
//                                OSI_STACK_SIZE, NULL, 1, NULL );
//            
//      osi_TaskCreate( UdpHumStatus, \
//                                (const signed char*)"UDP Humidity Task", \
//                                OSI_STACK_SIZE, NULL, 1, NULL );
    
    UdpData();
      
      

//    LOOP_FOREVER();
    
}


/*******************************************************************************
 函数名：ConfigureMode
 功  能: 设置接入点模式
 参  数：iMode
 返回值：无
*******************************************************************************/
static int ConfigureMode(int iMode)
{
  // 设备不是接入点模式ROLE_AP 需要调用该函数设置接入点模式
  
//    char    pcSsidName[33];
//    long   lRetVal = -1;
//
//    UART_PRINT("Enter the AP SSID name: ");
//    GetSsidName(pcSsidName,33);    // 获取接入点名称
//
//    lRetVal = sl_WlanSetMode(ROLE_AP);   // 设置WLAN模式
//    ASSERT_ON_ERROR(lRetVal);
//
//    // 设置接入点名称
//    lRetVal = sl_WlanSet(SL_WLAN_CFG_AP_ID, WLAN_AP_OPT_SSID, strlen(pcSsidName),
//                            (unsigned char*)pcSsidName);
//    ASSERT_ON_ERROR(lRetVal);
//
//    UART_PRINT("Device is configured in AP mode\n\r");
//
//    /* Restart Network processor */
//    // 对网络处理器进行重启 先关闭再打开
//    lRetVal = sl_Stop(SL_STOP_TIMEOUT);
//
//    // reset status bits
//    CLR_STATUS_BIT_ALL(g_ulStatus);
//
//    return sl_Start(NULL,NULL,NULL);
    
    // prosivisioning_ap代码 需要消化理解
   long   lRetVal = -1;

    lRetVal = sl_WlanSetMode(iMode);    // 设置WLAN模式
    ASSERT_ON_ERROR(lRetVal);

    /* Restart Network processor */
    // 重启网络处理器
    lRetVal = sl_Stop(SL_STOP_TIMEOUT);
    ASSERT_ON_ERROR(lRetVal);

    // reset status bits
    CLR_STATUS_BIT_ALL(g_ulStatus);

    return sl_Start(NULL,NULL,NULL);
}


/*******************************************************************************
 函数名：PingTest
 功  能: Ping测试函数
 参  数：ulIpAddr
 返回值：无
*******************************************************************************/
//static int PingTest(unsigned long ulIpAddr)
//{  
//    signed long           lRetVal = -1;
//    SlPingStartCommand_t PingParams;
//    SlPingReport_t PingReport;
//    PingParams.PingIntervalTime = PING_INTERVAL;
//    PingParams.PingSize = PING_PKT_SIZE;
//    PingParams.PingRequestTimeout = PING_TIMEOUT;
//    PingParams.TotalNumberOfAttempts = NO_OF_ATTEMPTS;
//    PingParams.Flags = PING_FLAG;
//    PingParams.Ip = ulIpAddr; /* Cleint's ip address */
//    
//    UART_PRINT("Running Ping Test...\n\r");
//    /* Check for LAN connection */
//    lRetVal = sl_NetAppPingStart((SlPingStartCommand_t*)&PingParams, SL_AF_INET,
//                            (SlPingReport_t*)&PingReport, NULL);
//    ASSERT_ON_ERROR(lRetVal);
//
//    g_ulPingPacketsRecv = PingReport.PacketsReceived;
//
//    if (g_ulPingPacketsRecv > 0 && g_ulPingPacketsRecv <= NO_OF_ATTEMPTS)
//    {
//      // LAN connection is successful
//      UART_PRINT("Ping Test successful\n\r");
//    }
//    else
//    {
//        // Problem with LAN connection
//        ASSERT_ON_ERROR(LAN_CONNECTION_FAILED);
//    }
//
//    return SUCCESS;
//}

/*******************************************************************************
 函数名：WlanAPMode
 功  能: AP模式
 参  数：pvParameters
 返回值：无
*******************************************************************************/
//void WlanAPMode( void *pvParameters )
//{   
//    int iTestResult = 0;
//    unsigned char ucDHCP;
//    long lRetVal = -1;
//
//    InitializeAppVariables();    // 初始化程序变量
//
//    //
//    // Following function configure the device to default state by cleaning
//    // the persistent settings stored in NVMEM (viz. connection profiles &
//    // policies, power policy etc)
//    //
//    // Applications may choose to skip this step if the developer is sure
//    // that the device is in its default state at start of applicaton
//    //
//    // Note that all profiles and persistent settings that were done on the
//    // device will be lost
//    //
//    lRetVal = ConfigureSimpleLinkToDefaultState();    // 设置SimpleLink为默认状态
//    if(lRetVal < 0)
//    {
//        if (DEVICE_NOT_IN_STATION_MODE == lRetVal)
//            UART_PRINT("Failed to configure the device in its default state \n\r");
//
//        LOOP_FOREVER();
//    }
//
//    UART_PRINT("Device is configured in default state \n\r");
//
//    //
//    // Asumption is that the device is configured in station mode already
//    // and it is in its default state
//    //
//    lRetVal = sl_Start(NULL,NULL,NULL);    // 启动SimpleLink
//
//    if (lRetVal < 0)
//    {
//        UART_PRINT("Failed to start the device \n\r");
//        LOOP_FOREVER();
//    }
//
//    UART_PRINT("Device started as STATION \n\r");
//    
//    //
//    // Configure the networking mode and ssid name(for AP mode)
//    //
//    if(lRetVal != ROLE_AP)
//    {
//        if(ConfigureMode(lRetVal) != ROLE_AP)
//        {
//            UART_PRINT("Unable to set AP mode, exiting Application...\n\r");
//            sl_Stop(SL_STOP_TIMEOUT);
//            LOOP_FOREVER();
//        }
//    }
//
//    while(!IS_IP_ACQUIRED(g_ulStatus))
//    {
//      //looping till ip is acquired
//    }
//
//    unsigned char len = sizeof(SlNetCfgIpV4Args_t);
//    SlNetCfgIpV4Args_t ipV4 = {0};
//
//    // get network configuration
//    lRetVal = sl_NetCfgGet(SL_IPV4_AP_P2P_GO_GET_INFO,&ucDHCP,&len,
//                            (unsigned char *)&ipV4);
//    if (lRetVal < 0)
//    {
//        UART_PRINT("Failed to get network configuration \n\r");
//        LOOP_FOREVER();
//    }
//    
//    UART_PRINT("Connect a client to Device\n\r");
//    while(!IS_IP_LEASED(g_ulStatus))
//    {
//      //wating for the client to connect
//    }
//    UART_PRINT("Client is connected to Device\n\r");
//
//    iTestResult = PingTest(g_ulStaIp);
//    if(iTestResult < 0)
//    {
//        UART_PRINT("Ping to client failed \n\r");
//    }
//
//    UNUSED(ucDHCP);
//    UNUSED(iTestResult);
//
//    // revert to STA mode
//    lRetVal = sl_WlanSetMode(ROLE_STA);
//    if(lRetVal < 0)
//    {
//      ERR_PRINT(lRetVal);
//      LOOP_FOREVER();
//    }
//
//    // Switch off Network processor
//    lRetVal = sl_Stop(SL_STOP_TIMEOUT);
//    UART_PRINT("WLAN AP example executed successfully");
//    while(1);
//}

//*****************************************************************************
//
//! \brief This function Get the Scan Result
//!
//! \param[out]      netEntries - Scan Result 
//!
//! \return         Success - Size of Scan Result Array
//!                    Failure   - -1 
//!
//! \note
//!
//
//*****************************************************************************
//static int GetScanResult(Sl_WlanNetworkEntry_t* netEntries )
//{
//    unsigned char   policyOpt;
//    unsigned long IntervalVal = 60;
//    int lRetVal;
//
//    policyOpt = SL_CONNECTION_POLICY(0, 0, 0, 0, 0);
//    lRetVal = sl_WlanPolicySet(SL_POLICY_CONNECTION , policyOpt, NULL, 0);
//	ASSERT_ON_ERROR(lRetVal);
//
//
//    // enable scan
//    policyOpt = SL_SCAN_POLICY(1);
//
//    // set scan policy - this starts the scan
//    lRetVal = sl_WlanPolicySet(SL_POLICY_SCAN , policyOpt,
//                            (unsigned char *)(IntervalVal), sizeof(IntervalVal));
//	ASSERT_ON_ERROR(lRetVal);
//		
//	
//    // delay 1 second to verify scan is started
//    osi_Sleep(1000);
//
//    // lRetVal indicates the valid number of entries
//    // The scan results are occupied in netEntries[]
//    lRetVal = sl_WlanGetNetworkList(0, SCAN_TABLE_SIZE, netEntries);
//	ASSERT_ON_ERROR(lRetVal);
//
//    // Disable scan
//    policyOpt = SL_SCAN_POLICY(0);
//
//    // set scan policy - this stops the scan
//    sl_WlanPolicySet(SL_POLICY_SCAN , policyOpt,
//                            (unsigned char *)(IntervalVal), sizeof(IntervalVal));
//	ASSERT_ON_ERROR(lRetVal);	
//
//    return lRetVal;
//
//}


/*******************************************************************************
 函数名：ProvisioningAP
 功  能: AP模式开启HTTP服务器 通过网页来配置Wi-Fi信息 配置成功后转换为STA模式
 参  数：*pTaskParams
 返回值：无
*******************************************************************************/
void ProvisioningAP(void* pTaskParams)
{
//   long lCountSSID;
   long lRetVal = -1;

    InitializeAppVariables();    // 初始化应用变量

    //
    // Following function configure the device to default state by cleaning
    // the persistent settings stored in NVMEM (viz. connection profiles &
    // policies, power policy etc)
    //
    // Applications may choose to skip this step if the developer is sure
    // that the device is in its default state at start of applicaton
    //
    // Note that all profiles and persistent settings that were done on the
    // device will be lost
    //
   lRetVal = ConfigureSimpleLinkToDefaultState();    // 配置成初始状态
   if(lRetVal < 0)
   {
       if (DEVICE_NOT_IN_STATION_MODE == lRetVal)
       {
         // 配置设备至默认状态失败
           UART_PRINT("Failed to configure the device in its default state\n\r");
       }

       LOOP_FOREVER();
   }

   // 设备已配置为默认状态
    UART_PRINT("Device is configured in default state \n\r");

	//
	// Assumption is that the device is configured in station mode already
	// and it is in its default state
	//
	lRetVal = sl_Start(0, 0, 0);    // 启动SimpleLink
	if (lRetVal < 0 || ROLE_STA != lRetVal)
	{
          // 开启失败
	   UART_PRINT("Failed to start the device \n\r");
	   LOOP_FOREVER();
	}
        
        // 设备STA模式开启
	UART_PRINT("Device started as STATION \n\r");
    
    
  
    while(!g_ucProvisioningDone)
    {   
      
      // 这部分其实可以删减？
        //Get Scan Result
//        lCountSSID = GetScanResult(&g_NetEntries[0]);
//        if(lCountSSID < 0)
//        {
//            ERR_PRINT(lRetVal);
//            LOOP_FOREVER();
//        }	
        
        //
        // Configure to AP Mode
        // 
        if(ConfigureMode(ROLE_AP) !=ROLE_AP)
        {
          // 无法设置为AP模式
            UART_PRINT("Unable to set AP mode...\n\r");
            lRetVal = sl_Stop(SL_STOP_TIMEOUT);
            CLR_STATUS_BIT_ALL(g_ulStatus);
            LOOP_FOREVER();
        }

        while(!IS_IP_ACQUIRED(g_ulStatus))
        {
          //looping till ip is acquired
          // 循环直至IP获取
        }

        //Stop Internal HTTP Server
        // 停止内部HTTP服务器
        lRetVal = sl_NetAppStop(SL_NET_APP_HTTP_SERVER_ID);
        if(lRetVal < 0)
        {
            ERR_PRINT(lRetVal);
            LOOP_FOREVER();
        }

        //Start Internal HTTP Server
        // 开启内部HTTP服务器
        lRetVal = sl_NetAppStart(SL_NET_APP_HTTP_SERVER_ID);
        if(lRetVal < 0)
        {
            ERR_PRINT(lRetVal);
            LOOP_FOREVER();
        }

	 //
	// Wait for AP Configuraiton, Open Browser and Configure AP
	//
        // 等待AP配置Wi-Fi信息
	while(g_ucProfileAdded && !g_ucProvisioningDone)
	{
		MAP_UtilsDelay(100);
	}

	g_ucProfileAdded = 1;    // 配置添加成功

        
        // 这部分应该转成STA了
        
        //
        // Configure to STA Mode
        // 
        // 设置为STA模式
        if(ConfigureMode(ROLE_STA) !=ROLE_STA)
        {
          // 无法配置为STA模式
            UART_PRINT("Unable to set STA mode...\n\r");
            lRetVal = sl_Stop(SL_STOP_TIMEOUT);
            CLR_STATUS_BIT_ALL(g_ulStatus);
            LOOP_FOREVER();
        }
        
        
           
        lRetVal = WlanConnect();    // 连接WLAN无线局域网
    if(lRetVal < 0)
    {
      // 向串口打印“设备建立连接失败”
        UART_PRINT("Failed to establish connection of an AP \n\r");
        LOOP_FOREVER();
    }

    // 向串口打印“建立AP连接且IP已获得”
    UART_PRINT("Connection established w/ AP and IP is aquired \n\r");
    UART_PRINT("Pinging...! \n\r");

    //
    // Checking the Lan connection by pinging to AP gateway
    //
    lRetVal = CheckLanConnection();    // 检查LAN局域网连接
    if(lRetVal < 0)
    {
      // 向串口打印“设备无法ping通网关”
        UART_PRINT("Device couldn't ping the gateway \n\r");
        LOOP_FOREVER();
    }
    
    // 获得网关响应后表示连接成功 绿灯亮
    GPIO_IF_LedOn(MCU_GREEN_LED_GPIO);
    }
        //
        // Connect to the Configured Access Point
        //
//        lRetVal = WlanConnect();
//            if(lRetVal < 0)
//            {
//                    ERR_PRINT(lRetVal);
//                    LOOP_FOREVER();
//            }	
//
//        g_ucConnectedToConfAP = IS_CONNECTED(g_ulStatus);
//
//        
//        // 这里是最关键的语句 需要消化 我觉得不需要断开连接 直接照着这个进行网络传输就行了
//        lRetVal = sl_WlanDisconnect();    // 断开WLAN连接
//        if(0 == lRetVal)
//        {
//            // Wait
//            while(IS_CONNECTED(g_ulStatus))
//            {
//#ifndef SL_PLATFORM_MULTI_THREADED
//                _SlNonOsMainLoopTask(); 
//#endif
//            }
//        }
//        CLR_STATUS_BIT_ALL(g_ulStatus);
//    }
//   
//    while(1)
//    {
//
//    }
     

}

//****************************************************************************
//
//!    \brief Parse the input IP address from the user
//!
//!    \param[in]                     ucCMD (char pointer to input string)
//!
//!    \return                        0 : if correct IP, -1 : incorrect IP
//
//****************************************************************************
//int IpAddressParser(char *ucCMD)
//{
//    volatile int i=0;
//    unsigned int uiUserInputData;
//    unsigned long ulUserIpAddress = 0;
//    char *ucInpString;
//    ucInpString = strtok(ucCMD, ".");
//    uiUserInputData = (int)strtoul(ucInpString,0,10);
//    while(i<4)
//    {
//        //
//       // Check Whether IP is valid
//       //
//       if((ucInpString != NULL) && (uiUserInputData < 256))
//       {
//           ulUserIpAddress |= uiUserInputData;
//           if(i < 3)
//               ulUserIpAddress = ulUserIpAddress << 8;
//           ucInpString=strtok(NULL,".");
//           uiUserInputData = (int)strtoul(ucInpString,0,10);
//           i++;
//       }
//       else
//       {
//           return -1;
//       }
//    }
//    g_ulDestinationIp = ulUserIpAddress;
//    return SUCCESS;
//}



