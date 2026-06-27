Q ?= @
CC = arm-none-eabi-gcc
CXX = arm-none-eabi-g++
BUILD_DIR = target
NWLINK = npx --yes -- nwlink@0.0.19
LINK_GC = 1
LTO = 1

define object_for
$(addprefix $(BUILD_DIR)/,$(addsuffix .o,$(basename $(1))))
endef

source = $(addprefix source/,\
  NumworksHost.cpp \
)

CPPFLAGS = -std=c++11 -fno-exceptions
CPPFLAGS += -Os -Wall
CPPFLAGS += $(shell $(NWLINK) eadk-cflags-device)
LDFLAGS = -Wl,--relocatable
LDFLAGS += -nostartfiles
LDFLAGS += --specs=nano.specs

ifeq ($(LINK_GC),1)
CPPFLAGS += -fdata-sections -ffunction-sections
LDFLAGS += -Wl,-e,main -Wl,-u,eadk_app_name -Wl,-u,eadk_app_icon -Wl,-u,eadk_api_level
LDFLAGS += -Wl,--gc-sections
endif

ifeq ($(LTO),1)
CPPFLAGS += -flto -fno-fat-lto-objects
CPPFLAGS += -fwhole-program
CPPFLAGS += -fvisibility=internal
LDFLAGS += -flinker-output=nolto-rel
endif

.PHONY: build
build: $(BUILD_DIR)/fake08.bin

.PHONY: run
run: $(BUILD_DIR)/fake08.nwa
	@echo "INSTALL $<"
	$(Q) $(NWLINK) install-nwa $<

$(BUILD_DIR)/%.bin: $(BUILD_DIR)/%.nwa
	@echo "BIN     $@"
	$(Q) $(NWLINK) nwa-bin $< $@

$(BUILD_DIR)/fake08.nwa: $(call object_for,$(source)) $(BUILD_DIR)/icon.o
	@echo "LD      $@"
	$(Q) $(CC) $(CPPFLAGS) $(LDFLAGS) $^ -o $@

$(addprefix $(BUILD_DIR)/,%.o): %.cpp | $(BUILD_DIR)
	@echo "CXX     $^"
	$(Q) $(CXX) $(CPPFLAGS) $(SFLAGS) -c $^ -o $@

$(BUILD_DIR)/icon.o: source/icon.png
	@echo "ICON    $<"
	$(Q) $(NWLINK) png-icon-o $< $@

.PRECIOUS: $(BUILD_DIR)
$(BUILD_DIR):
	$(Q) mkdir -p $@/source

.PHONY: clean
clean:
	@echo "CLEAN"
	$(Q) rm -rf $(BUILD_DIR)
