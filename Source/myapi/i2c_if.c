/***************************************************************************
* File         : i2c_if.c
* Summary      : 
* Version      : v0.1
* Author       : veahow
* Date         : 2019/4/17
* Change Logs  : 
***************************************************************************/

#include "global.h"

//*****************************************************************************
//                      宏定义
//*****************************************************************************
#define I2C_BASE                I2CA0_BASE
#define SYS_CLK                 80000000
#define RETERR_IF_TRUE(condition) {if(condition) return FAILURE;}
#define RET_IF_ERR(Func)          {int iRetVal = (Func); \
                                   if (SUCCESS != iRetVal) \
                                     return  iRetVal;}

//****************************************************************************
//                      局部函数定义                 
//****************************************************************************
static int I2CTransact(unsigned long ulCmd);


//****************************************************************************
//
//! Invokes the transaction over I2C
//!
//! \param ulCmd is the command to be executed over I2C
//! 
//! This function works in a polling mode,
//!    1. Initiates the transfer of the command.
//!    2. Waits for the I2C transaction completion
//!    3. Check for any error in transaction
//!    4. Clears the master interrupt
//!
//! \return 0: Success, < 0: Failure.
//
//****************************************************************************
/*******************************************************************************
 函数名：I2CTransact
 功  能: I2C处理 
 参  数：ulCmd 处理命令
 返回值：0 SUCCESS | <0 Failed
*******************************************************************************/
static int 
I2CTransact(unsigned long ulCmd)
{
    MAP_I2CMasterIntClearEx(I2C_BASE,MAP_I2CMasterIntStatusEx(I2C_BASE,false));    // 清除I2C中断标志

    MAP_I2CMasterTimeoutSet(I2C_BASE, I2C_TIMEOUT_VAL);    // 设置I2C超时
    //
    // Initiate the transfer.
    //
    MAP_I2CMasterControl(I2C_BASE, ulCmd);    // 控制I2C状态
    //
    // Wait until the current byte has been transferred.
    // Poll on the raw interrupt status.
    //
    while((MAP_I2CMasterIntStatusEx(I2C_BASE, false) 
                & (I2C_INT_MASTER | I2C_MRIS_CLKTOUT)) == 0)
    {
    }
    //
    // Check for any errors in transfer
    //
    if(MAP_I2CMasterErr(I2C_BASE) != I2C_MASTER_ERR_NONE)
    {
        switch(ulCmd)
        {
        case I2C_MASTER_CMD_BURST_SEND_START:    // 突发发送开始
        case I2C_MASTER_CMD_BURST_SEND_CONT:    // 突发发送继续
        case I2C_MASTER_CMD_BURST_SEND_STOP:    // 突发发送停止
            MAP_I2CMasterControl(I2C_BASE,
                         I2C_MASTER_CMD_BURST_SEND_ERROR_STOP);
            break;
        case I2C_MASTER_CMD_BURST_RECEIVE_START:    // 突发接收开始
        case I2C_MASTER_CMD_BURST_RECEIVE_CONT:    // 突发接收继续
        case I2C_MASTER_CMD_BURST_RECEIVE_FINISH:    // 突发接收完成
            MAP_I2CMasterControl(I2C_BASE,
                         I2C_MASTER_CMD_BURST_RECEIVE_ERROR_STOP);
            break;
        default:
            break;
        }
        return FAILURE;
    }

    return SUCCESS;
}

