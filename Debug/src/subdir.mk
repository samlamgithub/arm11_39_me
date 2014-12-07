################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
O_SRCS += \
../src/assemble.o \
../src/emulate.o 

C_SRCS += \
../src/assemble.c \
../src/emulate.c \
../src/emulate1.c 

OBJS += \
./src/assemble.o \
./src/emulate.o \
./src/emulate1.o 

C_DEPS += \
./src/assemble.d \
./src/emulate.d \
./src/emulate1.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


