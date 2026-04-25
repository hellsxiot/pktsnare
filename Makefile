CC      := gcc
CFLAGS  := -Wall -Wextra -Wpedantic -std=c99 -D_DEFAULT_SOURCE
LDFLAGS :=

SRC_DIR   := src
TEST_DIR  := tests
BUILD_DIR := build

SRCS    := $(SRC_DIR)/capture.c
OBJS    := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRCS))

TARGET  := pktsnare
TEST_BIN := $(BUILD_DIR)/test_capture

.PHONY: all clean test

all: $(BUILD_DIR) $(TARGET)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

test: $(BUILD_DIR) $(TEST_BIN)
	./$(TEST_BIN)

$(TEST_BIN): $(TEST_DIR)/test_capture.c $(SRC_DIR)/capture.c
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -rf $(BUILD_DIR) $(TARGET)