/*******************************************************************************
 函数名：I2C_IF_Write
 功  能: I2C写 工作在轮询模式 写设备寄存器地址 通过I2C写所有字节
 参  数：ucDevAddr 器件地址 7位I2C从地址 TMP006-0x41 BMA222-0x18 SHT2x-0x40
         *pucData 写数据指针
         ucLen 写数据长度
         ucStop 停止位数 0-无停止位 1-有停止位
 返回值：0 SUCCESS | <0 Failed
*******************************************************************************/
int 
I2C_IF_Write(unsigned char ucDevAddr,
         unsigned char *pucData,
         unsigned char ucLen, 
         unsigned char ucStop)
{
  
    RETERR_IF_TRUE(pucData == NULL);
    RETERR_IF_TRUE(ucLen == 0);

    MAP_I2CMasterSlaveAddrSet(I2C_BASE, ucDevAddr, false);    // 设置从地址

    MAP_I2CMasterDataPut(I2C_BASE, *pucData);    // I2C发送数据 写第一个字节给控制器
    //
    // Initiate the transfer.
    //
    RET_IF_ERR(I2CTransact(I2C_MASTER_CMD_BURST_SEND_START));
    //
    // Decrement the count and increment the data pointer
    // to facilitate the next transfer
    //
    ucLen--;
    pucData++;
    //
    // Loop until the completion of transfer or error
    //
    while(ucLen)
    {
        //
        // Write the next byte of data
        //
        MAP_I2CMasterDataPut(I2C_BASE, *pucData);
        //
        // Transact over I2C to send byte
        //
        RET_IF_ERR(I2CTransact(I2C_MASTER_CMD_BURST_SEND_CONT));
        //
        // Decrement the ·       and increment the data pointer
        // to facilitate the next transfer
        //
        ucLen--;
        pucData++;
    }
    //
    // If stop bit is to be sent, send it.
    //
    if(ucStop == true)
    {
        RET_IF_ERR(I2CTransact(I2C_MASTER_CMD_BURST_SEND_STOP));
    }

    return SUCCESS;
}

//****************************************************************************
//
//! Invokes the I2C driver APIs to read from the device. This assumes the 
//! device local address to read from is set using the I2CWrite API.
//!
//! \param ucDevAddr is the 7-bit I2C slave address
//! \param pucData is the pointer to the read data to be placed
//! \param ucLen is the length of data to be read
//! 
//! This function works in a polling mode,
//!    1. Writes the device register address to be written to.
//!    2. In a loop, reads all the bytes over I2C
//!
//! \return 0: Success, < 0: Failure.
//
//****************************************************************************
/*******************************************************************************
 函数名：I2C_IF_Read
 功  能: I2C读
 参  数：ucDevAddr 器件地址 TMP006-0x41 BMA222-0x18 SHT2x-0x40
         *pucData 写数据指针
         ucLen 写数据长度
 返回值：0 SUCCESS | <0 Failed
*******************************************************************************/
int 
I2C_IF_Read(unsigned char ucDevAddr,
        unsigned char *pucData,
        unsigned char ucLen)
{
    unsigned long ulCmdID;

    RETERR_IF_TRUE(pucData == NULL);
    RETERR_IF_TRUE(ucLen == 0);

    // 设置I2C从地址编码
    MAP_I2CMasterSlaveAddrSet(I2C_BASE, ucDevAddr, true);
    //
    // Check if its a single receive or burst receive
    //
    if(ucLen == 1)
    {
        //
        // Configure for a single receive
        //
        ulCmdID = I2C_MASTER_CMD_SINGLE_RECEIVE;
    }
    else
    {
        //
        // Initiate a burst receive sequence
        //
        ulCmdID = I2C_MASTER_CMD_BURST_RECEIVE_START;
    }
    //
    // Initiate the transfer.
    //
    RET_IF_ERR(I2CTransact(ulCmdID));
    //
    // Decrement the count and increment the data pointer
    // to facilitate the next transfer
    //
    ucLen--;
    //
    // Loop until the completion of reception or error
    //
    while(ucLen)
    {
        //
        // Receive the byte over I2C
        //
        *pucData = MAP_I2CMasterDataGet(I2C_BASE);    // I2C接收数据
        //
        // Decrement the count and increment the data pointer
        // to facilitate the next transfer
        //
        ucLen--;
        pucData++;
        if(ucLen)
        {
            //
            // Continue the reception
            //
            RET_IF_ERR(I2CTransact(I2C_MASTER_CMD_BURST_RECEIVE_CONT));
        }
        else
        {
            //
            // Complete the last reception
            //
            RET_IF_ERR(I2CTransact(I2C_MASTER_CMD_BURST_RECEIVE_FINISH));
        }
    }
    //
    // Receive the byte over I2C
    //
    *pucData = MAP_I2CMasterDataGet(I2C_BASE);

    return SUCCESS;
}

