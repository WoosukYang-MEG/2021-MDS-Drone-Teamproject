################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include Drivers/STM32F4xx_HAL_Driver/Src/subdir.mk
-include Core/Startup/subdir.mk
-include Core/Src/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
2-2.\ ICM20602.elf \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
2-2.\ ICM20602.list \


# All Target
all: 2-2.\ ICM20602.elf secondary-outputs

# Tool invocations
2-2.\ ICM20602.elf: $(OBJS) $(USER_OBJS) C:\Users\owner\STM32CubeIDE\workspace_1.1.0\2-2.\ ICM20602\STM32F405RGTX_FLASH.ld
	arm-none-eabi-gcc -o "2-2. ICM20602.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m4 -T"C:\Users\owner\STM32CubeIDE\workspace_1.1.0\2-2. ICM20602\STM32F405RGTX_FLASH.ld" --specs=nosys.specs -Wl,-Map="2-2. ICM20602.map" -Wl,--gc-sections -static -u _printf_float --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -Wl,--start-group -lc -lm -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

2-2.\ ICM20602.list: $(EXECUTABLES)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "2-2. ICM20602.list"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) *
	-@echo ' '

secondary-outputs: $(SIZE_OUTPUT) $(OBJDUMP_LIST)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
