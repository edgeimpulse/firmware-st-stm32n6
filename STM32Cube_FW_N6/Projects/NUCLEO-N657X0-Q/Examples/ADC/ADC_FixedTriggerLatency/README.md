## <b>ADC_FixedTriggerLatency Example Description</b>

How to use an ADC peripheral to perform a single ADC conversion on a channel
at each trigger event from a timer without any uncertainty (fixed trigger latency).

This example is based on the STM32N6xx ADC HAL API.

Example configuration:

ADC is configured to convert a single channel, in single conversion mode,
from HW trigger: timer peripheral (timer instance: TIM2).
ck_timg1 is selected as the kernel clock source.
DMA is configured to transfer conversion data in an array, in circular mode.
A timer is configured in time base and to generate TRGO events.

**Note:**
Jitter-free operation is guaranteed only when one of the 2 requirements
below is met (depending on the selected trigger source):

 1.the counter period must be a multiple of the ADC clock period
   (TIMx_PSC1 + 1) x (TIMx_ARR1 + 1) x Ttim_ker_ck = n x Tadc_ker_ck

 2. the compare value must be a multiple of the ADC clock period
     (TIMx_PSC1 + 1) x TIMx_CMPy x Ttim_ker_ck = m x Tadc_ker_ck

If none of the 2 above requirement are met, the trigger is still generated,
but the latency is not constant and varies with the timer and ADC clocks
phase shift.

Example execution:

From the main program execution, the ADC group regular converts the
selected channel at each trig from timer. DMA transfers conversion data to the array,
indefinitely (DMA in circular mode).

**Note:**
It is mandatory to start first the ADC clock before enabling the timer

When conversion are completed (DMA half or full buffer complete),
interruption occurs and data is processed in callback functions (for this example purpose,
convert digital data to physical value in mV).

LED1 is used to monitor program execution status:

- Normal operation: ADC group regular buffer activity (buffer complete events): LED toggle
- Error: LED remaining turned on

Debug: variables to monitor with debugger:

- "uhADCxConvertedData": ADC group regular buffer conversion data (resolution 12 bits)
- "uhADCxConvertedData_Voltage_mVolt": ADC group regular buffer conversion data computed to voltage value (unit: mV)

Connection needed:

Use an external power supply to generate a voltage in range [0V; 3.3V]
and connect it to analog input pin (cf pin below).

**Note:** If no power supply available, this pin can be let floating (in this case
ADC conversion data will be undetermined).

Other peripherals used:

 - 1 GPIO for LED1
 - 1 GPIO for analog input: ADC channel 0 on pin PC.00 (Arduino A0 pin 1 CN4)
 - 1 DMA channel
 - 1 timer instance

#### <b>Notes</b>

 1. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
    based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
    a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
    than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
    To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

 2. The example needs to ensure that the SysTick time base is always set to 1 millisecond
    to have correct HAL operation.

### <b>Keywords</b>

ADC, analog digital converter, analog, conversion, voltage, channel, analog input, regular, injected, DMA transfer, trigger

### <b>Directory contents</b>

File | Description
 --- | ---
      - ADC/ADC_FixedTriggerLatency/FSBL/Inc/main.h                   | Header for main.c module
      - ADC/ADC_FixedTriggerLatency/FSBL/Inc/stm32n6xx_hal_conf.h     | HAL configuration file
      - ADC/ADC_FixedTriggerLatency/FSBL/Inc/stm32n6xx_it.h           | Interrupt handlers header file
      - ADC/ADC_FixedTriggerLatency/FSBL/Inc/stm32n6xx_nucleo_conf.h  | BSP configuration file
      - ADC/ADC_FixedTriggerLatency/FSBL/Src/main.c                   | Main program
      - ADC/ADC_FixedTriggerLatency/FSBL/Src/stm32n6xx_hal_msp.c      | HAL MSP module
      - ADC/ADC_FixedTriggerLatency/FSBL/Src/stm32n6xx_it.c           | Interrupt handlers
      - ADC/ADC_FixedTriggerLatency/FSBL/Src/system_stm32n6xx_fsbl.c  | STM32N6xx system source file

### <b>Hardware and Software environment</b>

  - This example runs on STM32N6xx devices.

  - This example has been tested with STMicroelectronics NUCLEO-N657X0
    board and can be easily tailored to any other supported device
    and development board.

### <b>How to use it ?</b>

In order to make the program work, you must do the following :
 - Set the boot mode in development mode (BOOT1 switch position is 2-3, BOOT0 switch position doesn't matter).
 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory. Code can be executed in this mode for debugging purposes.

 Next, this program can be run in boot from flash mode. This is done by following the instructions below:

 - Resort to CubeProgrammer to add a header to the generated binary Project.bin with the following command
   - for Cut1.1,
     - *STM32MP_SigningTool_CLI.exe -bin Project.bin -nk -of 0x80000000 -t fsbl -o Project-trusted.bin -hv 2.1 -dump Project-trusted.bin*
   - for Cut2.0, 
      - *STM32MP_SigningTool_CLI.exe -bin Project.bin -nk -of 0x80000000 -t fsbl -o Project-trusted.bin -hv 2.3 -dump Project-trusted.bin*
   - The resulting binary is Project-trusted.bin.
 - Next, in resorting again to CubeProgrammer, load the binary and its header (Project-trusted.bin) in the board external Flash at address 0x7000'0000.
 - Set the boot mode in boot from external Flash (BOOT0 switch position is 1-2 and BOOT1 switch position is 1-2).
 - Press the reset button. The code then executes in boot from external Flash mode.
