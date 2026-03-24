#
# Makefile
#

arch := arm

ifeq ($(arch), arm)
CC              := arm-linux-gnueabihf-gcc
CXX             := arm-linux-gnueabihf-g++
else
CC              := gcc
CXX             := g++
endif


LVGL_DIR_NAME   ?= lvgl
LVGL_DIR        ?= .

CFLAGS          ?= -O3 -g -I$(LVGL_DIR)/
LDFLAGS         ?= -lm -lpthread


ifeq ($(arch), arm)

ifneq ($(shell grep -m1 '^\#define LV_USE_TSLIB ' lv_conf.h | awk '{print $$3}'),0)
TSLIB_CFLAGS    := $(shell pkg-config --cflags ./libs/tslib/lib/pkgconfig/tslib.pc)
TSLIB_LIBS      := $(shell pkg-config --libs ./libs/tslib/lib/pkgconfig/tslib.pc)
endif

CFLAGS += $(TSLIB_CFLAGS)
LDFLAGS += $(TSLIB_LIBS)

else
# Auto-detect enabled backends from lv_conf.h and add flags
ifneq ($(shell grep -m1 '^\#define LV_USE_SDL ' lv_conf.h | awk '{print $$3}'),0)
SDL_CFLAGS      := $(shell pkg-config --cflags sdl2)
SDL_LDFLAGS     := $(shell pkg-config --libs sdl2)
endif

ifneq ($(shell grep -m1 '^\#define LV_USE_EVDEV ' lv_conf.h | awk '{print $$3}'),0)
EVDEV_CFLAGS    := $(shell pkg-config --cflags libevdev)
EVDEV_LDFLAGS   := $(shell pkg-config --libs libevdev)
endif

CFLAGS += $(SDL_CFLAGS) $(EVDEV_CFLAGS)
LDFLAGS += $(SDL_LDFLAGS) $(EVDEV_LDFLAGS)

endif


BIN             = main
BUILD_DIR       = ./build
BUILD_OBJ_DIR   = $(BUILD_DIR)/obj
BUILD_BIN_DIR   = $(BUILD_DIR)/bin

prefix          ?= /usr
bindir          ?= $(prefix)/bin

# Collect app source files (root + src)
CSRCS       	+= $(wildcard *.c)
CXXSRCS    		+= $(wildcard *.cpp)
CSRCS           += $(shell find src -type f -name '*.c')
CXXSRCS         += $(shell find src -type f -name '*.cpp')

# Include LVGL sources
include $(LVGL_DIR)/lvgl/lvgl.mk

OBJEXT          ?= .o

COBJS           = $(CSRCS:.c=$(OBJEXT))
CXXOBJS         = $(CXXSRCS:.cpp=$(OBJEXT))
AOBJS           = $(ASRCS:.S=$(OBJEXT))

SRCS            = $(ASRCS) $(CSRCS) $(CXXSRCS)
OBJS            = $(AOBJS) $(COBJS) $(CXXOBJS)
TARGET          = $(addprefix $(BUILD_OBJ_DIR)/, $(patsubst ./%, %, $(OBJS)))

all: default

$(BUILD_OBJ_DIR)/%.o: %.c lv_conf.h
	@mkdir -p $(dir $@)
	@$(CC)  $(CFLAGS) -c $< -o $@
	@echo "CC  $<"

$(BUILD_OBJ_DIR)/%.o: %.cpp lv_conf.h
	@mkdir -p $(dir $@)
	@$(CXX)  $(CFLAGS) -c $< -o $@
	@echo "CXX $<"

$(BUILD_OBJ_DIR)/%.o: %.S lv_conf.h
	@mkdir -p $(dir $@)
	@$(CC)  $(CFLAGS) -c $< -o $@
	@echo "AS  $<"

default: $(TARGET)
	@mkdir -p $(dir $(BUILD_BIN_DIR)/)
	$(CXX) -o $(BUILD_BIN_DIR)/$(BIN) $(TARGET) $(LDFLAGS)

clean:
	rm -rf $(BUILD_DIR)

install:
	install -d $(DESTDIR)$(bindir)
	install $(BUILD_BIN_DIR)/$(BIN) $(DESTDIR)$(bindir)

uninstall:
	$(RM) -r $(addprefix $(DESTDIR)$(bindir)/,$(BIN))

