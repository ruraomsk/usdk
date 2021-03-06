################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/freertos.c \
../Core/Src/main.c \
../Core/Src/stm32h7xx_hal_msp.c \
../Core/Src/stm32h7xx_hal_timebase_tim.c \
../Core/Src/stm32h7xx_it.c \
../Core/Src/syscalls.c \
../Core/Src/sysmem.c \
../Core/Src/system_stm32h7xx.c 

OBJS += \
./Core/Src/freertos.o \
./Core/Src/main.o \
./Core/Src/stm32h7xx_hal_msp.o \
./Core/Src/stm32h7xx_hal_timebase_tim.o \
./Core/Src/stm32h7xx_it.o \
./Core/Src/syscalls.o \
./Core/Src/sysmem.o \
./Core/Src/system_stm32h7xx.o 

C_DEPS += \
./Core/Src/freertos.d \
./Core/Src/main.d \
./Core/Src/stm32h7xx_hal_msp.d \
./Core/Src/stm32h7xx_hal_timebase_tim.d \
./Core/Src/stm32h7xx_it.d \
./Core/Src/syscalls.d \
./Core/Src/sysmem.d \
./Core/Src/system_stm32h7xx.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/%.o: ../Core/Src/%.c Core/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32H743xx -c -I../Core/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../Drivers/CMSIS/Include -I"/home/rura/STM32CubeIDE/workspace_1.7.0/STM32H743_Nucleo_ETH/DebugLogger" -I"/home/rura/STM32CubeIDE/workspace_1.7.0/STM32H743_Nucleo_ETH/DeviceLogger" -I"/home/rura/STM32CubeIDE/workspace_1.7.0/STM32H743_Nucleo_ETH/DeviceTime" -I"/home/rura/STM32CubeIDE/workspace_1.7.0/STM32H743_Nucleo_ETH/libmodbus" -I"/home/rura/STM32CubeIDE/workspace_1.7.0/STM32H743_Nucleo_ETH/MobusTCP" -I"/home/rura/STM32CubeIDE/workspace_1.7.0/STM32H743_Nucleo_ETH/Transport" -I"/home/rura/STM32CubeIDE/workspace_1.7.0/STM32H743_Nucleo_ETH/FatFs" -I../LWIP/App -I../LWIP/Target -I../Middlewares/Third_Party/LwIP/src/include -I../Middlewares/Third_Party/LwIP/system -I../Drivers/BSP/Components/lan8742 -I../Middlewares/Third_Party/LwIP/src/include/netif/ppp -I../Middlewares/Third_Party/LwIP/src/include/lwip -I../Middlewares/Third_Party/LwIP/src/include/lwip/apps -I../Middlewares/Third_Party/LwIP/src/include/lwip/priv -I../Middlewares/Third_Party/LwIP/src/include/lwip/prot -I../Middlewares/Third_Party/LwIP/src/include/netif -I../Middlewares/Third_Party/LwIP/src/include/compat/posix -I../Middlewares/Third_Party/LwIP/src/include/compat/posix/arpa -I../Middlewares/Third_Party/LwIP/src/include/compat/posix/net -I../Middlewares/Third_Party/LwIP/src/include/compat/posix/sys -I../Middlewares/Third_Party/LwIP/src/include/compat/stdc -I../Middlewares/Third_Party/LwIP/system/arch -I"/home/rura/STM32CubeIDE/workspace_1.7.0/STM32H743_Nucleo_ETH/Cameras" -I"/home/rura/STM32CubeIDE/workspace_1.7.0/STM32H743_Nucleo_ETH/Files" -I"/home/rura/STM32CubeIDE/workspace_1.7.0/STM32H743_Nucleo_ETH/Middlewares/ST/STM32_USB_Device_Library/Core/Inc" -I"/home/rura/STM32CubeIDE/workspace_1.7.0/STM32H743_Nucleo_ETH/Middlewares/ST/STM32_USB_Device_Library/Class/MSC/Inc" -I"/home/rura/STM32CubeIDE/workspace_1.7.0/STM32H743_Nucleo_ETH/CommonData" -I"/home/rura/STM32CubeIDE/workspace_1.7.0/STM32H743_Nucleo_ETH/core_JSON" -I"/home/rura/STM32CubeIDE/workspace_1.7.0/STM32H743_Nucleo_ETH/Technology" -I../USB_DEVICE/App -I../USB_DEVICE/Target -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -I"/home/rura/STM32CubeIDE/workspace_1.7.0/STM32H743_Nucleo_ETH/Shell" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

