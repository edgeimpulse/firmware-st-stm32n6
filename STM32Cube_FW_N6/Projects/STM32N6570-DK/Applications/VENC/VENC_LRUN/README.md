---
pagetitle: Readme
lang: en
---
::: {.row}
::: {.col-sm-12 .col-lg-8}

## <b>VENC Description</b>

This project demonstrates the use of the STM32N6 video encoder and the camera pipeline.<br>It is targeted to run on STM32N657xx device on STM32N6570-DK board from STMicroelectronics.

This application is meant to run in internal SRAM via the Load & Run method. The application is stored in external flash and then loaded in internal RAM for execution.<br>
It can also be run in dev_mode debug. For information on those modes, see the "How to use" section.

### <b> Example behaviour </b>

This example takes a camera (OV5640) input and transfers it to the video encoder through the camera pipeline. The camera image is also displayed on the board's LCD screen for user feedback of what is being recorded.<br>
The video encoder output stream is saved to the SD card. The output is raw since no file system is used for now. For information on how to get data from the SD card after the execution of the example, see the "How to use" section.

This example is 'bare-metal', meaning there is no use of any RTOS.

The example does the following :<br>

 - Enable all internal RAMs
 - Configure all necessary clocks
 - Configure all necessary security attributes (RISC and RIMC)
 - Enable the SD card via the BSP
 - Enable the camera via the BSP
 - Enable the LCD via the BSP
 - Initialize the encoder
 - Encode 600 frames

Depending on the execution mode, some of the initialization will be performed in the FSBL.

#### <b>Error Handling</b><br>

Upon successful execution of the example, both LEDs on the board will turn on after initialization and turn off when 600 frames have been encoded.

If an error occurs during initialization, the red LED will blink indefinitely (this can for example be caused by the SD card not being present).

#### <b>Notes</b>

 1. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
    based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
    a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
    than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
    To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

 2. The application needs to ensure that the SysTick time base is always set to 1 millisecond
    to have correct HAL operation.


### <b>Keywords</b>

Graphics, VENC, Encoding,  Hardware Encoding

### <b>Directory contents</b>

#### <b>Sub-project FSBL</b>

    - VENC_LRUN_Example/FSBL/Inc/main.h                    Header for main.c module
    - VENC_LRUN_Example/FSBL/Inc/extmem.h                  Header for extmem.c module
    - VENC_LRUN_Example/FSBL/Inc/partition_stm32n657xx.h   SAU partition configuration
    - VENC_LRUN_Example/FSBL/Incstm32n6xx_hal_conf.h       HAL Configuration file
    - VENC_LRUN_Example/FSBL/Inc/stm32n6xx_it.h            Interrupt handlers header file
    - VENC_LRUN_Example/FSBL/stm32_extmem_conf.h           External memory manager Configuration file
    - VENC_LRUN_Example/FSBL/Src/main.c                    Main program
    - VENC_LRUN_Example/FSBL/Src/extmem.c                  code to initialize external memory
    - VENC_LRUN_Example/FSBL/Src/stm32n6xx_hal_msp.c       HAL MSP module
    - VENC_LRUN_Example/FSBL/Src/stm32n6xx_it.c            Interrupt handlers
    - VENC_LRUN_Example/FSBL/Src/system_stm32n6xx_fsbl.c   STM32N6xx system source file

#### <b>Sub-project Appli</b>
    - VENC_LRUN_Example/Appli/Inc/main.h                       Header for main.c module
    - VENC_LRUN_Example/Appli/Inc/stm32n6xx_hal_conf.h         HAL Configuration file
    - VENC_LRUN_Example/Appli/Inc/stm32n6xx_it.h               Interrupt handlers header file
    - VENC_LRUN_Example/Appli/Inc/partition_stm32n657xx.h      SAU partition configuration
    - VENC_LRUN_Example/Appli/Inc/stm32_assert.h               Assert function definition
    - VENC_LRUN_Example/Appli/Inc/ewl_conf.h                   EWL configuration file
    - VENC_LRUN_Example/Appli/Inc/stm32n6570_discovery_conf.h  BSP configuration file
    - VENC_LRUN_Example/Appli/Src/main.c                       Main program
    - VENC_LRUN_Example/Appli/Src/hal_timebase_tim.c           HAL timer timebase source file
    - VENC_LRUN_Example/Appli/Src/stm32n6xx_it.c               Interrupt handlers
    - VENC_LRUN_Example/Appli/Src/system_stm32n6xx.c           STM32N6xx system source file

