# RISC-V Notes

I have decided to retarget the FromZero project to use RISC-V for the following reasons:
- There are available cheap RISC-V microcontrollers
- RISC-V uses an open ISA, which aligns with my open philosophy
- RISC-V ISA seems much more comprehensible and simple than ARM-Thumb
- Building a project on top of RISC-V is more novel than on top of ARM

## Hardware

### GD32VF103CBT6

Sipeed Longan Nano RISC-V GD32VF103CBT6 MCU Development Board

This MCU implements the RV32IMAC ISA

- Aliexpress: https://www.aliexpress.com/wholesale?catId=0&initiative_id=AS_20201011041932&SearchText=gd32vf103cbt6
- Price: ~ $4
- Specs:
    - 128KB Flash
    - 32KB SRAM
    - Watchdog, RTC, Systick
    - 4 x universal 16-bit timer
    - 2 x basic 16-bit timer
    - 1 x advanced 16-bit timer
    - 3 x USART
    - 2 x I2C
    - 3 x SPI
    - 2 x I2S
    - 2 x CAN
    - 1 x USBFS (OTG)
    - 2 x ADC (10 channel)
    - 2 x DAC
- MCU User Manual: https://raw.githubusercontent.com/riscv-mcu/GD32VF103_DataSheets/master/GD32VF103_User_Manual_EN_V1.0.pdf
- MCU Datas Sheet: https://raw.githubusercontent.com/riscv-mcu/GD32VF103_DataSheets/master/GD32VF103_Datasheet_Rev_1.0.pdf
- CPU Manual: https://raw.githubusercontent.com/nucleisys/Bumblebee_Core_Doc/master/Bumblebee%20Core%20Architecture%20Manual.pdf

## ISA Specification

https://github.com/riscv/riscv-isa-manual/releases/download/Ratified-IMAFDQC/riscv-spec-20191213.pdf

## Simulation

```
sudo pacman -S spike
```

## Programming

- C: https://github.com/esmil/gd32vf103inator
