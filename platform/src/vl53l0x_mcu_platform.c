/*
 * MCU implementation of the STM API platform layer.
 * Build this instead of the Windows-specific vl53l0x_platform.c.
 */

#include "vl53l0x_platform.h"

#include <stddef.h>

VL53L0X_Error VL53L0X_LockSequenceAccess(VL53L0X_DEV Dev)
{
    (void)Dev;
    return VL53L0X_ERROR_NONE;
}

VL53L0X_Error VL53L0X_UnlockSequenceAccess(VL53L0X_DEV Dev)
{
    (void)Dev;
    return VL53L0X_ERROR_NONE;
}

VL53L0X_Error VL53L0X_WriteMulti(VL53L0X_DEV Dev, uint8_t index,
                                 uint8_t *pdata, uint32_t count)
{
    if ((Dev == NULL) || (count >= COMMS_BUFFER_SIZE) ||
        ((count > 0u) && (pdata == NULL)))
    {
        return VL53L0X_ERROR_INVALID_PARAMS;
    }

    return (VL53L0X_write_multi(Dev->I2cDevAddr, index, pdata,
                                (int32_t)count) == 0) ?
           VL53L0X_ERROR_NONE : VL53L0X_ERROR_CONTROL_INTERFACE;
}

VL53L0X_Error VL53L0X_ReadMulti(VL53L0X_DEV Dev, uint8_t index,
                                uint8_t *pdata, uint32_t count)
{
    if ((Dev == NULL) || (count == 0u) || (count >= COMMS_BUFFER_SIZE) ||
        (pdata == NULL))
    {
        return VL53L0X_ERROR_INVALID_PARAMS;
    }

    return (VL53L0X_read_multi(Dev->I2cDevAddr, index, pdata,
                               (int32_t)count) == 0) ?
           VL53L0X_ERROR_NONE : VL53L0X_ERROR_CONTROL_INTERFACE;
}

VL53L0X_Error VL53L0X_WrByte(VL53L0X_DEV Dev, uint8_t index, uint8_t data)
{
    if (Dev == NULL)
    {
        return VL53L0X_ERROR_INVALID_PARAMS;
    }

    return (VL53L0X_write_byte(Dev->I2cDevAddr, index, data) == 0) ?
           VL53L0X_ERROR_NONE : VL53L0X_ERROR_CONTROL_INTERFACE;
}

VL53L0X_Error VL53L0X_WrWord(VL53L0X_DEV Dev, uint8_t index, uint16_t data)
{
    if (Dev == NULL)
    {
        return VL53L0X_ERROR_INVALID_PARAMS;
    }

    return (VL53L0X_write_word(Dev->I2cDevAddr, index, data) == 0) ?
           VL53L0X_ERROR_NONE : VL53L0X_ERROR_CONTROL_INTERFACE;
}

VL53L0X_Error VL53L0X_WrDWord(VL53L0X_DEV Dev, uint8_t index, uint32_t data)
{
    if (Dev == NULL)
    {
        return VL53L0X_ERROR_INVALID_PARAMS;
    }

    return (VL53L0X_write_dword(Dev->I2cDevAddr, index, data) == 0) ?
           VL53L0X_ERROR_NONE : VL53L0X_ERROR_CONTROL_INTERFACE;
}

VL53L0X_Error VL53L0X_RdByte(VL53L0X_DEV Dev, uint8_t index, uint8_t *data)
{
    if ((Dev == NULL) || (data == NULL))
    {
        return VL53L0X_ERROR_INVALID_PARAMS;
    }

    return (VL53L0X_read_byte(Dev->I2cDevAddr, index, data) == 0) ?
           VL53L0X_ERROR_NONE : VL53L0X_ERROR_CONTROL_INTERFACE;
}

VL53L0X_Error VL53L0X_RdWord(VL53L0X_DEV Dev, uint8_t index, uint16_t *data)
{
    if ((Dev == NULL) || (data == NULL))
    {
        return VL53L0X_ERROR_INVALID_PARAMS;
    }

    return (VL53L0X_read_word(Dev->I2cDevAddr, index, data) == 0) ?
           VL53L0X_ERROR_NONE : VL53L0X_ERROR_CONTROL_INTERFACE;
}

VL53L0X_Error VL53L0X_RdDWord(VL53L0X_DEV Dev, uint8_t index, uint32_t *data)
{
    if ((Dev == NULL) || (data == NULL))
    {
        return VL53L0X_ERROR_INVALID_PARAMS;
    }

    return (VL53L0X_read_dword(Dev->I2cDevAddr, index, data) == 0) ?
           VL53L0X_ERROR_NONE : VL53L0X_ERROR_CONTROL_INTERFACE;
}

VL53L0X_Error VL53L0X_UpdateByte(VL53L0X_DEV Dev, uint8_t index,
                                 uint8_t AndData, uint8_t OrData)
{
    uint8_t data;
    VL53L0X_Error status;

    status = VL53L0X_RdByte(Dev, index, &data);
    if (status != VL53L0X_ERROR_NONE)
    {
        return status;
    }

    data = (uint8_t)((data & AndData) | OrData);
    return VL53L0X_WrByte(Dev, index, data);
}

VL53L0X_Error VL53L0X_PollingDelay(VL53L0X_DEV Dev)
{
    (void)Dev;
    return (VL53L0X_wait_ms(1) == 0) ?
           VL53L0X_ERROR_NONE : VL53L0X_ERROR_CONTROL_INTERFACE;
}
