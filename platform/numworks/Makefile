#---------------------------------------------------------------------------------
# TARGET is the name of the output
# BUILD is the directory where object files & intermediate files will be placed
# SOURCES is a list of directories containing source code
# INCLUDES is a list of directories containing header files
#
#---------------------------------------------------------------------------------
TARGET		:=	FAKE08
BUILD		:=	build
SOURCES		:=	${SOURCES} source
INCLUDES	:=	${INCLUDES}

#---------------------------------------------------------------------------------
# options for code generation
#---------------------------------------------------------------------------------

Q ?= @
CXX := arm-none-eabi-g++
CC = $(CXX)
NWLINK := npx --yes -- nwlink@0.0.19
LINK_GC := 1
LTO := 1

COMMON_FLAGS := $(shell $(NWLINK) eadk-cflags-device)
COMMON_FLAGS += -Os -Wall
COMMON_FLAGS += -D_NUMWORKS -DMINIZ_NO_TIME

LDFLAGS := -Wl,--relocatable
LDFLAGS += -nostartfiles
LDFLAGS += --specs=nano.specs

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

CFLAGS += -ggdb
CFLAGS += $(COMMON_FLAGS)

CXXFLAGS := -std=gnu++17 -fno-exceptions -fno-rtti -fpermissive
CXXFLAGS += $(COMMON_FLAGS)
#---------------------------------------------------------------------------------
# no real need to edit anything past this point unless you need to add additional
# rules for different file extensions
#---------------------------------------------------------------------------------
ifneq ($(BUILD),$(notdir $(CURDIR)))
#---------------------------------------------------------------------------------

export OUTPUT	:=	$(CURDIR)/$(TARGET)

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
	$(Q) echo "CLEAN"
	$(Q) rm -fr $(BUILD) 
#potentiellement ajouter le $(TARGET) ou comme dans 3ds, mettre target.machin
#---------------------------------------------------------------------------------
#run:
#---------------------------------------------------------------------------------

# .PHONY: run
# run: $(BUILD)/$(TARGET).nwa
# 	@echo "INSTALL $<"
# 	$(Q) $(NWLINK) install-nwa $<


# .PRECIOUS: $(BUILD)
# $(BUILD)/source:
# 	mkdir -p $@

else

#---------------------------------------------------------------------------------
# main targets
#---------------------------------------------------------------------------------

.PHONY: build
build: $(TARGET).bin

%.bin: %.nwa
	@echo "BIN     $@"
	$(Q) $(NWLINK) nwa-bin $< $@

$(TARGET).nwa: $(OFILES_SOURCES) icon.o
	@echo "LD      $@"
	$(Q) $(LD) $(CPPFLAGS) $(LDFLAGS) $^ -o $@
# 	$(Q) $(CXX) $(CPPFLAGS) $(CXXFLAGS) $(LDFLAGS)
# 	$(Q) $(CC) $(CPPFLAGS) $(CXXFLAGS) $(LDFLAGS)

%.o: %.cpp
	@echo "CXX     $<"
	$(Q) $(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

%.o: %.c
	@echo "CC      $^"
	$(Q) $(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

icon.o: source/icon.png
	@echo "ICON    $<"
	$(Q) $(NWLINK) png-icon-o $< $@

#---------------------------------------------------------------------------------------
endif
#---------------------------------------------------------------------------------------