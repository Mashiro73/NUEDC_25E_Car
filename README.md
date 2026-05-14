# NUEDC25-E — Smart Car

A line-following smart car the **2025 National Undergraduate Electronic Design Competition (NUEDC)**, Topic E.

## Hardware

- **MCU**: TI MSPM0G3507 (ARM Cortex-M0+, 80 MHz)
- **IMU**: InvenSense ICM45686 (6-axis, SPI)
- **Display**: 1.8" TFT LCD (ST7735S, 128x160, SPI)
- **Motor Driver**: Dual H-bridge with PWM + direction GPIO
- **Encoders**: Quadrature encoders on both wheels (GPIO interrupts)
- **Line Tracking**: 8-channel photoelectric sensor array
- **Communication**: UART debug console + UART telemetry to companion MCU
- **User Input**: 4 push buttons (S1-S4)
- **Additional**: RGB LED, buzzer

## Project Structure

```
NUEDC25/
├── firmware/
│   ├── main.c                    # Main application (state machine, SysTick handler)
│   ├── main.h                    # Shared macros, state enum, global declarations
│   ├── NUED25_CAR.syscfg         # TI SysConfig hardware configuration
│   ├── ti_msp_dl_config.c/.h     # Auto-generated driverlib config
│   ├── drivers/
│   │   ├── lcd/                  # ST7735S LCD driver (SPI, drawing primitives)
│   │   ├── motor/                # Motor driver (PWM, encoder, PID speed control)
│   │   ├── line_sensor/          # 8-channel line-tracking sensor array
│   │   ├── key/                  # Non-blocking key state machine with debounce
│   │   └── imu/                  # ICM45686 IMU driver + Madgwick AHRS filter
│   ├── control/
│   │   └── pid.c/.h              # Incremental & positional PID controller
│   └── platform/
│       └── clock.c/.h            # Delay functions, timer utilities
```

## Prerequisites

- [TI MSPM0 SDK](https://www.ti.com/tool/MSPM0-SDK) v2.00.01.00 or later
- [Keil MDK](https://www.keil.com/demo/eval/arm.htm) (ARM Compiler 6)
- [TI SysConfig](https://www.ti.com/tool/SYSCONFIG) v1.20+

## Build

1. Install the TI MSPM0 SDK to a local directory (default: `D:\ti\mspm0_sdk_2_00_01_00`)
2. Open `firmware/MDK-ARM/NUED25_CAR.uvprojx` in Keil MDK
3. In **Project → Options → C/C++ → Include Paths**, add:
   - `<SDK_PATH>/source/third_party/CMSIS/Core/Include`
   - `<SDK_PATH>/source`
4. In **Project → Options → Linker → Misc**, add:
   - `<SDK_PATH>/source/ti/driverlib/lib/keil/m0p/mspm0g1x0x_g3x0x/driverlib.a`
5. Build and flash

## Pin Mapping

| Function      | Pin            | Peripheral |
| ------------- | -------------- | ---------- |
| Motor PWM L   | PA23           | TIMG7 C0   |
| Motor PWM R   | PA18           | TIMG7 C1   |
| Motor Dir L A | PA21           | GPIO       |
| Motor Dir L B | PA22           | GPIO       |
| Motor Dir R A | PB10           | GPIO       |
| Motor Dir R B | PB8            | GPIO       |
| Encoder L A/B | PA8/PA9        | GPIO INT   |
| Encoder R A/B | PB13/PB2       | GPIO INT   |
| LCD SCLK      | PA17           | SPI1       |
| LCD MOSI      | PB15           | SPI1       |
| LCD MISO      | PB14           | SPI1       |
| IMU SCLK      | PA12           | SPI0       |
| IMU MOSI      | PA14           | SPI0       |
| IMU MISO      | PA13           | SPI0       |
| UART Debug    | PA10/PA11      | UART0      |
| UART STM32    | PA25/PB12      | UART3      |
| RGB LED       | PA26/PA27/PA28 | GPIO       |
| Buzzer        | PB27           | GPIO       |
| Key S1        | PB24           | GPIO       |
| Key S2        | PB21           | GPIO       |
| Line Sensors  | PA0-PA7        | GPIO       |

## License

MIT License - see [LICENSE](LICENSE) for details.

## Author

Dinglei Xu
