MAKEFLAGS += --no-print-directory -s

ROOT_PATH := $(strip $(patsubst %/, %, $(dir $(abspath $(lastword $(MAKEFILE_LIST))))))

$(ROOT_PATH)/bin/example: $(ROOT_PATH)/example.c
	gcc -Wall -Wextra -Werror -std=c89 -pedantic -o $@ $<
	echo built $@
