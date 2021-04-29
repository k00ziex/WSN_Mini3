CONTIKI_PROJECT = sink source aggregator
all: $(CONTIKI_PROJECT)

CONTIKI = ../../..

PLATFORMS_EXCLUDE = nrf52dk

#use this to enable TSCH: MAKE_MAC = MAKE_MAC_TSCH
MAKE_MAC ?= MAKE_MAC_CSMA
MAKE_NET = MAKE_NET_NULLNET
CFLAGS += -std=gnu99 
#-O3 -ftree-vectorize 

include $(CONTIKI)/Makefile.include
