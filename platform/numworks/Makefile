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

COMMON_FLAGS := $(shell $(NWLINK) eadk-cflags-$(PLATFORM)) -MMD -MP
COMMON_FLAGS += -Wall -ggdb
COMMON_FLAGS += -DMINIZ_NO_TIME -DVER_STR=\"$(APP_VERSION)\"
ifeq ($(PLATFORM),device)
	COMMON_FLAGS += -D_NUMWORKS
endif

LDFLAGS := $(shell $(NWLINK) eadk-ldflags-$(PLATFORM))
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
COMMON_FLAGS += -flto -fno-fat-lto-objects
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
# no real need to edit anything past this point unless you need to add additional
# rules for different file extensions
#---------------------------------------------------------------------------------
ifneq ($(BUILD),$(notdir $(CURDIR)))
#---------------------------------------------------------------------------------

export OUTPUT	:=	$(CURDIR)/$(TARGET)
export TOPDIR := $(CURDIR)

export VPATH	:=	$(foreach dir,$(SOURCES),$(CURDIR)/$(dir))

#export DEPSDIR	:=	$(CURDIR)/$(BUILD)

CFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.c)))
CPPFILES	:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.cpp)))

#---------------------------------------------------------------------------------
# use CXX for linking C++ projects, CC for standard C
#---------------------------------------------------------------------------------
ifeq ($(strip $(CPPFILES)),)
#---------------------------------------------------------------------------------
	export LD	:=	$(CC)
#---------------------------------------------------------------------------------
else
#---------------------------------------------------------------------------------
	export LD	:=	$(CXX)
#---------------------------------------------------------------------------------
endif
#---------------------------------------------------------------------------------

export OFILES_SOURCES 	:=	$(CPPFILES:.cpp=.o) $(CFILES:.c=.o) #$(SFILES:.s=.o)
export INCLUDE	:=	$(foreach dir,$(INCLUDES),-I$(CURDIR)/$(dir)) \
			-I$(CURDIR)/$(BUILD)

.PHONY: all run clean

#---------------------------------------------------------------------------------
all: $(BUILD)
	$(Q) $(MAKE) --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile

$(BUILD):
	$(Q) mkdir -p $@
#---------------------------------------------------------------------------------
clean:
	$(Q) echo "clean ..."
	$(Q) rm -fr build-* $(TARGET).nwa $(TARGET).nwb $(TARGET).bin pico.log
#---------------------------------------------------------------------------------
ifeq ($(PLATFORM),device)
run: $(TARGET).nwa
	$(Q) echo "INSTALL $<"
	$(Q) $(NWLINK) install-nwa $<

$(TARGET).nwa: all
else
run: $(TARGET).nwb $(LINUX_SIMULATOR_PATH)
	$(Q) echo "RUN     $<"
	$(Q) $(LINUX_SIMULATOR_PATH) --nwb ./$<

.PHONY: debug
debug: $(TARGET).nwb $(LINUX_SIMULATOR_PATH)
	@echo "DEBUG   $<"
	$(Q) $(GDB) $(LINUX_SIMULATOR_PATH) --nwb ./$<

$(TARGET).nwb: all
endif
#---------------------------------------------------------------------------------

else

#---------------------------------------------------------------------------------
# main targets
#---------------------------------------------------------------------------------

.PHONY: build
ifeq ($(PLATFORM),device)

build: $(TOPDIR)/$(TARGET).bin

$(TOPDIR)/%.bin: $(TOPDIR)/$(TARGET).nwa
	@echo $(notdir $@)
# 	$(Q) $(NWLINK) nwa-bin -d $(TOPDIR)/../../carts/ld45.p8 $< $@
	$(Q) $(NWLINK) nwa-bin $< $@

$(TOPDIR)/$(TARGET).nwa: $(OFILES_SOURCES) icon.o
	@echo "LD      $(notdir $@)"
	$(Q) $(LD) $(CXXFLAGS) $(LDFLAGS) $^ -o $@

else

build: $(TOPDIR)/$(TARGET).nwb

$(TOPDIR)/$(TARGET).nwb: $(OFILES_SOURCES) $(TOPDIR)/$(LINUX_SIMULATOR_PATH)
	@echo "LD      $(notdir $@)"
	$(Q) $(LD) $(CXXFLAGS) $(LDFLAGS) $(OFILES_SOURCES) -o $@

endif

DEPS := $(OFILES_SOURCES:.o=.d)
-include $(DEPS)

%.o: %.cpp
	@echo $(notdir $^)
	$(Q) $(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

%.o: %.c
	@echo $(notdir $^)
	$(Q) $(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

icon.o: $(TOPDIR)/$(APP_ICON)
	@echo "ICON    $(notdir $<)"
	$(Q) $(NWLINK) png-icon-o $< $@

#---------------------------------------------------------------------------------------
endif
#---------------------------------------------------------------------------------------