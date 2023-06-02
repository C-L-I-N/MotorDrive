/*
 * @Author: NO3 
 * @Copyright (c): Sewingtech Ltd.
 * @Date: 2023-05-30 11:05:45 
 */
#include <stdint.h>
#include <stdbool.h>

void COM_Slave_UART_Init(void *pHW_Handle)
{

}

void COM_Slave_SetTimerISR(void *pHW_Handle, uint32_t Us)
{

}

/**
 * @brief 模拟光栅电平接口，调用20次为一个周期，前10次调用返回高电平，后10次调用返回低电平
 * 
 * @return true 高电平
 * @return false 低电平
 */
bool HOME_GetPhotoelectricSwitchIoLevel(void)
{
    bool result = false;
    static uint8_t counter = 0;

    if (counter < 10)
    {
        result = true;
    }
    else if (counter < 20)
    {
        result = false;
    }

    counter++;
    if (counter >= 20)
    {
        counter = 0;
    }

    return result;
}
