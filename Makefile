CONTIKI_PROJECT = sink source1 source2 source3 source4 aggmote1 aggmote2
all: $(CONTIKI_PROJECT)

CONTIKI = ../..

PLATFORMS_EXCLUDE = nrf52dk

#use this to enable TSCH: MAKE_MAC = MAKE_MAC_TSCH
MAKE_MAC ?= MAKE_MAC_CSMA
MAKE_NET = MAKE_NET_NULLNET
CFLAGS += -std=gnu99 -Wno-unused-variable
#-O3 -ftree-vectorize 

include $(CONTIKI)/Makefile.include
