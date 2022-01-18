@echo off

"\pf\gcc\bin\arm-none-eabi-g++.exe" -mcpu=cortex-m7 -mthumb -Tgcc_arm.ld -fsigned-char -O2 -ffunction-sections -fdata-sections -fno-exceptions -Wno-address-of-packed-member -DSTM32F746xx -Iinc *.cpp *.c *.s

if errorlevel 1 goto end
"\pf\gcc\bin\arm-none-eabi-objcopy.exe" -O ihex a.out firmware.hex

"I:\PF\STM32CubeProgrammer\bin\STM32_Programmer_CLI.exe" -c port=SWD -w firmware.hex -rst -run

:end