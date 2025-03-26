## <b>TIM_DeadtimeInsertion Example Description</b>

This example shows how to configure the TIM peripheral in PWM (Pulse Width Modulation) mode
with asymmetric deadtime insertion between the complementaty channels.

- At the beginning of the main program the HAL_Init() function is called to reset all the peripherals,
initialize the Flash interface and the systick. Then the SystemClock_Config() function is used to configure the system
clock (SYSCLK) to run at 600 MHz.

- In this example TIM1 input clock (tim_ker_ck) is set to Timer groups clock.  
      => tim_ker_ck = SYSCLK/TIMPRE = 400 MHz/2 = 200 MHz with TIMPRE=DIV2

- To get TIM1 counter clock (tim_cnt_ck) at 1 MHz, the prescaler is computed as follows:  
      Prescaler = (tim_ker_ck / tim_cnt_ck) - 1  
      Prescaler = (200 MHz / 1 MHz) - 1

- To get TIM1 output clock at 5 KHz, the period (ARR) is computed as follows:  
      ARR = (tim_cnt_ck / TIM1 output clock) - 1  
      ARR = 199

- Initial conditions (DT is off): TIM1 Channel1(N) duty cycle (TIM1_CCR1 / TIM1_ARR + 1)* 100 = 50%

- The user button is used to change the rising (DTr) and falling (DTf) deadtime as follows:

             -------------------------------                            -------------------------------
            | Step0 | Step1 | Step2 | Step3 |                          | Step0 | Step1 | Step2 | Step3 |
       -------------------------------------           ------------------------------------------------
      |DTr  | 0 us  | 12 us | 16 us | 20 us |         |CH1 duty cycle  |  50%  |  44%  |  42%  |  40%  |
       -------------------------------------           ------------------------------------------------
      |DTf  | 0 us  | 20 us | 16 us | 12 us |         |CH1N duty cycle |  50%  |  40%  |  42%  |  44%  |

- The following figure shows the relationship between the output signals of the deadtime generator
(tim_oc1 & tim_oc1n) and the reference signal tim_oc1ref with the respective DTr and DTf according to step.

                   ___                      ____________________
      tim_oc1ref:     |____________________|                    |__
                      <-------100us-------> <-------100us------->
                   ___                              ____________
      tim_oc1   :     |____________________________|            |__
                                            <-DTr->
                              _____________
      tim_oc1n  :  __________|             |_______________________
                      <-DTf->

- aDT array contains the values to put in the deadtime related bit fields (BDTR.DTG for the rising deadtime
  and DTR2.DTGF for the falling deadtime).  
  The deadtime sampling clock (tDTS) depends on the clock division (CKD) applied on the timer kernel clock (tim_ker_ck).
  In this example the deadtime generator is configured to insert a deadtime ranging from 10240 ns to 20160 ns between
  the complementary channels (see calculation below).

      DTG(F)[7:5] = 111, so the deadtime value is DT = (32 + DTG[4:0]) x tDTG with:  
      tDTS = 4*(1/tim_ker_ck) = 20ns because CR1.CKD=2  
      tDTG = 16 x tDTS = 320ns  

- The generated PWM signals can be displayed using an oscilloscope. The rising and falling deadtime
values can be obtained via cursor measurement.

- LED_RED is ON when there is an error during the configuration sequence.

#### <b>Notes</b>

 1. The deadtime values mentioned above are theoretical (obtained when the system clock frequency is exactly 800 MHz).
    They might be slightly different depending on system clock frequency precision.

 2. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
    based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
    a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
    than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
    To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

 3. The application needs to ensure that the SysTick time base is always set to 1 millisecond
    to have correct HAL operation.

### <b>Keywords</b>

Motor control, TIM, Output, signal, PWM, Deadtime, Oscilloscope, Waveform.

### <b>Sub-project FSBL directory contents</b>

      - TIM/TIM_DeadtimeInsertion/FSBL/Inc/main.h                       Header for main.c module
      - TIM/TIM_DeadtimeInsertion/FSBL/Inc/stm32n6xx_hal_conf.h         HAL Configuration file
      - TIM/TIM_DeadtimeInsertion/FSBL/Inc/stm32n6xx_nucleo_conf.h      BSP configuration file
      - TIM/TIM_DeadtimeInsertion/FSBL/Inc/stm32n6xx_it.h               Interrupt handlers header file
      - TIM/TIM_DeadtimeInsertion/FSBL/Src/main.c                       Main program
      - TIM/TIM_DeadtimeInsertion/FSBL/Src/stm32n6xx_hal_msp.c          HAL MSP module
      - TIM/TIM_DeadtimeInsertion/FSBL/Src/stm32n6xx_it.c               Interrupt handlers
      - TIM/TIM_DeadtimeInsertion/FSBL/Src/system_stm32n6xx_fsbl.c      STM32N6xx system source file

### <b>Hardware and Software environment</b>

  - This example runs on STM32N657X0HxQ devices.

  - This example has been tested with NUCLEO-N657X0-Q board and can be
    easily tailored to any other supported device and development board.

  - NUCLEO-N657X0-Q Set-up
    Connect the following pins to an oscilloscope to monitor the different waveforms:
    - TIM1_CH1  : PE.09 (PIN31 CN15)
    - TIM1_CH1N : PE.08 (PIN15 CN16)

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
