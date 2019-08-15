#
# This is a project Makefile. It is assumed the directory this Makefile resides in is a
# project subdirectory.
#

PROJECT_NAME := ble_midi_test

COMPONENT_ADD_INCLUDEDIRS := components/include

EXTRA_COMPONENT_DIRS := $(IDF_PATH)/../esp32-snippets/cpp_utils

include $(IDF_PATH)/make/project.mk

