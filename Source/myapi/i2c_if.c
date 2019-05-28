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
//                      �궨��
//*****************************************************************************
#define I2C_BASE                I2CA0_BASE
#define SYS_CLK                 80000000
#define RETERR_IF_TRUE(condition) {if(condition) return FAILURE;}
#define RET_IF_ERR(Func)          {int iRetVal = (Func); \
                                   if (SUCCESS != iRetVal) \
                                     return  iRetVal;}

//****************************************************************************
//                      �ֲ���������                 
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
 ��������I2CTransact
 ��  ��: I2C���� 
 ��  ����ulCmd ��������
 ����ֵ��0 SUCCESS | <0 Failed
*******************************************************************************/
static int 
I2CTransact(unsigned long ulCmd)
{
    MAP_I2CMasterIntClearEx(I2C_BASE,MAP_I2CMasterIntStatusEx(I2C_BASE,false));    // ���I2C�жϱ�־

    MAP_I2CMasterTimeoutSet(I2C_BASE, I2C_TIMEOUT_VAL);    // ����I2C��ʱ
    //
    // Initiate the transfer.
    //
    MAP_I2CMasterControl(I2C_BASE, ulCmd);    // ����I2C״̬
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
        case I2C_MASTER_CMD_BURST_SEND_START:    // ͻ�����Ϳ�ʼ
        case I2C_MASTER_CMD_BURST_SEND_CONT:    // ͻ�����ͼ���
        case I2C_MASTER_CMD_BURST_SEND_STOP:    // ͻ������ֹͣ
            MAP_I2CMasterControl(I2C_BASE,
                         I2C_MASTER_CMD_BURST_SEND_ERROR_STOP);
            break;
        case I2C_MASTER_CMD_BURST_RECEIVE_START:    // ͻ�����տ�ʼ
        case I2C_MASTER_CMD_BURST_RECEIVE_CONT:    // ͻ�����ռ���
        case I2C_MASTER_CMD_BURST_RECEIVE_FINISH:    // ͻ���������
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
 ��������I2C_IF_Write
 ��  ��: I2Cд ��������ѯģʽ д�豸�Ĵ�����ַ ͨ��I2Cд�����ֽ�
 ��  ����ucDevAddr ������ַ 7λI2C�ӵ�ַ TMP006-0x41 BMA222-0x18 SHT2x-0x40
         *pucData д����ָ��
         ucLen д���ݳ���
         ucStop ֹͣλ�� 0-��ֹͣλ 1-��ֹͣλ
 ����ֵ��0 SUCCESS | <0 Failed
*******************************************************************************/
int 
I2C_IF_Write(unsigned char ucDevAddr,
         unsigned char *pucData,
         unsigned char ucLen, 
         unsigned char ucStop)
{
  
    RETERR_IF_TRUE(pucData == NULL);
    RETERR_IF_TRUE(ucLen == 0);

    MAP_I2CMasterSlaveAddrSet(I2C_BASE, ucDevAddr, false);    // ���ôӵ�ַ

    MAP_I2CMasterDataPut(I2C_BASE, *pucData);    // I2C�������� д��һ���ֽڸ�������
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
        // Decrement the ��       and increment the data pointer
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
 ��������I2C_IF_Read
 ��  ��: I2C��
 ��  ����ucDevAddr ������ַ TMP006-0x41 BMA222-0x18 SHT2x-0x40
         *pucData д����ָ��
         ucLen д���ݳ���
 ����ֵ��0 SUCCESS | <0 Failed
*******************************************************************************/
int 
I2C_IF_Read(unsigned char ucDevAddr,
        unsigned char *pucData,
        unsigned char ucLen)
{
    unsigned long ulCmdID;

    RETERR_IF_TRUE(pucData == NULL);
    RETERR_IF_TRUE(ucLen == 0);

    // ����I2C�ӵ�ַ����
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
        *pucData = MAP_I2CMasterDataGet(I2C_BASE);    // I2C��������
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
 ��������I2C_IF_Open
 ��  ��: I2C��
 ��  ����ulMode I2C���÷�ʽ 
         I2C_MASTER_MODE_STD ��׼��ʽ 0 | I2C_MASTER_MODE_FST ���ٷ�ʽ 1
 ����ֵ��0 SUCCESS | <0 Failed
*******************************************************************************/
int 
I2C_IF_Open(unsigned long ulMode)
{
    //MAP_HwSemaphoreLock(HWSEM_I2C, HWSEM_WAIT_FOR_EVER);
    MAP_PRCMPeripheralClkEnable(PRCM_I2CA0, PRCM_RUN_MODE_CLK);    // ��������ʱ��
    MAP_PRCMPeripheralReset(PRCM_I2CA0);    // ��λ����

    // ��i2c����Ϊ�ض�ģʽ
    switch(ulMode)
    {
        case I2C_MASTER_MODE_STD:       /* 100000 100kHz */
            MAP_I2CMasterInitExpClk(I2C_BASE,SYS_CLK,false);    // ����I2C�ӿ�
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
 ��������I2C_IF_Close
 ��  ��: I2C�ر� ������������ѯģʽ
 ��  ������
 ����ֵ��0 SUCCESS | <0 Failed
*******************************************************************************/
int 
I2C_IF_Close()
{
    // �ر�I2C����
    MAP_PRCMPeripheralClkDisable(PRCM_I2CA0, PRCM_RUN_MODE_CLK);

    return SUCCESS;
}