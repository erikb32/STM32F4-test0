PROJECT = f4test

# Should come from environment
ARM_CC_ROOT = /opt/arm-gcc-13

# Compiler and other tools
CC = $(ARM_CC_ROOT)/bin/arm-none-eabi-gcc
OBJCOPY = $(ARM_CC_ROOT)/bin/arm-none-eabi-objcopy
SIZE = $(ARM_CC_ROOT)/bin/arm-none-eabi-size
OBJDUMP = $(ARM_CC_ROOT)/bin/arm-none-eabi-objdump

SRC_DIR   = src/
LIB_DIR   = lib/
BUILD_DIR = build/

TARGET = $(BUILD_DIR)$(PROJECT)

# Project sources
SRC_FILES = $(wildcard $(SRC_DIR)*.c) $(wildcard $(SRC_DIR)*/*.c)
ASM_FILES = $(wildcard $(SRC_DIR)*.s) $(wildcard $(SRC_DIR)*/*.s)
LD_SCRIPT = linker/STM32F412ZGJX_FLASH.ld

# Project includes
INCLUDES  = -I$(SRC_DIR)
INCLUDES += -I$(SRC_DIR)hw/
INCLUDES += -I$(SRC_DIR)sw/
INCLUDES += -I$(SRC_DIR)common/
INCLUDES += -I$(SRC_DIR)jpeg/
INCLUDES += -I$(SRC_DIR)usb/

# Vendor sources
SRC_FILES += $(LIB_DIR)Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal.c
SRC_FILES += $(LIB_DIR)Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_cortex.c
SRC_FILES += $(LIB_DIR)Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma.c
SRC_FILES += $(LIB_DIR)Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash.c
SRC_FILES += $(LIB_DIR)Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash_ex.c
SRC_FILES += $(LIB_DIR)Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_gpio.c
SRC_FILES += $(LIB_DIR)Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_i2c.c
SRC_FILES += $(LIB_DIR)Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pcd.c
SRC_FILES += $(LIB_DIR)Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pcd_ex.c
SRC_FILES += $(LIB_DIR)Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc.c
SRC_FILES += $(LIB_DIR)Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc_ex.c
SRC_FILES += $(LIB_DIR)Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_sram.c
SRC_FILES += $(LIB_DIR)Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_tim.c
SRC_FILES += $(LIB_DIR)Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_uart.c
SRC_FILES += $(LIB_DIR)Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_ll_fsmc.c
SRC_FILES += $(LIB_DIR)Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_ll_usb.c
SRC_FILES += $(LIB_DIR)Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_core.c
SRC_FILES += $(LIB_DIR)Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_ctlreq.c
SRC_FILES += $(LIB_DIR)Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_ioreq.c
SRC_FILES += $(LIB_DIR)Middlewares/ST/STM32_USB_Device_Library/Class/MSC/Src/usbd_msc.c
SRC_FILES += $(LIB_DIR)Middlewares/ST/STM32_USB_Device_Library/Class/MSC/Src/usbd_msc_bot.c
SRC_FILES += $(LIB_DIR)Middlewares/ST/STM32_USB_Device_Library/Class/MSC/Src/usbd_msc_data.c
SRC_FILES += $(LIB_DIR)Middlewares/ST/STM32_USB_Device_Library/Class/MSC/Src/usbd_msc_scsi.c
SRC_FILES += $(LIB_DIR)Middlewares/Third_Party/FatFs/src/ff.c
SRC_FILES += $(LIB_DIR)Middlewares/Third_Party/FatFs/src/ff_gen_drv.c
SRC_FILES += $(LIB_DIR)Middlewares/Third_Party/FatFs/src/diskio.c
SRC_FILES += $(LIB_DIR)Middlewares/Third_Party/LibJPEG/source/jdapimin.c
SRC_FILES += $(LIB_DIR)Middlewares/Third_Party/LibJPEG/source/jdapistd.c
SRC_FILES += $(LIB_DIR)Middlewares/Third_Party/LibJPEG/source/jdatasrc.c
SRC_FILES += $(LIB_DIR)Middlewares/Third_Party/LibJPEG/source/jcomapi.c
SRC_FILES += $(LIB_DIR)Middlewares/Third_Party/LibJPEG/source/jerror.c
SRC_FILES += $(LIB_DIR)Middlewares/Third_Party/LibJPEG/source/jmemmgr.c
SRC_FILES += $(LIB_DIR)Middlewares/Third_Party/LibJPEG/source/jdmarker.c
SRC_FILES += $(LIB_DIR)Middlewares/Third_Party/LibJPEG/source/jdinput.c
SRC_FILES += $(LIB_DIR)Middlewares/Third_Party/LibJPEG/source/jdmaster.c
SRC_FILES += $(LIB_DIR)Middlewares/Third_Party/LibJPEG/source/jmemnobs.c
SRC_FILES += $(LIB_DIR)Middlewares/Third_Party/LibJPEG/source/jutils.c
SRC_FILES += $(LIB_DIR)Middlewares/Third_Party/LibJPEG/source/jquant1.c
SRC_FILES += $(LIB_DIR)Middlewares/Third_Party/LibJPEG/source/jquant2.c
SRC_FILES += $(LIB_DIR)Middlewares/Third_Party/LibJPEG/source/jddctmgr.c
SRC_FILES += $(LIB_DIR)Middlewares/Third_Party/LibJPEG/source/jdarith.c
SRC_FILES += $(LIB_DIR)Middlewares/Third_Party/LibJPEG/source/jdcoefct.c
SRC_FILES += $(LIB_DIR)Middlewares/Third_Party/LibJPEG/source/jdcolor.c
SRC_FILES += $(LIB_DIR)Middlewares/Third_Party/LibJPEG/source/jdsample.c
SRC_FILES += $(LIB_DIR)Middlewares/Third_Party/LibJPEG/source/jdpostct.c
SRC_FILES += $(LIB_DIR)Middlewares/Third_Party/LibJPEG/source/jdmainct.c
SRC_FILES += $(LIB_DIR)Middlewares/Third_Party/LibJPEG/source/jdhuff.c
SRC_FILES += $(LIB_DIR)Middlewares/Third_Party/LibJPEG/source/jdmerge.c
SRC_FILES += $(LIB_DIR)Middlewares/Third_Party/LibJPEG/source/jidctint.c
SRC_FILES += $(LIB_DIR)Middlewares/Third_Party/LibJPEG/source/jidctfst.c
SRC_FILES += $(LIB_DIR)Middlewares/Third_Party/LibJPEG/source/jidctflt.c
SRC_FILES += $(LIB_DIR)Middlewares/Third_Party/LibJPEG/source/jaricom.c
# Board sources
SRC_FILES += $(LIB_DIR)Drivers/BSP/STM32412G-Discovery/stm32412g_discovery.c
SRC_FILES += $(LIB_DIR)Drivers/BSP/STM32412G-Discovery/stm32412g_discovery_lcd.c
SRC_FILES += $(LIB_DIR)Drivers/BSP/STM32412G-Discovery/stm32412g_discovery_ts.c
SRC_FILES += $(LIB_DIR)Drivers/BSP/Components/st7789h2/st7789h2.c
SRC_FILES += $(LIB_DIR)Drivers/BSP/Components/ls016b8uy/ls016b8uy.c
SRC_FILES += $(LIB_DIR)Drivers/BSP/Components/ft6x06/ft6x06.c
SRC_FILES += $(LIB_DIR)Drivers/BSP/Components/ft3x67/ft3x67.c

