#####################################################
###        MAKEFILE CONFIGURATION PART       ########
### Edit these variables to compile your project! ###
#####################################################

### This makefile is supported by makefile-controller.sh
### _MM44FFCC00_ # do not touch, magic number




##################################
### General Makefile behaviour.

## set to true to make the project recompile if the makefile is changed
# MAKEFILE_FORCEBUILD := true
MAKEFILE_FORCEBUILD := true

## if the target you issue changes from one time to the next (e.g. you
## pass from debug to release or the other way round) force rebuild of
## everything. highly useful but experimental, use with care.
# NEW_TARGET_FORCEBUILD := true
# NEW_TARGET_FOCERBUILD_FILE := .file_old_goal
NEW_TARGET_FORCEBUILD := true
NEW_TARGET_FOCERBUILD_FILE := .file_old_goal

##################################
### Project configuration section.

## Use ifeq ($(USERNAME),yourname) to add user-specific configurations
## in multideveloper settings
# USERNAME := $(shell whoami)
USERNAME := $(shell whoami)

## executable name
# TARGET = exe
TARGET = track2csv

## Directory where build junk goes. Comment this line out or leave
## empty if you want to build in the working directory.
# BUILD_DIR = build-dir
BUILD_DIR = build

## Additional directories for source files
# VPATH = add/dir1 ../add/dir2 ../../etc/
VPATH = ../../common
VPATH += ../../scaletracker

## Directory for external libraries
## not used outside the configuration section and provided for your
## convenience when defining INCLUDES and LDFLAGS. Define as many as
## such variables as you need.
BASELIBS_DIR = ../../../libs

## C, CXX compiler
# CXXC = g++
# CC = gcc
CXXC = g++
CC = gcc

## file extensions for C++ files, their objects and dependencies (e.g:
## .cpp/.dpp/.opp, or .cc/.dd/.oo or whatever you like; remember that
## .c, .d and .o are always used for C files).
# CPP_EXT = cc
# DEP_EXT = dd
# OBJ_EXT = oo
CPP_EXT = cc
DEP_EXT = dd
OBJ_EXT = oo

## Names of files to compile, without path. add C++ sources and C
## sources to the right variable. Remember, you cannot have file with
## the same name in different dirs because name of object files would
## conflict.
# CPPSOURCES = file1.cc file2.cc
# CSOURCES = file1.c file2.c
CPPSOURCES = ascheader.cc csvreader.cc demreader.cc dem.cc track.cc flipper.cc trackstats.cc scalespace.cc
CPPSOURCES += track2csv.cc

## add directives -D__FILENAME_EXISTS for every file "filename.h" added here.
## purpose: untracked files with custom build configurations.
# CUSTOM_HEADERS = custom_header.h
CUSTOM_HEADERS = custom_header.h

## generic compilation configuration, to be used for both C++ and C on
## every OS. FLAGS are always used, while DEBUG_FLAGS are used instead
## of RELEASE flags when "make debug" is used. STATIC_LDFLAGS are used
## instead of LDFLAGS if "make static" is used.
# override FLAGS +=
# override DEBUG_FLAGS +=
# override TRACE_FLAGS +=
# override RELEASE_FLAGS +=
# override STATIC_FLAGS +=
# override INCLUDES +=
# override DEFINES +=
# override LDFLAGS +=
# override STATIC_LDFLAGS +=
# override FRAMEWORKS +=
override FLAGS += 
override DEBUG_FLAGS += -Wall -Wextra -Wno-unused-local-typedefs -g
override TRACE_FLAGS += -DDEBUGTRACE
override RELEASE_FLAGS += -O2 -DNDEBUG
override INCLUDES += -I../../common/ -I${BASELIBS_DIR}/Eigen/ -I../common/
override DEFINES += 
override LDFLAGS +=
override STATIC_LDFLAGS +=
override FRAMEWORKS += 


####################################
### Language dependent configuration

# Ignore this section if your project use C++ only or C only without
# mixing the two. When using both languages, the C++ compiler is the
# one called in the linking phase.

# C specific conf
# override CFLAGS +=
# override CINCLUDES +=
# override CDEFINES +=
override CFLAGS += -std=gnu99
override CINCLUDES += 
override CDEFINES += 

