#include "dac6551.h"

static inline void cs_low(dac6551_t *d)  { HAL_GPIO_WritePin(d->cs_port, d->cs_pin, GPIO_PIN_RESET); }
static inline void cs_high(dac6551_t *d) { HAL_GPIO_WritePin(d->cs_port, d->cs_pin, GPIO_PIN_SET); }

void dac6551_init(dac6551_t *d)
{
    // CS (SYNC) idles high (inactive); it only goes low while a frame is clocked out
    cs_high(d);

    // First write: 0 V
    dac6551_write_code(d, 0);
}

HAL_StatusTypeDef dac6551_write_code(dac6551_t *d, uint16_t code)
{
    // Clamp to 12-bit
    if (code > 0x0FFF) {
        code = 0x0FFF;
    }

    // Frame layout (24 bits total):
    // [23:18] = 000000 (don't care)
    // [17:16] = PD1:PD0 (00 = normal)
    // [15:0]  = data (MSB first), 12-bit code left-aligned (bits 15..4)
    uint32_t frame = ((uint32_t)code) << 4;

    uint8_t tx[3];
    tx[0] = (uint8_t)((frame >> 16) & 0xFF);
    tx[1] = (uint8_t)((frame >> 8) & 0xFF);
    tx[2] = (uint8_t)(frame & 0xFF);

    cs_low(d);
    HAL_StatusTypeDef status = HAL_SPI_Transmit(d->hspi, tx, 3, HAL_MAX_DELAY);
    cs_high(d);

    return status;
}

HAL_StatusTypeDef dac6551_set_mv(dac6551_t *d, uint32_t mv)
{
    // vref_mv is a fixed calibration constant set at init, so it is not overwritten here
    if (mv > d->vref_mv) {
        mv = d->vref_mv;
    }

    uint32_t code = (mv * 4095UL) / d->vref_mv;

    return dac6551_write_code(d, (uint16_t)code);
}
