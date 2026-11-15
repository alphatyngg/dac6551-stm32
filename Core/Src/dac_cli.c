// FreeRTOS CLI commands for the DAC6551.
#include <stdio.h>
#include "FreeRTOS_CLI.h"
#include "dac6551.h"

extern dac6551_t g_dac;

static BaseType_t cmdDacSet_mV(char *writeBuf, size_t writeLen, const char *cmdStr)
{
    // Usage: dac <vout_mV>
    unsigned long vout = 0;
    if (sscanf(cmdStr, "dac %lu", &vout) != 1) {
        snprintf(writeBuf, writeLen, "Usage: dac <vout_mV>");
        return pdFALSE;
    }
    if (dac6551_set_mv(&g_dac, (uint32_t)vout) == HAL_OK)
        snprintf(writeBuf, writeLen, "Command Sent");
    else
        snprintf(writeBuf, writeLen, "SPI error");
    return pdFALSE;
}

static BaseType_t cmdDacRaw(char *writeBuf, size_t writeLen, const char *cmdStr)
{
    // Usage: dacRaw <code 0..4095>
    unsigned code = 0;
    int n = sscanf(cmdStr, "dacRaw %u", &code);
    if (n < 1 || code > 4095) {
        snprintf(writeBuf, writeLen, "Usage: dacRaw <0..4095>");
        return pdFALSE;
    }
    if (dac6551_write_code(&g_dac, (uint16_t)code) == HAL_OK)
        snprintf(writeBuf, writeLen, "OK: code=%u", code);
    else
        snprintf(writeBuf, writeLen, "SPI error");
    return pdFALSE;
}

static const CLI_Command_Definition_t xDacSetCmd = {
    "dac", "\r\ndac <vout_mV>\r\n", cmdDacSet_mV, 1
};

static const CLI_Command_Definition_t xDacSetRaw = {
    "dacRaw", "\r\ndacRaw <code>\r\n", cmdDacRaw, 1
};

void vRegisterDacCLI(void)
{
    FreeRTOS_CLIRegisterCommand(&xDacSetCmd);
    FreeRTOS_CLIRegisterCommand(&xDacSetRaw);
}
