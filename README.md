# Edge Impulse STM32N6 Application

Firmware project based on the STMicroElectronics STM32N6570-DK board. The firmware includes following both ingestion and inference code.


## Ingestion
Data sampling and uploading directly into Edge Impulse Studio. Connect the board over the `STLINK USB` to your computer and run the Edge Impulse CLI tools.
Follow the [guide](https://docs.edgeimpulse.com/docs/edge-ai-hardware/mcu-+-ai-accelerators/stm32n6570-dk#id-4.-connecting-cli-to-development-kit) for the step-by-step procedure.

## Inference
Run an Edge Impulse model on ST hardware. This firmware projects ships with an object detection model detecting: `Persons`, `EI logos` and `ST logos`. 
To start inference use the CLI tool `edge-impulse-run-impulse`. This will start inference and shows bounding boxes / labels on the display and detailed info in the terminal.
Starting the tool in debug mode: `edge-impulse-run-impulse --debug`, opens a webserver where you can view the results in your webbrowser.

Follow one of our [end-to-end tuturials](https://docs.edgeimpulse.com/docs/tutorials/end-to-end-tutorials/computer-vision/object-detection/object-detection) to build and deploy your own model.

## Hardware Support

- MB1939 STM32N6570-DK REV C01, B01, A03 and A01 board
  - The board should be connected to the onboard ST-LINK debug adapter CN6 with
a __USB-C to USB-C to ensure sufficient power__
- __Set the boot mode in dev mode__ (BOOT1 switch to right position.
BOOT0 switch position doesn't matter)
  - OTP fuses are set in this example for xSPI IOs in order to get the maximum speed (200MHz) on xSPI interfaces.
- 5 Cameras are supported through C Defines:
  - MB1854B IMX335 (Default Camera provided with the MB1939 STM32N6570-DK board)
  - OV5640 Camera module
  - ST VD66GY Camera module
  - ST VD55G1 Camera module
  - ST VD1941 Camera module

## Tools version

- IAR Embedded Workbench for Arm (EWARM) 9.40.1 + N6 patch (**EWARMv9_STM32N6xx_V0.6.0**)
- STM32CubeIDE (**STM32CubeIDE 1.15.1.24-N6-A2**)
- STM32CubeProgrammer (**v0.0.2-N6-B02**)
- STM32Cube.AI (**v10.0.0-A7**)

## Boot modes

The STM32N6 does not have any internal flash. To retain your firmware after a reboot, you must program it in the external flash. Alternatively, you can load your firmware directly from SRAM (dev mode). However, in dev mode if you turn off the board, your program will be lost.

__Boot modes:__
- Dev mode: load firmware from debug session in RAM (boot switch to the right)
- Boot from flash: Program firmware in external flash (boot switch to the left)

## Quickstart using prebuilt binaries

### Flash prebuilt binaries using STM32CubeProgrammer

Three binaries must be programmed in the board external flash using the following procedure:

  1. Switch BOOT1 switch to right position
  2. Program `Upload/ai_fsbl_cut_2_0.hex` (To be done once) (First stage boot loader) (STM32N6570-DK-REV-C01)
  3. Program `Upload/network_data.hex` (params of the networks; To be changed only when the network is changed)
  4. Program `Upload/Project.hex` (firmware application)
  5. Switch BOOT1 switch to Left position
  6. Power down / up sequence

__Note__: The `Upload/Project.hex` firmware is built for MB1939 STM32N6570-DK REV C01 with IMX335 Camera module.
__Note__: The `Upload/ai_fsbl_cut_2_0.hex` firmware is built for MB1939 STM32N6570-DK REV C01. For previous board please use `Binary/ai_fsbl_cut_1_1.hex`. For the app binaries you need to build them.

### Flash prebuilt binaries using included scripts

Alternatively to using the STM32CubeProgrammer directly, the scripts the `Upload` folder can be used. Instructions can be found in the [Readme](Upload/README.md)

### How to Program hex files using Command line

Make sure to have the STM32CubeProgrammer bin folder added in your path.

```bash
export DKEL="<STM32CubeProgrammer_N6 Install Folder>/bin/ExternalLoader/MX66UW1G45G_STM32N6570-DK.stldr"

# First Stage Boot loader
STM32_Programmer_CLI -c port=SWD mode=HOTPLUG ap=1 -el $DKEL -hardRst -w Upload/ai_fsbl_cut_2_0.hex

# Network parameters and biases
STM32_Programmer_CLI -c port=SWD mode=HOTPLUG ap=1 -el $DKEL -hardRst -w Upload/network_data.hex

# Application Firmware
STM32_Programmer_CLI -c port=SWD mode=HOTPLUG ap=1 -el $DKEL -hardRst -w Upload/Project.hex
```

## Quickstart using source code

Before building and running the application you have to program `network_data.hex` (model weights and biases).

This step only has to be done once unless you change AI model.
See [Quickstart using prebuilt binaries](#quickstart-using-prebuilt-binaries) for details.

More information about boot modes is available at [Boot Overview](Doc/Boot-Overview.md)

### Application build and run - Dev mode

__Make sure to have the switch to the right side.__

#### STM32CubeIDE

Double click on `STM32CubeIDE/.project` to open project in STM32CubeIDE. Build and run with build and run buttons.

__Known issue__: Some warnings are interpreted as error (displayed in red). Build is still successful

#### IAR EWARM

Double click on `EWARM/Project.eww` to open project in IAR IDE. Build and run with build and run buttons.

#### Makefile

Before running commands below be sure to have commands in your PATH.

1. Build project using the provided `Makefile`:

```bash
make -j8
```

2. Open a gdb server connected to the STM32 target:

```bash
ST-LINK_gdbserver -p 61234 -l 1 -d -s -cp <path-to-stm32cubeprogramer-bin-dir> -m 1 -g
```

3. In a separate terminal session launch a GDB session to load the firmware image into the device memory:

```bash
$ arm-none-eabi-gdb build/Project.elf
(gdb) target remote :61234
(gdb) monitor reset
(gdb) load
(gdb) continue
```

### Application build and run - Boot from flash

__Make sure to have the switch to the right side.__

#### STM32CubeIDE

Double click on `STM32CubeIDE/.project` to open project in STM32CubeIDE. Build and run with build and run buttons.

__Known issue__: Some warnings are interpreted as error (displayed in red). Build is still successful

#### IAR EWARM

Double click on `EWARM/Project.eww` to open project in IAR IDE. Build and run with build and run buttons.

#### Makefile

Before running commands below be sure to have commands in your PATH.

1. Build project using the provided `Makefile`:

```bash
make -j8
```

Once your app is built with Makefile, STM32CubeIde or EWARM. You can program the bin file at the address `0x70080000`;

```bash
export DKEL="<STM32CubeProgrammer_N6 Install Folder>/bin/ExternalLoader/MX66UW1G45G_STM32N6570-DK.stldr"

# Adapt build path to your IDE
STM32_Programmer_CLI -c port=SWD mode=HOTPLUG ap=1 -el $(EL) -hardRst -w build/Project.bin 0x70080000
```

Note: Only the App binary needs to be programmed if the fsbl and network_data.hex was previously programmed.

## Known Issues and Limitations

- Disable D-Cache for debug (Hardware issue related to debugger cache visibility with Cut 1.1).
- NN_WIDTH and NN_HEIGHT must be a multiple of 16.
- NN_WIDTH and NN_HEIGHT must be equal.
- Only RGB888 format for nn input has been tested.
