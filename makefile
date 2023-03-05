#======================================================================================================================#
# CONFIG
#======================================================================================================================#

#---- BASIC -----------------------------------------------------------------------------------------------------------#

LIBRARY        := microcompute
LIBS           := -lmicrocompute -lgbm -lEGL -lGL -lGLEW
FLAGS          := -Wall -Wextra -Wno-missing-braces -Wno-unused-parameter
DEFS           := $(CFLAGS)

#---- PROJECT STRUCTURE -----------------------------------------------------------------------------------------------#

INCLUDE_FOLDER := include
LIB_FOLDER     := lib
BUILD_FOLDER   := build
SRC_FOLDER     := src
OUT_FOLDER     := tmp
EXAMPLE_FOLDER := example

#======================================================================================================================#

CC                := gcc $(FLAGS) -isystem $(INCLUDE_FOLDER) -I $(SRC_FOLDER)
AR                := ar rcs
RM                := rm -rf
CP                := cp
MKDIR             := mkdir -p
SRC_FOLDERS       := $(shell find $(SRC_FOLDER)/ -type d)
BUILD_SUB_FOLDERS := $(subst $(SRC_FOLDER)/,$(BUILD_FOLDER)/,$(SRC_FOLDERS))
C_FILES           := $(shell find $(SRC_FOLDER)/ -type f -name "*.c")
C_OBJECTS         := $(subst $(SRC_FOLDER)/,$(BUILD_FOLDER)/,$(subst .c,.o,$(C_FILES)))
EXAMPLE_C_FILES   := $(shell find $(EXAMPLE_FOLDER)/ -type f -name "*.c")
EXAMPLES          := $(subst $(EXAMPLE_FOLDER)/,$(OUT_FOLDER)/,$(subst .c,,$(EXAMPLE_C_FILES)))
STATIC_LIB        := $(LIB_FOLDER)/lib$(LIBRARY).a

.PHONY: default all standalone_lib library examples doc clean

default: library doc

all: standalone_lib examples doc

standalone_lib: DEFS += -DSTANDALONE
standalone_lib: library

library: clean $(STATIC_LIB)

examples: DEFS += -DSTANDALONE
examples: $(EXAMPLES)

doc:
	python3 microdoc/doc_generator.py $(SRC_FOLDER)/$(LIBRARY).h doc.md

$(OUT_FOLDER):
	$(MKDIR) $(OUT_FOLDER)

$(INCLUDE_FOLDER):
	$(MKDIR) $(INCLUDE_FOLDER)

$(LIB_FOLDER):
	$(MKDIR) $(LIB_FOLDER)

$(BUILD_SUB_FOLDERS):
	$(MKDIR) $(BUILD_SUB_FOLDERS)

$(C_OBJECTS): $(BUILD_SUB_FOLDERS) $(C_FILES)
	$(CC) $(DEFS) -c $(subst $(BUILD_FOLDER)/,$(SRC_FOLDER)/,$(subst .o,.c,$@)) -o $@

$(STATIC_LIB): $(INCLUDE_FOLDER) $(LIB_FOLDER) $(C_OBJECTS)
	$(AR) $(STATIC_LIB) $(C_OBJECTS)
	$(CP) $(SRC_FOLDER)/$(LIBRARY).h $(INCLUDE_FOLDER)/$(LIBRARY).h

$(EXAMPLES): $(OUT_FOLDER) $(STATIC_LIB)
	$(CC) $(DEFS) -g $(subst $(OUT_FOLDER)/,$(EXAMPLE_FOLDER)/,$@).c -o $@ -L$(LIB_FOLDER) $(LIBS)

clean:
	$(RM) $(BUILD_FOLDER) $(INCLUDE_FOLDER) $(LIB_FOLDER) $(OUT_FOLDER)