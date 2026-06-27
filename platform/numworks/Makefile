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

# DATA		:=	data

#---------------------------------------------------------------------------------
# options for code generation
#---------------------------------------------------------------------------------

Q ?= @
CC = arm-none-eabi-gcc
CXX = arm-none-eabi-g++
NWLINK = npx --yes -- nwlink@0.0.19
LINK_GC = 1
LTO = 1

# define object_for
# $(addprefix $(BUILD)/,$(addsuffix .o,$(basename $(1))))
# endef

CPPFLAGS = -std=gnu++17 -fno-exceptions
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

#---------------------------------------------------------------------------------
# no real need to edit anything past this point unless you need to add additional
# rules for different file extensions
#---------------------------------------------------------------------------------
ifneq ($(BUILD),$(notdir $(CURDIR)))
#---------------------------------------------------------------------------------

export OUTPUT	:=	$(CURDIR)/$(TARGET)

export VPATH	:=	$(foreach dir,$(SOURCES),$(CURDIR)/$(dir)) \

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


#pas sur d'avoir capté


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
	$(Q) $(CC) $(CPPFLAGS) $(LDFLAGS) $^ -o $@

%.o: %.cpp
	@echo "CXX     $<"
	$(Q) $(CXX) $(CPPFLAGS) $(SFLAGS) -c $< -o $@

icon.o: source/icon.png
	@echo "ICON    $<"
	$(Q) $(NWLINK) png-icon-o $< $@

#---------------------------------------------------------------------------------------
endif
#---------------------------------------------------------------------------------------


