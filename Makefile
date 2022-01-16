APPLICATION = cayenne

BOARD ?= lora-e5-dev

RIOTBASE ?= /home/gauthier/iot-polytech/riot/RIOT


#include $(RIOTBASE)/tests/Makefile.tests_common

# Default LoRa region is Europe and default band is 868MHz
LORA_REGION ?= EU868

USEMODULE += periph_gpio

DRIVER ?= scd30
USEMODULE += xtimer
USEMODULE += sx126x_stm32wl

USEMODULE += $(DRIVER)
USEMODULE += printf_float

USEPKG += semtech-loramac
USEMODULE += auto_init_loramac
USEPKG += cayenne-lpp

USEMODULE += pir

TEST_ITERATIONS ?= 10
# export iterations for continuous measurements
CFLAGS += -DTEST_ITERATIONS=$(TEST_ITERATIONS)

include $(RIOTBASE)/Makefile.include