#Vendor includes
INCLUDES += -I$(LIB_DIR)Drivers/CMSIS/Include
INCLUDES += -I$(LIB_DIR)Drivers/CMSIS/Core/Include
INCLUDES += -I$(LIB_DIR)Drivers/CMSIS/Device/ST/STM32F4xx/Include
INCLUDES += -I$(LIB_DIR)Drivers/STM32F4xx_HAL_Driver/Inc
INCLUDES += -I$(LIB_DIR)Middlewares/ST/STM32_USB_Device_Library/Core/Inc
INCLUDES += -I$(LIB_DIR)Middlewares/ST/STM32_USB_Device_Library/Class/MSC/Inc
INCLUDES += -I$(LIB_DIR)Middlewares/Third_Party/FatFs/src
INCLUDES += -I$(LIB_DIR)Middlewares/Third_Party/LibJPEG/include
INCLUDES += -I$(LIB_DIR)Utilities/Fonts
# Board includes
INCLUDES += -I$(LIB_DIR)Drivers/BSP/STM32412G-Discovery

# Compiler Flags
CFLAGS = \
-std=gnu99 \
-O2 \
-Wall \
-Wextra \
-Wshadow \
-Wstrict-overflow=5 \
-mthumb \
-mcpu=cortex-m4 \
-mfpu=fpv4-sp-d16 \
-mfloat-abi=hard \
-ffunction-sections \
-fdata-sections \
-fstack-usage \
-DSTM32F412Zx \

CFLAGS += $(INCLUDES)

# Linker Flags
LFLAGS = \
-Wl,-Map="$(BUILD_DIR)$(PROJECT).map" \
-Wl,--gc-sections \
-Wl,--print-memory-usage \
-Wl,-T$(LD_SCRIPT) \
--specs=nano.specs

###############################################################################

OBJ_DIR := $(BUILD_DIR)
C_OBJS = $(addprefix $(OBJ_DIR), $(patsubst %.c, %.o, $(SRC_FILES)))
ASM_OBJS = $(addprefix $(OBJ_DIR), $(patsubst %.s, %.o, $(ASM_FILES)))

ALL_OBJS = $(ASM_OBJS) $(C_OBJS)

DEP_FLAGS = -MT $@ -MD -MP -MF $(BUILD_DIR)$*.d
DEPS = $(addprefix $(BUILD_DIR), $(patsubst %.c, %.d, $(SRC_FILES)))

###############################################################################

all: build
	$(SIZE) "$(TARGET).elf"
	@$(OBJCOPY) -O binary "$(TARGET).elf" "$(TARGET).bin"
	@$(OBJCOPY) -O ihex "$(TARGET).elf" "$(TARGET).hex"
	@$(OBJDUMP) -h -S "$(TARGET).elf" > "$(TARGET).lss"

build: $(TARGET)

# Compile
$(OBJ_DIR)%.o: %.c
	@echo "[CC] $@"
	@mkdir -p $(@D)
	@$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)%.o: %.s
	@echo "[CC] $@"
	@mkdir -p $(@D)
	@$(CC) $(CFLAGS) -c $< -o $@

# Link
$(TARGET): $(ALL_OBJS)
	@echo "[LD] $@"
	@$(CC) $(CFLAGS) $(LFLAGS) $(ALL_OBJS) -o "$(TARGET).elf"

# Clean
.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)
