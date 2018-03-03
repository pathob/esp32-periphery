#
# "peripherals" pseudo-component makefile.

SRC_DIRS := $(patsubst $(COMPONENT_PATH)/%/,%,$(sort $(dir $(wildcard $(COMPONENT_PATH)/periph/*/))))
INCLUDE_DIRS := $(patsubst $(COMPONENT_PATH)/%,%,$(sort $(dir $(wildcard $(COMPONENT_PATH)/include/*/*/))))

COMPONENT_SRCDIRS := $(SRC_DIRS)
COMPONENT_ADD_INCLUDEDIRS := include
COMPONENT_PRIV_INCLUDEDIRS := $(INCLUDE_DIRS)
