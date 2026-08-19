# ==============================================================================
# Makefile for EmbeddedDataConvert (EDC) Library
# ==============================================================================

CC ?= gcc
AR ?= ar
CFLAGS ?= -Wall -Wextra -Werror -pedantic -std=c99 -O2 -Iinclude
LIB_CFLAGS = $(CFLAGS) -Os

SRC = $(wildcard src/*.c)
OBJ = $(SRC:.c=.o)
LIB = libedc.a

TEST_SRC = $(wildcard tests/*.c) tests/unity/unity.c
FUZZ_SRC = fuzz/fuzz_runner.c
BENCH_SRC = benchmarks/benchmark_main.c

EX_STM32_SRC = examples/stm32/main_stm32_example.c
EX_ESP32_SRC = examples/esp32/main_esp32_example.c
EX_GPS_SRC   = examples/gps_tracker/main_gps_tracker.c
EX_UART_SRC  = examples/uart_streaming/main_uart_streaming.c

.PHONY: all lib test fuzz bench examples clean size

all: lib test

lib: $(LIB)

$(LIB): $(OBJ)
	$(AR) rcs $@ $^

src/%.o: src/%.c
	$(CC) $(LIB_CFLAGS) -c $< -o $@

test: $(LIB)
	$(CC) $(CFLAGS) -Itests/unity $(TEST_SRC) -L. -ledc -o run_tests
	./run_tests

fuzz: $(LIB)
	$(CC) $(CFLAGS) $(FUZZ_SRC) -L. -ledc -o run_fuzz
	./run_fuzz

bench: $(LIB)
	$(CC) -O3 -Wall -Wextra -Werror -pedantic -std=c99 -Iinclude $(BENCH_SRC) -L. -ledc -o run_bench
	./run_bench

examples: $(LIB)
	$(CC) $(CFLAGS) $(EX_STM32_SRC) -L. -ledc -o ex_stm32 && ./ex_stm32
	$(CC) $(CFLAGS) $(EX_ESP32_SRC) -L. -ledc -o ex_esp32 && ./ex_esp32
	$(CC) $(CFLAGS) $(EX_GPS_SRC) -L. -ledc -o ex_gps && ./ex_gps
	$(CC) $(CFLAGS) $(EX_UART_SRC) -L. -ledc -o ex_uart && ./ex_uart

size: $(LIB)
	@echo "--- Object File Sizes (Flash Text / Data / BSS) ---"
	@size src/*.o

clean:
	rm -f src/*.o $(LIB) run_tests run_fuzz run_bench ex_stm32 ex_esp32 ex_gps ex_uart
