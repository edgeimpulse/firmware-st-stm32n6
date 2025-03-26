
---
pagetitle: Readme
lang: en
---
::: {.row}
::: {.col-sm-12 .col-lg-8}

## <b>BSEC_OTP_Config Example Description</b>

How to configure OTP (One-Time Programmable) bits.


The example shows how to configure an OTP value using BSEC driver.

The fuse identifier to be programmed should be defined in header file (BSEC_FUSE_ADDRESS).
For lower fuses (allowing bitwise programming), the bit to be programmed should
also be defined in header file (BSEC_FUSE_MASK).

<span style="color:red">As their name indicates, OTP bits are on-time programmable. When set, there is no way to revert them back.</span>

#### <b>Notes</b>

 1. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
    based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
    a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
    than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
    To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.
 2. The application needs to ensure that the SysTick time base is always set to 1 millisecond
    to have correct HAL operation.
 

### <b>Keywords</b>

Reference, Template 

### <b>Directory contents</b>

      - BSEC/BSEC_OTP_Config/Src/main.c                         Main program
      - BSEC/BSEC_OTP_Config/Src/stm32n6xx_it.c                 Interrupt handlers
      - BSEC/BSEC_OTP_Config/Inc/main.h                         Main program header file
      - BSEC/BSEC_OTP_Config/Inc/stm32n6xx_hal_conf.h           HAL Configuration file
      - BSEC/BSEC_OTP_Config/Inc/stm32n6xx_it.h                 Interrupt handlers header file


### <b>Hardware and Software environment</b>

  - This example runs on STM32N657X0H3QU devices.
  - This example has been tested with STMicroelectronics STM32N6570-DK (MB1939)
    board and can be easily tailored to any other supported device
    and development board.
  - <span style="color:red">The boot mode must be set to development mode to run this example.</span>

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Set the boot mode in development mode (BOOT1 switch position is 1-3, BOOT0 switch position doesn't matter).
 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory.
 - Run the example.



:::
:::

