
#################################################
#################################################
#######   MAKEFILE CONFIGURATION PART   #########
#################################################
#################################################

#####################################################
### Edit these variables to compile your project! ###
#####################################################


##################################
### General configuration section.

#TARGET = executable name
TARGET = trackviewer

# Directory where build junk goes. Comment this line out or leave
# empty if you want to build in the working directory.
BUILD_DIR = build

# Additional directories for source files 
VPATH = ../common ../scaletracker

# Directory where external libs are
BASELIBS_DIR = ../..

# C, CXX compiler
CXXC = g++
CC = gcc

# file extensions for C++ files, their objects and dependencies. (.c,
# .o and .d are always used for C files)
CPP_EXT = cc
DEP_EXT = dd
OBJ_EXT = oo

### files to compile. add C++ sources and C sources to the right variable
#CPPSOURCES = test_trackstring.cc tracking.cc trackstring.cc demreader.cc scalespace.cc
CPPSOURCES = viewer.cc camera.cc trackdisplay.cc track.cc demreader.cc scalespace.cc dem.cc flipper.cc trackstats.cc
CSOURCES =

# generic compilation configuration, to be used for both C++ and C
# on every OS.  FLAGS are always used, while DEBUG_FLAGS are used
# instead of RELEASE flags when "make debug" is used.
FLAGS = 
DEBUG_FLAGS = -Wall -W -g
RELEASE_FLAGS = -O2 -DNDEBUG
INCLUDES = -I../common/ -I${BASELIBS_DIR}/AntTweakBar/include/
DEFINES = 
LDFLAGS = -lpng -lz -lm -L${BASELIBS_DIR}/AntTweakBar/lib -lAntTweakBar
FRAMEWORKS = 


####################################
### Language dependent configuration

# Ignore this section if your project use C++ only or C only without
# mixing the two. When using both languages, the C++ compiler is the
# one called in the linking phase.

# C specific conf
CFLAGS = -std=gnu99
CINCLUDES = 
CDEFINES = 

# C++ specific conf
CXXFLAGS = -pipe  -D_THREAD_SAFE -pthread -Wno-write-strings
CXXINCLUDES = 
CXXDEFINES = 


##################################
### System-dependent configuration

# here add system-dependent settings to previous variables, using the
# syntax `override +=' if you want to add things, `=' if you want to
# replace them

UNAME := $(shell uname -s)
GENERIC = true

# macosx
ifeq ($(UNAME),Darwin)
override FRAMEWORKS += 
override INCLUDES += -I/opt/local/include
override LDFLAGS += -L/opt/local/lib
override FRAMEWORKS += -framework GLUT -framework OpenGL
override INCLUDES += -I/System/Library/Frameworks/GLUT.framework/Headers/
override LDFLAGS += -lm -lpthread -L/usr/X11R6/lib -lm -lpthread -lX11
GENERIC = false
endif

# eventually add here other specific OSes, remembering to set 
# `GENERIC = false' once you're done

# generic UNIX
ifeq ($(GENERIC),true)
override LDFLAGS += -lglut -lGLU -lX11 -lGL
override INCLUDES += 
endif


##############################################
##############################################
########    END OF EDITABLE PART     #########
##############################################
##############################################

### Leave this section alone unless there is a bug or you want to
### alter funcionality. If you do, please send back addictional
### features to the author!

# non-file targets
.PHONY : all clean

# use release flags for `make', debug flags for `make debug'
all: override FLAGS += $(RELEASE_FLAGS)
debug: override FLAGS += $(DEBUG_FLAGS)

# complete specific language flags with general ones
override CFLAGS += $(FLAGS)
override CINCLUDES += $(INCLUDES)
override CDEFINES += $(DEFINES)
override CXXFLAGS += $(FLAGS)
override CXXINCLUDES += $(INCLUDES)
override CXXDEFINES += $(DEFINES)

# use C++ as linker if there are C++ sources.
ifeq ($(strip $(CPPSOURCES)),)
LD = $(CC)
else
LD = $(CXXC)
endif

# set the list of modules to be compiled
override OBJECTS += $(CPPSOURCES:.$(CPP_EXT)=.$(OBJ_EXT))
override DEPS += $(CPPSOURCES:.$(CPP_EXT)=.$(DEP_EXT))
override OBJECTS += $(CSOURCES:.c=.o)
override DEPS += $(CSOURCES:.c=.d)

# comment out to debug the makefile
# all: stupid
# stupid:
## debug print
#	echo $(OBJECTS)
#	echo $(DEPS)

# standard goals
all : $(TARGET)
debug : $(TARGET)
clean :
	-$(RM) $(BUILD_OBJECTS) $(BUILD_DEPS) $(TARGET)
ifneq ($(strip $(BUILD_DIR)),)
	-$(RM) -r $(BUILD_DIR)
endif

# if build dir is non-empty
ifneq ($(strip $(BUILD_DIR)),)
# add trailing slash and escaped trailing slash
BUILD_DIR_SLASH = $(BUILD_DIR)/
BUILD_DIR_SLASH_SED = $(BUILD_DIR)\/
# make it if not existent
ifneq ($(MAKECMDGOALS),clean)
$(shell if [ ! -d $(BUILD_DIR) ]; then mkdir $(BUILD_DIR); fi)
endif
endif
# prepend its name to targets.
BUILD_DEPS = $(addprefix $(BUILD_DIR_SLASH), $(DEPS))
BUILD_OBJECTS = $(addprefix $(BUILD_DIR_SLASH), $(OBJECTS))

# include dependencies, but only if we're not cleaning
ifneq ($(MAKECMDGOALS),clean)
-include $(BUILD_DEPS)
endif


# program linking
$(TARGET) : $(BUILD_OBJECTS)
	$(LD) $^ $(FRAMEWORKS) -o $@ $(LDFLAGS)

# implicit rules for objects and dependencies, C
$(BUILD_DIR_SLASH)%.o: %.c
	$(CC) $(CDEFINES) $(CFLAGS) $(CINCLUDES) -c $< -o $@  

$(BUILD_DIR_SLASH)%.d: %.c
	$(CC) -MM $(CDEFINES) $(CFLAGS) $(CINCLUDES) $< | sed 's/\($(subst /,\/,$*)\)\.o[ :]*/$(BUILD_DIR_SLASH_SED)\1.o $(subst /,\/,$@) : /g' > $@;

# implicit rules for objects and dependencies, C++
$(BUILD_DIR_SLASH)%.$(OBJ_EXT): %.$(CPP_EXT)
	$(CXXC) $(CXXDEFINES) $(CXXFLAGS) $(CXXINCLUDES) -c $< -o $@  

$(BUILD_DIR_SLASH)%.$(DEP_EXT): %.$(CPP_EXT)
	$(CXXC) -MM $(CXXDEFINES) $(CXXFLAGS) $(CXXINCLUDES) $< | sed 's/\($(subst /,\/,$*)\)\.o[ :]*/$(BUILD_DIR_SLASH_SED)\1.$(OBJ_EXT) $(subst /,\/,$@) : /g' > $@;
