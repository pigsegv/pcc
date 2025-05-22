OBJ := $(OBJ)/src

CFLAGS += -std=c++26 -Wall -Wextra -ggdb3 -O0 -fsanitize=address -pedantic
CFLAGS += -Wno-missing-field-initializers -fno-exceptions
LDFLAGS += -fsanitize=address -flto

SRCS := $(shell find -type f -name "*.cpp" -printf "%P\n")
OBJS := $(patsubst %.cpp, $(OBJ)/%.o, $(SRCS))
DEPS := $(patsubst %.cpp, $(OBJ)/%.d, $(SRCS))

ifeq ($(GENERATE_ASM), 1)
	ASMS := $(patsubst %.cpp, $(OBJ)/%.s, $(SRCS))
endif

all: $(BIN)/pcc $(ASMS)

$(BIN)/pcc: $(OBJS) 
	@$(LD) $(CFLAGS) $^ -o $@ $(LDFLAGS)
	@echo built $@

-include $(DEPS)

$(OBJ)/%.o: %.cpp
	@$(CC) $(CFLAGS) -MD -MP -c $< -o $@
	@echo built $@

$(OBJ)/%.s: %.cpp
	@$(CC) $(CFLAGS) -S -masm=intel $< -o $@
	@echo Generated $@
