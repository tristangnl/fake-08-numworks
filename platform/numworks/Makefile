#---------------------------------------------------------------------------------
# numworks linux simulator config
#---------------------------------------------------------------------------------

PLATFORM ?= device
LINUX_SIMULATOR_PATH ?= ../../../epsilon/epsilon/external_apps/epsilon_simulators/linux/epsilon.bin

ifeq ($(filter device simulator,$(PLATFORM)),)
$(error PLATFORM must be either 'device' or 'simulator', got '$(PLATFORM)')
endif

#---------------------------------------------------------------------------------
# TARGET is the name of the output
# APP_ICON is the path of the image used as an icon
# BUILD is the directory where object files & intermediate files will be placed
# SOURCES is a list of directories containing source code
# INCLUDES is a list of directories containing header files
#
#---------------------------------------------------------------------------------
TARGET		:=	FAKE08
APP_ICON    :=  source/icon.png
BUILD		:=	build-$(PLATFORM)
SOURCES		:=	${SOURCES} source
INCLUDES	:=	${INCLUDES}

#---------------------------------------------------------------------------------
# options for code generation
#---------------------------------------------------------------------------------

Q ?= @
NWLINK := npx --yes -- nwlink@0.0.19

CFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.c)))
CPPFILES	:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.cpp)))

export OFILES_SOURCES 	:=	$(addprefix $(BUILD)/,$(CPPFILES:.cpp=.o) $(CFILES:.c=.o)) #$(SFILES:.s=.o)
export INCLUDE	:=	$(foreach dir,$(INCLUDES),-I$(CURDIR)/$(dir)) \
			-I$(CURDIR)/$(BUILD)

ifeq ($(PLATFORM),device)
  CXX := arm-none-eabi-g++
  CC = $(CXX)
  LINK_GC := 1
  LTO := 1
else
  CXX := g++
  CC := $(CXX)
  GDB = gdb --args
  LINK_GC := 0
  LTO := 0
endif

# Avoid unnecessary nwlink queries
ifdef APP_TITLE
COMMON_FLAGS := $(shell $(NWLINK) eadk-cflags-$(PLATFORM)) -MMD -MP
endif
COMMON_FLAGS += -Wall -ggdb
COMMON_FLAGS += -DMINIZ_NO_TIME -DVER_STR=\"$(APP_VERSION)\"
ifeq ($(PLATFORM),device)
	COMMON_FLAGS += -D_NUMWORKS
endif

# Avoid unnecessary nwlink queries
ifdef APP_TITLE
LDFLAGS := $(shell $(NWLINK) eadk-ldflags-$(PLATFORM))
endif
ifeq ($(PLATFORM),device)
# LDFLAGS += --specs=nano.specs
LDFLAGS += --specs=nosys.specs # Alternatively, use full-fledged newlib
else
# Kept as in the official NumWorks template, but this flag is actually macOS (ld64) specific
LDFLAGS += -Wl,-undefined,dynamic_lookup
endif

ifeq ($(LINK_GC),1)
COMMON_FLAGS += -fdata-sections -ffunction-sections
LDFLAGS += -Wl,-e,main -Wl,-u,eadk_app_name -Wl,-u,eadk_app_icon -Wl,-u,eadk_api_level
LDFLAGS += -Wl,--gc-sections
endif

ifeq ($(LTO),1)
COMMON_FLAGS += -flto=auto -fno-fat-lto-objects
COMMON_FLAGS += -fwhole-program
COMMON_FLAGS += -fvisibility=internal
LDFLAGS += -flinker-output=nolto-rel
endif

CPPFLAGS := $(INCLUDE)

CFLAGS += $(COMMON_FLAGS)

CXXFLAGS := -std=gnu++17 -fno-exceptions -fno-rtti -fpermissive
CXXFLAGS += $(COMMON_FLAGS)

ifeq ($(PLATFORM),device)
  CXXFLAGS += -Os
else
  CXXFLAGS += -O0 -g
endif

