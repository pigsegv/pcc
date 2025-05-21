MAKEFLAGS += --no-print-directory -s -j $(shell nproc)

ROOT_PATH := $(strip $(patsubst %/, %, $(dir $(abspath $(lastword $(MAKEFILE_LIST))))))
export ROOT_PATH

PLATFORM := linux

define get_dirs
	$(shell find $(1) -type d)
endef

define relative_foreach
	$(foreach _dir, $(1), $(shell realpath -s --relative-to="$(ROOT_PATH)" $(_dir)))
endef

CC := g++
AS := fasm
LD := g++

SRC := $(ROOT_PATH)/src
OBJ := $(ROOT_PATH)/obj
BIN := $(ROOT_PATH)/bin
INCLUDE := $(ROOT_PATH)/include

EXTERNAL_DIR := $(ROOT_PATH)/external

CFLAGS := -I$(SRC) -I$(INCLUDE) -I$(ROOT_PATH) -isystem $(EXTERNAL_DIR)
ASMFLAGS :=
LDFLAGS := -Wl,-rpath=$(BIN) -L$(BIN)

GENERATE_ASM := 0

export PLATFORM CC LD SRC OBJ BIN INCLUDE EXTERNAL_DIR EXTERNAL_LIBS_DIR CFLAGS LDFLAGS GENERATE_ASM ASMFLAGS AS

OBJ_DIRS += $(call get_dirs, $(SRC))

OBJ_DIRS := $(call relative_foreach, $(patsubst %, $(OBJ)/%, $(call relative_foreach, $(OBJ_DIRS))))

CREATE_DIR_COMMAND := ./scripts/dirs.sh

PROJECTS := pcc

.PHONY: all dirs clean external run qemu $(PROJECTS) test


all: $(PROJECTS)

# This rule runs first due to the include
create_dirs: dirs
-include create_dirs

# ---------------------- PROJECTS ----------------------

pcc:
	@$(MAKE) -C $(ROOT_PATH)/src

# ---------------------- UTILITY ----------------------

dirs: 
	@mkdir -p $(BIN) 
	@mkdir -p $(OBJ)
	@$(CREATE_DIR_COMMAND) $(OBJ_DIRS)

clean:
	-@rm -rf $(OBJ)
	-@rm -rf $(BIN)

valgrind: 
	@valgrind --leak-check=full \
         --show-leak-kinds=all \
         --track-origins=yes \
         --verbose \
         --log-file=./val.txt \
				 --keep-debuginfo=yes\
         ./bin/pcc examples/example.c sdkfjds
