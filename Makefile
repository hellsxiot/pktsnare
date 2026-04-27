CC      := gcc
CFLAGS  := -Wall -Wextra -g -Isrc
LDFLAGS := -lpcap

SRCS    := src/capture.c src/dissect.c src/filter.c src/output.c
OBJS    := $(SRCS:.c=.o)

BIN     := pktsnare

.PHONY: all clean test

all: $(BIN)

$(BIN): $(OBJS) src/main.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

# ---- tests ----
TEST_BINS := tests/test_capture tests/test_dissect tests/test_filter tests/test_output

tests/test_capture: tests/test_capture.c src/capture.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

tests/test_dissect: tests/test_dissect.c src/dissect.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

tests/test_filter: tests/test_filter.c src/filter.o src/dissect.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

tests/test_output: tests/test_output.c src/output.o src/dissect.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

test: $(TEST_BINS)
	@for t in $(TEST_BINS); do echo "--- $$t ---"; ./$$t; done

clean:
	$(RM) $(OBJS) src/main.o $(BIN) $(TEST_BINS)
