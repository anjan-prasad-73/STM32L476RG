################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Src/exti_gpio.c \
../Src/gpio_driver.c \
../Src/main.c \
../Src/syscalls.c \
../Src/sysmem.c \
../Src/system_clock.c 

OBJS += \
./Src/exti_gpio.o \
./Src/gpio_driver.o \
./Src/main.o \
./Src/syscalls.o \
./Src/sysmem.o \
./Src/system_clock.o 

C_DEPS += \
./Src/exti_gpio.d \
./Src/gpio_driver.d \
./Src/main.d \
./Src/syscalls.d \
./Src/sysmem.d \
./Src/system_clock.d 


# Each subdirectory must supply rules for building sources it contributes
Src/%.o Src/%.su Src/%.cyclo: ../Src/%.c Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DSTM32L4 -DSTM32 -DNUCLEO_L476RG -DSTM32L476RGTx -c -I../Inc -I"D:/BITSILICA_WORKSHEETS/Drivers/Drivers/CMSIS/Device/ST/STM32L4xx/Include" -I"D:/BITSILICA_WORKSHEETS/Drivers/Drivers/CMSIS/Include" -I"D:/BITSILICA_WORKSHEETS/Drivers/Drivers/STM32L4xx_HAL_Driver/Inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Src

clean-Src:
	-$(RM) ./Src/exti_gpio.cyclo ./Src/exti_gpio.d ./Src/exti_gpio.o ./Src/exti_gpio.su ./Src/gpio_driver.cyclo ./Src/gpio_driver.d ./Src/gpio_driver.o ./Src/gpio_driver.su ./Src/main.cyclo ./Src/main.d ./Src/main.o ./Src/main.su ./Src/syscalls.cyclo ./Src/syscalls.d ./Src/syscalls.o ./Src/syscalls.su ./Src/sysmem.cyclo ./Src/sysmem.d ./Src/sysmem.o ./Src/sysmem.su ./Src/system_clock.cyclo ./Src/system_clock.d ./Src/system_clock.o ./Src/system_clock.su

.PHONY: clean-Src

