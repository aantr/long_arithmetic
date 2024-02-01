CC = g++

CFLAGS = \
	-std=c++17 \
	-O2 \
	-Wall      \
	-Wextra    \
	-Werror

# Linker flags:
# NOTE: it is sometimes required to link to math library.
LDFLAGS =

# Select build mode:
# NOTE: invoke with "DEBUG=1 make" or "make DEBUG=1".
ifeq ($(DEBUG),1)
	# Add default symbols:
	CFLAGS += -g
else
	# Enable link-time optimization:
	CFLAGS  += -flto
	LDFLAGS += -flto
endif

# Use ANSI color codes:
BRED    = \033[1;31m
BGREEN  = \033[1;32m
BYELLOW = \033[1;33m
GREEN   = \033[1;35m
BCYAN   = \033[1;36m
RESET   = \033[0m

INCLUDES = \
	include/arithmetic.hpp \
	include/fft.hpp

# Add "include" folder to header search path:
CFLAGS += -I $(abspath include) 

SOURCES = \
	arithmetic.cpp \
	fft.cpp

SOURCE_EXECUTABLE = calculate_pi.cpp
SOURCE_TEST = test.cpp

OBJECTS = $(SOURCES:%.cpp=build/%.o)
OBJECT_EXECUTABLE = $(SOURCE_EXECUTABLE:%.cpp=build/%.o)
OBJECT_TEST = $(SOURCE_TEST:%.cpp=build/%.o)

EXECUTABLE_SOURCE = build/arithmetic
EXECUTABLE_TEST = build/test

default: $(EXECUTABLE_SOURCE) $(EXECUTABLE_TEST)

$(EXECUTABLE_TEST): $(OBJECTS) $(OBJECT_TEST)
	@printf "$(BYELLOW)Linking executable $(BCYAN)$@$(RESET)\n"
	$(CC) $(LDFLAGS) $(OBJECTS) $(OBJECT_TEST) -o $@

build/%.o: src/%.cpp $(INCLUDES)
	@printf "$(BYELLOW)Building object file $(BCYAN)$@$(RESET)\n"
	@mkdir -p build
	$(CC) -c $< $(CFLAGS) -o $@

$(EXECUTABLE_SOURCE): $(OBJECTS) $(OBJECT_EXECUTABLE)
	@printf "$(BYELLOW)Linking executable $(BCYAN)$@$(RESET)\n"
	$(CC) $(LDFLAGS) $(OBJECTS) $(OBJECT_EXECUTABLE) -o $@

build/%.o: src/%.cpp $(INCLUDES)
	@printf "$(BYELLOW)Building object file $(BCYAN)$@$(RESET)\n"
	@mkdir -p build
	$(CC) -c $< $(CFLAGS) -o $@

run: $(EXECUTABLE_SOURCE)
	./$(EXECUTABLE_SOURCE)

test: $(EXECUTABLE_TEST)
	./$(EXECUTABLE_TEST)

clean:
	@printf "$(BYELLOW)Cleaning build directory $(RESET)\n"
	rm -rf build

.PHONY: run test clean default