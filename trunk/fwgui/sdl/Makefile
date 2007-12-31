#---------------------------------------------------------------------------------
.SUFFIXES:
#---------------------------------------------------------------------------------

export CXX	:=	g++
export LD	:=	g++

#---------------------------------------------------------------------------------
%.o: %.cpp
	$(CXX) -MMD -MP -MF $(DEPSDIR)/$*.d $(CXXFLAGS) -c $< -o $@

%.o: %.c
	gcc -MMD -MP -MF $(DEPSDIR)/$*.d $(CXXFLAGS) -c $< -o $@


#---------------------------------------------------------------------------------
# TARGET is the name of the output
# BUILD is the directory where object files & intermediate files will be placed
# SOURCES is a list of directories containing source code
# DATA is a list of directories containing data files
# INCLUDES is a list of directories containing header files
#---------------------------------------------------------------------------------
TARGET		:=	../lib/libfwgui_sdl
BUILD		:=	build
SOURCES		:=	source ../source
DATA		:=	../data
INCLUDES	:=	../include

# applications
BIN2C := bin2c

# other stuff
export PATH := ../../tools:$(PATH)

#---------------------------------------------------------------------------------
# options for code generation
#---------------------------------------------------------------------------------

CXXFLAGS	:=	$(INCLUDE) -g -Wall -fno-exceptions $(shell sdl-config --cflags)
ifneq (,$(findstring CYGWIN,$(shell uname)))
CXXFLAGS	+= -mno-cygwin
endif

LDFLAGS	=	-g -Wl,-Map,$(notdir $*.map) $(shell sdl-config --libs)

#---------------------------------------------------------------------------------
# list of directories containing libraries, this must be the top level containing
# include and lib
#---------------------------------------------------------------------------------
LIBDIRS	:=

#---------------------------------------------------------------------------------
# no real need to edit anything past this point unless you need to add additional
# rules for different file extensions
#---------------------------------------------------------------------------------
ifneq ($(BUILD),$(notdir $(CURDIR)))
#---------------------------------------------------------------------------------

export OUTPUT	:=	$(CURDIR)/$(TARGET)

export VPATH	:=	$(foreach dir,$(SOURCES),$(CURDIR)/$(dir)) \
			$(foreach dir,$(DATA),$(CURDIR)/$(dir))

export DEPSDIR	:=	$(CURDIR)/$(BUILD)

CFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.c)))
CPPFILES	:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.cpp)))
SFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.s)))
BINFILES	:=	$(foreach dir,$(DATA),$(notdir $(wildcard $(dir)/*.*)))

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

export OFILES	:=	$(addsuffix .o,$(BINFILES)) \
			$(CPPFILES:.cpp=.o) $(CFILES:.c=.o) $(SFILES:.s=.o)

export INCLUDE	:=	$(foreach dir,$(INCLUDES),-I$(CURDIR)/$(dir)) \
			$(foreach dir,$(LIBDIRS),-I$(dir)/include) \
			-I$(CURDIR)/$(BUILD)

export LIBPATHS	:=	$(foreach dir,$(LIBDIRS),-L$(dir)/lib)

.PHONY: $(BUILD) clean all

#---------------------------------------------------------------------------------
all: $(BUILD)

$(BUILD):
	@[ -d $@ ] || mkdir -p $@
	@$(MAKE) --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile

#---------------------------------------------------------------------------------
clean:
	@echo clean ...
	@rm -fr $(BUILD) $(TARGET).a


#---------------------------------------------------------------------------------
else

DEPENDS	:=	$(OFILES:.o=.d)

#---------------------------------------------------------------------------------
# main targets
#---------------------------------------------------------------------------------
$(OUTPUT).a	:	$(OFILES)
	@rm -f "$@"
	$(AR) rcs "$@" $(OFILES)
	@echo built ... $(notdir $@)

#---------------------------------------------------------------------------------
%.bin.o : %.bin.c
	@gcc -c -o $@ $<

%.bin.c %_bin.h	:	%.bin
	@echo $(notdir $<)
	@$(BIN2C) -h $(notdir $*)_bin > $(notdir $*)_bin.h
	@$(BIN2C) $(notdir $*)_bin < $< > $(notdir $*).bin.c
#---------------------------------------------------------------------------------


-include $(DEPENDS)

#---------------------------------------------------------------------------------------
endif
#---------------------------------------------------------------------------------------
