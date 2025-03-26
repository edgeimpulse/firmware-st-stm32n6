---
pagetitle: Readme
lang: en
---
::: {.row}
::: {.col-sm-12 .col-lg-8}


## <b>DCMIPP Snapshot Decimation Mode Example Description</b>

This example shows how to use the DCMIPP IP in Snapshot Mode as a Camera Serial Interface and it is based on the STM32Cube HAL API
that can be used to build any firmware application. Note that security is enabled by default on Cortex-M55.

- This project is targeted to run on STM32N657X0H3QU device on STM32N6570-DK board from STMicroelectronics.  
- The project configures the maximum system clock frequency at 600 MHz.

- The project calls also SCB_EnableICache() and SCB_EnableDCache() functions in order to enable the Layer 1 Core Instruction and Data Caches.

- This example describes how to configure the camera in snapshot and Serial Modes in 800x480 resolution:
- The used sensor OV5640 is configured with 800x480 resolution ,RGB565 Pixel Format and in serial Mode with the Virtual Channel0.
- The CSI is configured with two physical data lanes and Data Type 8 for the Virtual Channel0.
- The **DCMIPP PIPE1** has been set up to select Data Type A which is programmed to choose RGB565. Additionally, it outputs RGB565 pixel format.

- The frames are being captured through **DCMIPP PIPE1** and then dumped into BUFFER_ADDRESS which acts as a storage area for the data that gets overwritten with each new frame
and displayed through the LTDC.

- The **DCMIPP PIPE1** is set up to perform decimation, scaling down the image by a factor of 2 and then by 4 in both the horizontal
  and vertical dimensions. With each reduction, the pitch value and the display window dimensions are adjusted to match the resized image dimensions.

- Initially, an image with dimensions of 800x480 is shown. Following this, images that have been scaled down by factors of 2 and then by 4 are displayed.

- RED LED is ON when an error occurs

#### <b>Notes</b>

 1. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
    based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
    a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
    than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
    To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.
 2. The application needs to ensure that the SysTick time base is always set to 1 millisecond
    to have correct HAL operation.

### <b>Keywords</b>

Graphic, DCMIPP, CSI, Pipe, Virtual Channel, Serial Mode, Snapshot Mode, Pitch, Decimation, LTDC, Display

### <b>Directory contents</b>

      - DCMIPP/DCMIPP_ContinuousMode/FSBL/Src/main.c                         Main program
      - DCMIPP/DCMIPP_ContinuousMode/FSBL/Src/stm32n6xx_it.c                 Interrupt handlers
      - DCMIPP/DCMIPP_ContinuousMode/FSBL/Src/stm32n6xx_hal_msp.c            HAL MSP module
      - DCMIPP/DCMIPP_ContinuousMode/FSBL/Inc/main.h                         Main program header file
      - DCMIPP/DCMIPP_ContinuousMode/FSBL/Inc/stm32n6570_discovery_conf.h    BSP Configuration file
      - DCMIPP/DCMIPP_ContinuousMode/FSBL/Inc/stm32n6xx_hal_conf.h           HAL Configuration file
      - DCMIPP/DCMIPP_ContinuousMode/FSBL/Inc/stm32n6xx_it.h                 Interrupt handlers header file


### <b>Hardware and Software environment</b>

  - This project runs on STM32N657X0H3QU devices.
  - This project has been tested with STMicroelectronics STM32N6570-DK (MB1939)
    board and can be easily tailored to any other supported device
    and development board.

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Set the boot mode in development mode (BOOT1 switch position is 1-3, BOOT0 switch position doesn't matter).
 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory. Code can be executed in this mode for debugging purposes.

 Next, this program can be run in boot from flash mode. This is done by following the instructions below:
 
 - Resort to CubeProgrammer to add a header to the generated binary Project.bin with the following command
   - for Cut1.1,
     - *STM32MP_SigningTool_CLI.exe -bin Project.bin -nk -of 0x80000000 -t fsbl -o Project-trusted.bin -hv 2.1 -dump Project-trusted.bin*
   - for Cut2.0, 
      - *STM32MP_SigningTool_CLI.exe -bin Project.bin -nk -of 0x80000000 -t fsbl -o Project-trusted.bin -hv 2.3 -dump Project-trusted.bin*
   - The resulting binary is Project-trusted.bin.
 - Next, in resorting again to CubeProgrammer, load the binary and its header (Project-trusted.bin) in DK board external Flash at address 0x7000'0000.
 - Set the boot mode in boot from external Flash (BOOT0 switch position is 1-2 and BOOT1 switch position is 1-2).
 - Press the reset button. The code then executes in boot from external Flash mode.



:::
:::


:::
:::