//****************************************************************************
//
//! Invokes the I2C driver APIs to read from a specified address the device. 
//! This assumes the device local address to be of 8-bit. For other 
//! combinations use I2CWrite followed by I2CRead.
//!
//! \param ucDevAddr is the 7-bit I2C slave address
//! \param pucWrDataBuf is the pointer to the data to be written (reg addr)
//! \param ucWrLen is the length of data to be written
//! \param pucRdDataBuf is the pointer to the read data to be placed
//! \param ucRdLen is the length of data to be read
//! 
//! This function works in a polling mode,
//!    1. Writes the data over I2C (device register address to be read from).
//!    2. In a loop, reads all the bytes over I2C
//!
//! \return 0: Success, < 0: Failure.
//
//****************************************************************************
int 
I2C_IF_ReadFrom(unsigned char ucDevAddr,
            unsigned char *pucWrDataBuf,
            unsigned char ucWrLen,
            unsigned char *pucRdDataBuf,
            unsigned char ucRdLen)
{
    //
    // Write the register address to be read from.
    // Stop bit implicitly assumed to be 0.
    //
    RET_IF_ERR(I2C_IF_Write(ucDevAddr,pucWrDataBuf,ucWrLen,0));
    //
    // Read the specified length of data
    //
    RET_IF_ERR(I2C_IF_Read(ucDevAddr, pucRdDataBuf, ucRdLen));

    return SUCCESS;
}

/*******************************************************************************
 函数名：I2C_IF_Open
 功  能: I2C打开
 参  数：ulMode I2C配置方式 
         I2C_MASTER_MODE_STD 标准方式 0 | I2C_MASTER_MODE_FST 快速方式 1
 返回值：0 SUCCESS | <0 Failed
*******************************************************************************/
int 
I2C_IF_Open(unsigned long ulMode)
{
    //MAP_HwSemaphoreLock(HWSEM_I2C, HWSEM_WAIT_FOR_EVER);
    MAP_PRCMPeripheralClkEnable(PRCM_I2CA0, PRCM_RUN_MODE_CLK);    // 允许外设时钟
    MAP_PRCMPeripheralReset(PRCM_I2CA0);    // 复位外设

    // 将i2c配置为特定模式
    switch(ulMode)
    {
        case I2C_MASTER_MODE_STD:       /* 100000 100kHz */
            MAP_I2CMasterInitExpClk(I2C_BASE,SYS_CLK,false);    // 配置I2C接口
            break;

        case I2C_MASTER_MODE_FST:       /* 400000 400kHz */
            MAP_I2CMasterInitExpClk(I2C_BASE,SYS_CLK,true);
            break;

        default:
            MAP_I2CMasterInitExpClk(I2C_BASE,SYS_CLK,true);
            break;
    }
    
    //
    // Disables the multi-master mode
    //
    //MAP_I2CMasterDisable(I2C_BASE);
    
    return SUCCESS;
}

/*******************************************************************************
 函数名：I2C_IF_Close
 功  能: I2C关闭 函数工作在轮询模式
 参  数：无
 返回值：0 SUCCESS | <0 Failed
*******************************************************************************/
int 
I2C_IF_Close()
{
    // 关闭I2C外设
    MAP_PRCMPeripheralClkDisable(PRCM_I2CA0, PRCM_RUN_MODE_CLK);

    return SUCCESS;
}