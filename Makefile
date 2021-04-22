CFLAGS += -O3 -ftree-vectorize -std=gnu99
CONTIKI_PROJECT = WSN_MiniProject_3
all: $(CONTIKI_PROJECT)

CONTIKI = ../..
PLATFORMS_EXCLUDE = nrf52dk
TARGET_LIBFILES = -lm

MAKE_NET = MAKE_NET_NULLNET

include $(CONTIKI)/Makefile.include
