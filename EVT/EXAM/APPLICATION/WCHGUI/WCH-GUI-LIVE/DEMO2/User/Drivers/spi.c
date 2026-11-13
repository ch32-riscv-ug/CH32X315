/*********************************************************************
 * File Name          : spi.c
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2023/8/15
 * Description        : file for spi.
 *********************************************************************************
* Copyright (c) 2023 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#include "spi.h"

/*********************************************************************
 * @fn      SPI1_Init
 * 
 * @brief   Initialize SPI1 
 * 
 * @return  none
 */
void SPI1_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    SPI_InitTypeDef  SPI_InitStructure  = {0};

    RCC_HB2PeriphClockCmd(RCC_HB2Periph_GPIOA | RCC_HB2Periph_GPIOB | RCC_HB2Periph_GPIOC, ENABLE);

    RCC_HB2PeriphClockCmd(RCC_HB2Periph_SPI1, ENABLE);

    RCC_HB2PeriphClockCmd(RCC_HB2Periph_AFIO, ENABLE);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource5, GPIO_AF4);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF4);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF4);

    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_High;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_High;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_Mode      = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize  = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL      = SPI_CPOL_Low;
    SPI_InitStructure.SPI_CPHA      = SPI_CPHA_1Edge;
    SPI_InitStructure.SPI_NSS       = SPI_NSS_Soft;
#if SPI_SPEED_MODE == SPI_FAST_MODE
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_Mode0;
#else
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_Mode0;
#endif

    SPI_InitStructure.SPI_FirstBit      = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial = 7;
    SPI_Init(SPI1, &SPI_InitStructure);

    SPI_Cmd(SPI1, ENABLE);

    SPI1->HSCR = 1;

#if SPI_SPEED_MODE == SPI_FAST_MODE

#endif
}

/*********************************************************************
 * @fn      SPI1_Read_DMA_Init
 * 
 * @brief   Initialize DMA for SPI1, In this condition every time send 0XFF to read data
 * 
 * @return  none
 */
void SPI1_Read_DMA_Init()
{
    static uint32_t DummyByte         = 0xff;
    DMA_InitTypeDef DMA_InitStructure = {0};

    RCC_HBPeriphClockCmd(RCC_HBPeriph_DMA1, ENABLE);

    // Tx
    DMA_DeInit(SPI1_DMA_TX_CH);

    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&SPI1->DATAR;
    DMA_InitStructure.DMA_Memory0BaseAddr     = (uint32_t)&DummyByte;
    DMA_InitStructure.DMA_DIR                = DMA_DIR_PeripheralDST;
    DMA_InitStructure.DMA_BufferSize         = 0;
    DMA_InitStructure.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc          = DMA_MemoryInc_Disable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize     = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode               = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority           = DMA_Priority_High;
    DMA_InitStructure.DMA_M2M                = DMA_M2M_Disable;
    DMA_Init(SPI1_DMA_TX_CH, &DMA_InitStructure);

    // Rx
    DMA_DeInit(SPI1_DMA_RX_CH);

    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&SPI1->DATAR;
    DMA_InitStructure.DMA_Memory0BaseAddr     = (uint32_t)NULL;
    DMA_InitStructure.DMA_DIR                = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_BufferSize         = 0;
    DMA_InitStructure.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc          = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize     = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode               = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority           = DMA_Priority_High;
    DMA_InitStructure.DMA_M2M                = DMA_M2M_Disable;
    DMA_Init(SPI1_DMA_RX_CH, &DMA_InitStructure);
}

/*********************************************************************
 * @fn      SPI1_Read_DMA
 * 
 * @brief   read data from SPI1 via DMA
 * 
 * @param   data -  the data to recive
 *          len -  read length
 * 
 * @return  none
 */
void SPI1_Read_DMA(uint8_t* data, uint16_t len)
{
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == SET)
        ;
    DMA_Cmd(SPI1_DMA_TX_CH, DISABLE);
    DMA_Cmd(SPI1_DMA_RX_CH, DISABLE);
    DMA_ClearFlag(DMA1_FLAG_TC3);
    DMA_ClearFlag(DMA1_FLAG_TC3);
    SPI1_DMA_TX_CH->CNTR = len;

    SPI1_DMA_RX_CH->MADDR = (uint32_t)data;
    SPI1_DMA_RX_CH->CNTR  = len;

    DMA_Cmd(SPI1_DMA_RX_CH, ENABLE);
    DMA_Cmd(SPI1_DMA_TX_CH, ENABLE);
}

/*********************************************************************
 * @fn      SPI3_Init
 *
 * @brief   Initialize SPI3 for LCD
 *
 * @return  none
 */
