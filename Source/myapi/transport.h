/***************************************************************************
* File         : transport.h
* Summary      : 
* Version      : v0.1
* Author       : veahow
* Date         : 2019/4/17
* Change Logs  : 
***************************************************************************/
#ifndef __TRANSPORT_H_
#define __TRANSPORT_H_

//*****************************************************************************
//
// If building with a C++ compiler, make all of the definitions in this header
// have a C binding.
//
//*****************************************************************************

#ifdef __cplusplus
extern "C"
{
#endif
  
extern char dataBuf[BUF_SIZE];
  
extern int TcpClientCreate(unsigned short usPort);
extern int TcpClientConnect();
extern int TcpClientSend(char str[]);
extern void TcpClientClose();
extern void TcpApp();

extern int UdpClientCreate(unsigned short usPort, int *udp_sock, SlSockAddrIn_t *addr, SlSockAddrIn_t *des_addr);
extern void UdpClientSend(char str[], int udp_sock, SlSockAddrIn_t *des_addr);
extern void UdpClientRecv(char str[], int udp_sock, SlSockAddrIn_t *addr);
extern void UdpClientClose(int udp_sock);
extern void UdpApp();
extern void UdpLedStatus();
extern void UdpTmpStatus();
extern void UdpHumStatus();
extern void UdpPwmStatus();
extern void UdpData();
extern void UdpOperation();


//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif    // __TRANSPORT_H_