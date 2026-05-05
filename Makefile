CC      = gcc
CFLAGS  = -Wall -Wextra -Wpedantic -std=c99 -Isrc
LDFLAGS = -lpcap

SRCS = src/capture.c src/dissect.c src/filter.c src/output.c \
       src/stats.c src/ring.c src/session.c src/decode.c \
       src/pcap.c src/proto.c src/iface.c src/throttle.c \
       src/hexdump.c src/timestamp.c src/alarm.c src/ratelimit.c \
       src/ratelimit_log.c src/flowtrack.c

OBJS = $(SRCS:.c=.o)

TARGET = pktsnare

TESTS = tests/test_capture tests/test_dissect tests/test_filter \
        tests/test_output tests/test_stats tests/test_ring \
        tests/test_session tests/test_decode tests/test_pcap \
        tests/test_proto tests/test_iface tests/test_throttle \
        tests/test_alarm tests/test_ratelimit tests/test_flowtrack

.PHONY: all clean tests

all: $(TARGET)

$(TARGET): $(OBJS) src/main.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

tests/test_flowtrack: tests/test_flowtrack.c src/flowtrack.o
	$(CC) $(CFLAGS) -o $@ $^

tests/test_%: tests/test_%.c src/%.o
	$(CC) $(CFLAGS) -o $@ $^

tests: $(TESTS)
	@for t in $(TESTS); do echo "--- $$t ---"; ./$$t; done

clean:
	$(RM) $(OBJS) src/main.o $(TARGET) $(TESTS)
