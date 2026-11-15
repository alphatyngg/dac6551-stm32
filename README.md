# dac6551-stm32

Driver for the DAC6551 (12-bit DAC) over SPI on a NUCLEO-F401RE, plus a FreeRTOS CLI over UART to set the output voltage. This was the firmware onboarding task for UW Formula Electric.

## CLI

```
dac <mV>          set output in millivolts
dacRaw <0..4095>  write a raw code
```

## Files

- `Core/Inc/dac6551.h`, `Core/Src/dac6551.c`: the driver
- `Core/Src/cli_io.c`: UART line input and the CLI task
- `Core/Src/dac_cli.c`: the two commands

Only my source files are here. The CubeMX project and HAL code aren't included.

## Setup

1. New CubeMX project for the NUCLEO-F401RE.
2. SPI1 as master, 8-bit, MSB first, prescaler fPCLK/16, CPOL low, CPHA 2nd edge.
3. PB6 as a GPIO output, initial level high (this is CS/SYNC).
4. Enable FREERTOS, CMSIS_v1.
5. Generate code, then copy `Core/` into the project and add `FreeRTOS_CLI.h/.c` from the FreeRTOS-Plus-CLI folder.
6. In `main.c`, inside the USER CODE blocks:

```c
/* USER CODE BEGIN Includes */
#include "dac6551.h"
/* USER CODE END Includes */

/* USER CODE BEGIN PV */
extern SPI_HandleTypeDef hspi1;
dac6551_t g_dac = { .hspi = &hspi1, .cs_port = GPIOB, .cs_pin = GPIO_PIN_6, .vref_mv = 3300 };
extern void vCliTask(void *arg);
extern void vRegisterDacCLI(void);
/* USER CODE END PV */

/* USER CODE BEGIN 2 */
dac6551_init(&g_dac);
vRegisterDacCLI();
xTaskCreate(vCliTask, "cli", 1024, NULL, tskIDLE_PRIORITY + 1, NULL);
/* USER CODE END 2 */
```

## Notes

- SPI mode 1 (CPOL 0, CPHA 1). The DAC latches on the falling edge of SCLK.
- Each write is 3 bytes: 6 don't-care bits, 2 power-down bits (00 for normal), then the 12-bit code in bits 15..4.
- CS idles high and only goes low for the 24 clocks of a write.
- mV to code is `mv * 4095 / vref_mv`, with vref at 3300.


Based on the starter code from the UWFE firmware onboarding guide.