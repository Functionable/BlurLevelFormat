GXX=g++
AR=ar
RM=rm -rf

WINDOWSGXX=g++

CXXFLAGS=-std=c++17 -no-pie -g -lz -Werror=return-type -O3 -s -Os
CXXFLAGS_UNIX=$(CXXFLAGS)
CXXFLAGS_WINDOWS=$(CXXFLAGS) -static -static-libgcc -static-libstdc++

INCLUDES=./include

OUT_UNIX=./bin/libblf.a
OUT_WINDOWS=./bin/libblf.lib

OBJFOLDER=./build/objects
SRCFOLDER=./src

rwildcard=$(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))

SUBDIRS = $(call rwildcard,$(SRCFOLDER),*.cpp)
SRCDIRS = $(shell find $(SRCFOLDER) -type d)
OBJDIRS = $(OBJFOLDER) $(subst $(SRCFOLDER),$(OBJFOLDER),$(SRCDIRS))
OBJECTNAMES := $(subst $(SRCFOLDER),$(OBJFOLDER),$(SUBDIRS:%.cpp=%.o))
MINGWOBJECTS := $(subst $(SRCFOLDER),$(OBJFOLDER),$(SUBDIRS:%.cpp=%.mingw64.o))

# Colours
yellow=\033[1;33m
no_color=\033[0m

all: $(OBJDIRS) $(OUT_UNIX)
windows: $(OBJDIRS) $(OUT_WINDOWS)
test: all
	make -C ./tests test

clean: clean_tests $(OBJFOLDER) 
	@echo -e "$(no_color)Removing $(yellow)'$(OBJFOLDER)'$(no_color)..."
	@$(RM) $(OBJFOLDER)

clean_tests:
	make -C ./tests clean

$(OBJFOLDER):
	@echo -e "$(no_color)Making the $(yellow)'$(OBJFOLDER)' $(no_color)directory... $(no_color)"
	@mkdir -p $@

$(OBJFOLDER)/%: $(SRCFOLDER)/%
	@echo -e "$(no_color)Creating $(yellow)$@ $(no_color)from the source tree... $(no_color)"
	@mkdir -p $@

$(OBJFOLDER)/%.o : $(SRCFOLDER)/%.cpp
	@echo -e "$(no_color)Compiling $(yellow)$(notdir $@)$(no_color)..."
	@$(GXX) -c $< -I $(INCLUDES) $(CXXFLAGS_UNIX) -o $@

$(OBJFOLDER)/%.mingw64.o : $(SRCFOLDER)/%.cpp
	@echo -e "$(no_color)Compiling $(yellow)$(notdir $@)$(no_color)..."
	$(WINDOWSGXX) -c $< -I $(INCLUDES) $(CXXFLAGS_WINDOWS) -o $@

$(OUT_UNIX): $(OBJECTNAMES)
	@echo -e "$(no_color)Linking $(yellow)$(notdir $(OUT_UNIX))...$(no_color)"
	@$(AR) -r $(OUT_UNIX) $(OBJECTNAMES)

$(OUT_WINDOWS): $(MINGWOBJECTS)
	@echo -e "$(no_color)Linking $(yellow)$(notdir $(OUT_WINDOWS))...$(no_color)"
	@$(AR) -rv $(OUT_WINDOWS) $(MINGWOBJECTS)