void SPI3_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    SPI_InitTypeDef  SPI_InitStructure  = {0};

    RCC_HB2PeriphClockCmd(RCC_HB2Periph_GPIOC, ENABLE);
    RCC_HB2PeriphClockCmd(RCC_HB2Periph_AFIO, ENABLE);
    RCC_HB1PeriphClockCmd(RCC_HB1Periph_SPI3, ENABLE);

    GPIO_PinAFConfig(GPIOC, GPIO_PinSource5, GPIO_AF4);
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource6, GPIO_AF4);

    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    SPI_InitStructure.SPI_Direction         = SPI_Direction_1Line_Tx;
    SPI_InitStructure.SPI_Mode              = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize          = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL              = SPI_CPOL_Low;
    SPI_InitStructure.SPI_CPHA              = SPI_CPHA_1Edge;
    SPI_InitStructure.SPI_NSS               = SPI_NSS_Soft;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_Mode2;
    SPI_InitStructure.SPI_FirstBit          = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial     = 7;
    SPI_Init(SPI3, &SPI_InitStructure);

    SPI_Cmd(SPI3, ENABLE);
}

/*********************************************************************
 * @fn      SPI3_DMA_Init
 *
 * @brief   Initialize DMA for SPI3 TX
 *
 * @return  none
 */
void SPI3_DMA_Init()
{
    DMA_InitTypeDef DMA_InitStructure = {0};

    RCC_HBPeriphClockCmd(RCC_HBPeriph_DMA1, ENABLE);

    DMA_DeInit(SPI3_DMA_TX_CH);

    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&SPI3->DATAR;
    DMA_InitStructure.DMA_Memory0BaseAddr     = (uint32_t)NULL;
    DMA_InitStructure.DMA_DIR                = DMA_DIR_PeripheralDST;
    DMA_InitStructure.DMA_BufferSize         = 0;
    DMA_InitStructure.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc          = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize     = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode               = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority           = DMA_Priority_High;
    DMA_InitStructure.DMA_M2M                = DMA_M2M_Disable;
    DMA_Init(SPI3_DMA_TX_CH, &DMA_InitStructure);

    SPI_I2S_DMACmd(SPI3, SPI_I2S_DMAReq_Tx, ENABLE);
}

/*********************************************************************
 * @fn      SPI3_Write
 *
 * @brief   send one byte via SPI3
 *
 * @param   data -  the data to send
 *
 * @return  none
 */
void SPI3_Write(uint8_t data)
{
    while (SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_TXE) == RESET)
        ;
    SPI_I2S_SendData(SPI3, data);

    while (SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_BSY) == SET)
        ;
}

/*********************************************************************
 * @fn      SPI3_Write_DMA
 *
 * @brief   send a lot of data via SPI3 DMA
 *
 * @param   data -  the data to be send
 *          len -  the length of data
 *
 * @return  none
 */
void SPI3_Write_DMA(uint8_t* data, uint16_t len)
{
    while (SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_BSY) == SET)
        ;

    DMA_Cmd(SPI3_DMA_TX_CH, DISABLE);
    DMA_ClearFlag(SPI3_DMA_TC_FLAG);
    SPI3_DMA_TX_CH->CFGR  = 0x2090;
    SPI3_DMA_TX_CH->MADDR = (uint32_t)data;
    SPI3_DMA_TX_CH->CNTR  = len;

    DMA_Cmd(SPI3_DMA_TX_CH, ENABLE);
}

/*********************************************************************
 * @fn      SPI3_SetDataSize
 *
 * @brief   switch SPI3 frame size (8-bit / 16-bit)
 *
 * @param   dataSize - SPI_DataSize_8b or SPI_DataSize_16b
 *
 * @return  none
 */
void SPI3_SetDataSize(uint16_t dataSize)
{
    uint16_t ctlr1;

    while (SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_BSY) == SET)
        ;

    /* DFF bit can only be changed while SPI is disabled */
    SPI_Cmd(SPI3, DISABLE);
    ctlr1 = SPI3->CTLR1;
    ctlr1 &= (uint16_t)(~SPI_DataSize_16b);
    ctlr1 |= dataSize;
    SPI3->CTLR1 = ctlr1;
    SPI_Cmd(SPI3, ENABLE);
}

/*********************************************************************
 * @fn      SPI3_Write_DMA16
 *
 * @brief   SPI3 16-bit frame DMA transfer (one half word per pixel)
 *
 * @param   data  - half word buffer holding RGB565
 *          count - half word count
 *
 * @return  none
 */
void SPI3_Write_DMA16(uint16_t *data, uint16_t count)
{
    while (SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_BSY) == SET)
        ;

    DMA_Cmd(SPI3_DMA_TX_CH, DISABLE);
    DMA_ClearFlag(SPI3_DMA_TC_FLAG);

    SPI3_DMA_TX_CH->MADDR = (uint32_t)data;
    SPI3_DMA_TX_CH->CNTR  = count;
    /* 0x2590: memory to peripheral, address increment, high priority,
       both peripheral and memory data width set to half word */
    SPI3_DMA_TX_CH->CFGR  = 0x2590;

    DMA_Cmd(SPI3_DMA_TX_CH, ENABLE);
}
