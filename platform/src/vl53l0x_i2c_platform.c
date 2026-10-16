/*
 * Generic MCU transport for the VL53L0X breakout forwarding firmware.
 * Implement the VL53L0X_Port_* hooks in the target MCU's I2C driver.
 */

#include "vl53l0x_i2c_platform.h"

#include <stddef.h>
#include <string.h>

#define VL53L0X_BREAKOUT_WAIT_MS 1u
#define VL53L0X_REGISTER_SIZE 1u

static int32_t VL53L0X_validate_address(uint8_t address, uint8_t *address_7bit)
{
    if ((address_7bit == NULL) || (address < 0x10u) ||
        (address > 0xEEu) || ((address & 1u) != 0u))
    {
        return 1;
    }

    *address_7bit = (uint8_t)(address >> 1);
    return 0;
}

static int32_t VL53L0X_write_buffer(uint8_t address, const uint8_t *data,
                                    uint32_t count)
{
    uint8_t address_7bit;

    if ((data == NULL) || (count == 0u) ||
        (VL53L0X_validate_address(address, &address_7bit) != 0))
    {
        return 1;
    }

    return (VL53L0X_Port_I2C_Write(address_7bit, data, count) == 0) ? 0 : 1;
}

static int32_t VL53L0X_read_buffer(uint8_t address, uint8_t *data,
                                   uint32_t count)
{
    uint8_t address_7bit;

    if ((data == NULL) || (count == 0u) ||
        (VL53L0X_validate_address(address, &address_7bit) != 0))
    {
        return 1;
    }

    return (VL53L0X_Port_I2C_Read(address_7bit, data, count) == 0) ? 0 : 1;
}

int32_t VL53L0X_comms_initialise(uint8_t comms_type, uint16_t comms_speed_khz)
{
    if (comms_type != I2C)
    {
        return 1;
    }

    return (VL53L0X_Port_I2C_Init(comms_speed_khz) == 0) ? 0 : 1;
}

int32_t VL53L0X_comms_close(void)
{
    return (VL53L0X_Port_I2C_Close() == 0) ? 0 : 1;
}

int32_t VL53L0X_cycle_power(void)
{
    return 1;
}

int32_t VL53L0X_wait_ms(int32_t wait_ms)
{
    if (wait_ms < 0)
    {
        return 1;
    }

    return (VL53L0X_Port_DelayMs((uint32_t)wait_ms) == 0) ? 0 : 1;
}

int32_t VL53L0X_platform_wait_us(int32_t wait_us)
{
    uint32_t wait_ms;

    if (wait_us < 0)
    {
        return 1;
    }

    wait_ms = ((uint32_t)wait_us + 999u) / 1000u;
    return (VL53L0X_Port_DelayMs(wait_ms) == 0) ? 0 : 1;
}

int32_t VL53L0X_set_gpio(uint8_t level)
{
    (void)level;
    return 1;
}

int32_t VL53L0X_get_gpio(uint8_t *plevel)
{
    (void)plevel;
    return 1;
}

int32_t VL53L0X_release_gpio(void)
{
    return 1;
}

int32_t VL53L0X_get_timer_frequency(int32_t *ptimer_freq_hz)
{
    (void)ptimer_freq_hz;
    return 1;
}

int32_t VL53L0X_get_timer_value(int32_t *ptimer_count)
{
    (void)ptimer_count;
    return 1;
}

int32_t VL53L0X_write_multi(uint8_t address, uint8_t index,
                            uint8_t *pdata, int32_t count)
{
    uint8_t buffer[COMMS_BUFFER_SIZE];
    uint32_t write_count;

    if ((count < 0) || ((uint32_t)count >= COMMS_BUFFER_SIZE) ||
        ((count > 0) && (pdata == NULL)))
    {
        return 1;
    }

    buffer[0] = index;
    if (count > 0)
    {
        (void)memcpy(&buffer[VL53L0X_REGISTER_SIZE], pdata, (size_t)count);
    }
    write_count = (uint32_t)count + VL53L0X_REGISTER_SIZE;

    if ((VL53L0X_wait_ms((int32_t)VL53L0X_BREAKOUT_WAIT_MS) != 0) ||
        (VL53L0X_write_buffer(address, buffer, write_count) != 0))
    {
        return 1;
    }

    return 0;
}

int32_t VL53L0X_read_multi(uint8_t address, uint8_t index,
                           uint8_t *pdata, int32_t count)
{
    uint8_t pending_data[COMMS_BUFFER_SIZE];
    uint32_t read_count;

    if ((count <= 0) || ((uint32_t)count >= COMMS_BUFFER_SIZE) ||
        (pdata == NULL))
    {
        return 1;
    }
    read_count = (uint32_t)count;

    if ((VL53L0X_write_multi(address, index, NULL, 0) != 0) ||
        (VL53L0X_wait_ms((int32_t)VL53L0X_BREAKOUT_WAIT_MS) != 0) ||
        (VL53L0X_read_buffer(address, pending_data, read_count) != 0) ||
        (VL53L0X_wait_ms((int32_t)VL53L0X_BREAKOUT_WAIT_MS) != 0))
    {
        return 1;
    }

    return VL53L0X_read_buffer(address, pdata, read_count);
}

int32_t VL53L0X_write_byte(uint8_t address, uint8_t index, uint8_t data)
{
    return VL53L0X_write_multi(address, index, &data, 1);
}

int32_t VL53L0X_write_word(uint8_t address, uint8_t index, uint16_t data)
{
    uint8_t buffer[BYTES_PER_WORD];

    buffer[0] = (uint8_t)(data >> 8);
    buffer[1] = (uint8_t)data;
    return VL53L0X_write_multi(address, index, buffer, BYTES_PER_WORD);
}

int32_t VL53L0X_write_dword(uint8_t address, uint8_t index, uint32_t data)
{
    uint8_t buffer[BYTES_PER_DWORD];

    buffer[0] = (uint8_t)(data >> 24);
    buffer[1] = (uint8_t)(data >> 16);
    buffer[2] = (uint8_t)(data >> 8);
    buffer[3] = (uint8_t)data;
    return VL53L0X_write_multi(address, index, buffer, BYTES_PER_DWORD);
}

int32_t VL53L0X_read_byte(uint8_t address, uint8_t index, uint8_t *pdata)
{
    return VL53L0X_read_multi(address, index, pdata, 1);
}

int32_t VL53L0X_read_word(uint8_t address, uint8_t index, uint16_t *pdata)
{
    uint8_t buffer[BYTES_PER_WORD];
    int32_t status;

    if (pdata == NULL)
    {
        return 1;
    }

    status = VL53L0X_read_multi(address, index, buffer, BYTES_PER_WORD);
    if (status == 0)
    {
        *pdata = (uint16_t)(((uint16_t)buffer[0] << 8) | buffer[1]);
    }

    return status;
}

int32_t VL53L0X_read_dword(uint8_t address, uint8_t index, uint32_t *pdata)
{
    uint8_t buffer[BYTES_PER_DWORD];
    int32_t status;

    if (pdata == NULL)
    {
        return 1;
    }

    status = VL53L0X_read_multi(address, index, buffer, BYTES_PER_DWORD);
    if (status == 0)
    {
        *pdata = ((uint32_t)buffer[0] << 24) |
                 ((uint32_t)buffer[1] << 16) |
                 ((uint32_t)buffer[2] << 8) |
                 (uint32_t)buffer[3];
    }

    return status;
}