#---------------------------------------------------------------------------------
# External Data
#---------------------------------------------------------------------------------
ifdef EXTERNAL_DATA
  ifeq ($(PLATFORM),device)
    EXTERNAL_DATA_INPUT = --external-data $(EXTERNAL_DATA)
  else
    EXTERNAL_DATA_INPUT = --nwb-external-data $(EXTERNAL_DATA)
  endif
else
  EXTERNAL_DATA =
  EXTERNAL_DATA_INPUT =
endif

#---------------------------------------------------------------------------------
export OUTPUT	:=	$(CURDIR)/$(TARGET)

export VPATH	:=	$(foreach dir,$(SOURCES),$(CURDIR)/$(dir))

#export DEPSDIR	:=	$(CURDIR)/$(BUILD)

#---------------------------------------------------------------------------------
# use CXX for linking C++ projects, CC for standard C
#---------------------------------------------------------------------------------
ifeq ($(strip $(CPPFILES)),)
	export LD	:=	$(CC)
else
	export LD	:=	$(CXX)
endif

#---------------------------------------------------------------------------------
# Targets
#---------------------------------------------------------------------------------
.PHONY: build run clean debug

# Allows to execute make from either the root directory ("make numworks") or the platform/numworks directory
ifndef APP_TITLE
build: $(BUILD)
	$(Q) $(MAKE) -C ../.. numworks $(if $(EXTERNAL_DATA),EXTERNAL_DATA=$(abspath $(EXTERNAL_DATA)))

$(TARGET).nwa $(TARGET).nwb: build
else

ifeq ($(PLATFORM),device)

build: $(BUILD) $(TARGET).bin

%.bin: $(TARGET).nwa $(EXTERNAL_DATA)
	@echo $(notdir $@)
	$(Q) $(NWLINK) nwa-bin $(EXTERNAL_DATA_INPUT) $< $@

$(TARGET).nwa: $(OFILES_SOURCES) $(BUILD)/icon.o
	@echo "LD      $(notdir $@)"
	$(Q) $(LD) $(CXXFLAGS) $(LDFLAGS) $^ -o $@
else 

build: $(BUILD) $(TARGET).nwb

$(TARGET).nwb: $(OFILES_SOURCES) $(LINUX_SIMULATOR_PATH)
	@echo "LD      $(notdir $@)"
	$(Q) $(LD) $(CXXFLAGS) $(LDFLAGS) $(OFILES_SOURCES) -o $@

endif

endif

$(BUILD):
	$(Q) mkdir -p $@

clean:
	@echo "clean ..."
	$(Q) rm -fr build-* $(TARGET).nwa $(TARGET).nwb $(TARGET).bin pico.log

ifeq ($(PLATFORM),device)
run: $(TARGET).nwa $(EXTERNAL_DATA)
	@echo "INSTALL $<"
	$(Q) $(NWLINK) install-nwa $(EXTERNAL_DATA_INPUT) $<

debug:
	@echo "Debugging is only supported with PLATFORM=simulator."; \

else
run: $(TARGET).nwb $(LINUX_SIMULATOR_PATH) $(EXTERNAL_DATA)
	@echo "RUN     $<"
	$(Q) $(LINUX_SIMULATOR_PATH) --nwb ./$< $(EXTERNAL_DATA_INPUT)

.PHONY: debug
debug: $(TARGET).nwb $(LINUX_SIMULATOR_PATH) $(EXTERNAL_DATA)
	@echo "DEBUG   $<"
	$(Q) $(GDB) $(LINUX_SIMULATOR_PATH) --nwb ./$< $(EXTERNAL_DATA_INPUT)

endif

DEPS := $(OFILES_SOURCES:.o=.d)
-include $(DEPS)

$(BUILD)/%.o: %.cpp
	@echo $(notdir $^)
	$(Q) $(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

$(BUILD)/%.o: %.c
	@echo $(notdir $^)
	$(Q) $(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(BUILD)/icon.o: $(APP_ICON)
	@echo "ICON    $(notdir $<)"
	$(Q) $(NWLINK) png-icon-o $< $@