### <b>Hardware and Software environment</b>

  - This template runs on STM32N6 devices.

  - This template has been tested with STMicroelectronics STM32N6570-DK (MB1939)
    board and can be easily tailored to any other supported device
    and development board.

  - On STM32N6570-DK board, the BOOT0 mechanical slide switch must be set to SW1.

  - The following OTP fuses are set in this template:

    - VDDIO3_HSLV=1     I/O XSPIM_P2 High speed option enabled

**WARNING**

When OTP fuses are set, they can not be reset.
The NO_OTP_FUSE option is selected by default using the Preprocessor definition in the IDE. XSPI speed will however be limited to 50MHz.
To use the full XSPI speed, delete the NO_OTP_FUSE definition.

NOTE : for Cut1.1, the bootrom prevents OTP programming when booting from flash. The FSBL code must then be executed once in dev boot mode to set the OTP fuse.

### <b>How to use it ?</b>

The example can be run either in development mode or in Load & Run mode.

#### <b> Development mode </b>

Make sure that BOOT1 switch position is 1-3. BOOT0 position does not matter. Then, plug in your board via the ST-LINK USB port.

Select the "Appli" tab in IAR and select the "DK_debug" configuration. This enables the development mode configuration.<br>
Then, simply click the execute button and the program will run in debug mode.

#### <b> Load & Run </b>

This mode enables execution without having to connect through an IDE. The application will be stored in external memory and therefore will no require any external tools once loaded onto the board.

It is expected that a command line environment is configured with the CubeProgrammer in its PATH.

 - Make sure that the "Appli" project in IAR is in the "DK_LRUN" configuration
 - Compile the FSBL project and the Appli project
 - Using the command line, add the header to the Appli and the FSBL (see "Annex : adding a header" section of this README)
 - Make sure that the board is in development mode (BOOT1 switch position is 1-3)
 - Program the external flash using CubeProgrammer with the FSBL (with header) at address 0x7000'0000 and the Application (with header) at address 0x7010'0000
 - Switch to boot from external flash mode : BOOT0 switch position is 1-2 and BOOT1 switch position is 1-2
 - Press the reset button
 - The example should execute

#### <b> After execution </b>

After the red and green LED have turned off, 600 frames of video have been encoded. To play them back, the stream needs to be extracted from the SD card.
After plugging the SD card into a computer, its contents can be read using the `dd` command in git bash. Example : `dd of=dump.bin if=[path to sd e.g. /dev/sdd1] ibs=512 obs=512 count=25000` to dump 66000 blocks of 512 bytes from an sd.

To read back encoded video, it can be converted from raw bytestream to mp4 using a tool called [ffmpeg](https://trac.ffmpeg.org/) with the following command : `ffmpeg -f h264 -framerate 30 -i [extracted file] -c copy [output file]`

The file can then be read using VLC.

#### <b> Adding a header </b>

 - Resort to CubeProgrammer to add a header to a binary Project.bin with the following command
   - for Cut1.1,
     - *STM32MP_SigningTool_CLI.exe -bin Project.bin -nk -of 0x80000000 -t fsbl -o Project-trusted.bin -hv 2.1 -dump Project-trusted.bin*
   - for Cut2.0, 
      - *STM32MP_SigningTool_CLI.exe -bin Project.bin -nk -of 0x80000000 -t fsbl -o Project-trusted.bin -hv 2.3 -dump Project-trusted.bin*
   - The resulting binary is Project-trusted.bin.  
:::
:::