# C++ specific conf
# override CXXFLAGS +=
# override CXXINCLUDES +=
# override CXXDEFINES +=
override CXXFLAGS += -pipe  -D_THREAD_SAFE -pthread -Wno-write-strings
override CXXINCLUDES += 
override CXXDEFINES += 


##################################
### System-dependent configuration

## here add system-dependent settings to previous variables, using the
## syntax `override +=' if you want to add things, ` override =' if you
## want to replace them. A simple mechanism with uname is currently
## used. Generic is used as a catch-all implementation, currently
## targeting linux.

# # recognition
# UNAME := $(shell uname -s)
# GENERIC = true

# # MACOSX
# ifeq ($(UNAME),Darwin)
# override VAR += whatever
# GENERIC = false
# endif

# # eventually add here other specific OSes, remembering to set
# # `GENERIC = false' once you're done

# # generic UNIX
# ifeq ($(GENERIC),true)
# override VAR += whatever
# endif

UNAME := $(shell uname -s)
GENERIC = true

ifeq ($(UNAME),Darwin) # MACOSX
override CXXC = g++ -mmacosx-version-min=10.4
override FRAMEWORKS += 
override INCLUDES += -I/opt/local/include
override LDFLAGS += -L/opt/local/lib
override LDFLAGS += -Dcimg_use_png -lpng -lz -ltiff -lm
override LDFLAGS += -lm -lpthread -L/usr/X11R6/lib -lm -lpthread -lX11
override STATIC_LDFLAGS += -L/opt/local/lib
override STATIC_LDFLAGS += -Dcimg_use_png /opt/local/lib/libpng.a /opt/local/lib/libz.a /opt/local/lib/libtiff.a /opt/local/lib/libjpeg.a /opt/local/lib/liblzma.a -lm

GENERIC = false
endif

ifeq ($(GENERIC),true) # UNIX/LINUX
override LDFLAGS += -Dcimg_use_png -lpng -lz -ltiff -lm
override LDFLAGS +=  -lX11 -lpthread
override STATIC_LDFLAGS += -static -static-libgcc -static-libstdc++
override STATIC_LDFLAGS += -Dcimg_use_png -lpng -lz -ltiff -lm
override STATIC_LDFLAGS +=  -lX11 -lpthread
override INCLUDES += 
endif










# _MM44FFCC00__SPLIT_ # do not touch. magic line for splitting between the two parts using csplit.


##############################################
##############################################
########    END OF EDITABLE PART     #########
##############################################
##############################################

### Leave this section alone unless there is a bug or you want to
### alter funcionality. If you do, please send back addictional
### features to the author!


# experimental - rebuild all if current command is different from
# previous one.
ifeq ($(NEW_TARGET_FORCEBUILD), true)
#$(warning NEW_TARGET_FORCEBUILD)

ifeq ($(MAKECMDGOALS),)
NEWGOAL := all
else
NEWGOAL := $(MAKECMDGOALS)
endif
OLDGOAL := $(shell if [ -f .file_old_goal ]; then cat .file_old_goal; else echo __empty__; fi)
#$(warning $(OLDGOAL))
#$(warning $(NEWGOAL))
dummy := $(shell echo $(NEWGOAL) > .file_old_goal)

ifneq ($(NEWGOAL),clean)
ifneq ($(OLDGOAL),clean)
ifneq ($(MAKECMDGOALS),$(OLDGOAL))
ifneq ($(NEWGOAL), $(OLDGOAL))

#$(warning DELETE)
dummy := $(shell $(RM) $(BUILD_OBJECTS) $(BUILD_DEPS) $(TARGET))
ifneq ($(strip $(BUILD_DIR)),)
dummy := $(shell $(RM) -r $(BUILD_DIR))
endif

endif
endif
endif
endif

endif


# non-file targets
.PHONY : all clean debug trace debugtrace static staticdebugtrace


# use release flags for `make', debug flags for `make debug' and
# tracing flags for "make trace" if your application supports them.
all: override FLAGS += $(RELEASE_FLAGS)
debug: override FLAGS += $(DEBUG_FLAGS)
trace: override FLAGS += $(TRACE_FLAGS)
debugtrace: override FLAGS += $(DEBUG_FLAGS) $(TRACE_FLAGS)
#static: override FLAGS += $(RELEASE_FLAGS) $(STATIC_FLAGS)
#staticdebugtrace: override FLAGS+= $(DEBUG_FLAGS) $(TRACE_FLAGS) $(STATIC_FLAGS)

FINAL_LDFLAGS =
all: override FINAL_LDFLAGS += $(LDFLAGS)
debug: override FINAL_LDFLAGS += $(LDFLAGS)
trace: override FINAL_LDFLAGS += $(LDFLAGS)
debugtrace: override FINAL_LDFLAGS += $(LDFLAGS)
static: override FINAL_LDFLAGS += $(STATIC_LDFLAGS)
staticdebugtrace: override FINAL_LDFLAGS += $(STATIC_LDFLAGS)


# add custom headers defines, if any
lc = $(subst A,a,$(subst B,b,$(subst C,c,$(subst D,d,$(subst E,e,$(subst F,f,$(subst G,g,$(subst H,h,$(subst I,i,$(subst J,j,$(subst K,k,$(subst L,l,$(subst M,m,$(subst N,n,$(subst O,o,$(subst P,p,$(subst Q,q,$(subst R,r,$(subst S,s,$(subst T,t,$(subst U,u,$(subst V,v,$(subst W,w,$(subst X,x,$(subst Y,y,$(subst Z,z,$1))))))))))))))))))))))))))
uc = $(subst a,A,$(subst b,B,$(subst c,C,$(subst d,D,$(subst e,E,$(subst f,F,$(subst g,G,$(subst h,H,$(subst i,I,$(subst j,J,$(subst k,K,$(subst l,L,$(subst m,M,$(subst n,N,$(subst o,O,$(subst p,P,$(subst q,Q,$(subst r,R,$(subst s,S,$(subst t,T,$(subst u,U,$(subst v,V,$(subst w,W,$(subst x,X,$(subst y,Y,$(subst z,Z,$1))))))))))))))))))))))))))

ifneq ($(strip $(CUSTOM_HEADERS)),)
override FLAGS += $(addsuffix _EXISTS, $(addprefix -D__, $(call uc, $(basename $(notdir $(CUSTOM_HEADERS))))))
endif


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
override OBJECTS := $(CPPSOURCES:.$(CPP_EXT)=.$(OBJ_EXT))
override DEPS := $(CPPSOURCES:.$(CPP_EXT)=.$(DEP_EXT))
override OBJECTS += $(CSOURCES:.c=.o)
override DEPS += $(CSOURCES:.c=.d)


MAKEFILENAME := $(firstword $(MAKEFILE_LIST))

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
override BUILD_DEPS := $(addprefix $(BUILD_DIR_SLASH), $(DEPS))
override BUILD_OBJECTS := $(addprefix $(BUILD_DIR_SLASH), $(OBJECTS))


# standard goals
all : $(TARGET)
debug : $(TARGET)
trace : $(TARGET)
debugtrace : $(TARGET)
static: $(TARGET)
staticdebugtrace: $(TARGET)

# clean project; warning, the clean commands are repeated in
# new_target_forcerebuild section.
clean :
	-$(RM) $(BUILD_OBJECTS) $(BUILD_DEPS) $(TARGET)
ifneq ($(strip $(BUILD_DIR)),)
	-$(RM) -r $(BUILD_DIR)
endif


# if makefile changes and option is active, rebuild
ifeq ($(MAKEFILE_FORCEBUILD),true)
$(BUILD_DEPS) $(BUILD_OBJECTS) : $(MAKEFILENAME)
endif



# include dependencies, but only if we're not cleaning
ifneq ($(MAKECMDGOALS),clean)
-include $(BUILD_DEPS)
endif


# program linking
$(TARGET) : $(BUILD_OBJECTS)
	$(LD) $^ $(FRAMEWORKS) -o $@ $(FINAL_LDFLAGS)

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


#### debugging
# comment out to debug the makefile
# all: stupid
# stupid:
## debug print
#	echo $(OBJECTS)
#	echo $(DEPS)
# $(warning $(MAKEFILENAME))
# $(error error)

