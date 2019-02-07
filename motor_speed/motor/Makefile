ifeq ($(origin SYSTEMC_PREFIX),undefined)
  $(error "Please define SYSTEMC_PREFIX")
endif

ifeq ($(origin SYSTEMC_AMS_PREFIX),undefined)
   $(error Please set environment variable SYSTEMC_AMS_PREFIX -SystemC AMS copiled against SystemC 2.3.1a)
endif

TARGET_ARCH     = linux64
SYSTEMC_INC_DIR = $(SYSTEMC_PREFIX)/include
SYSTEMC_LIB_DIR = $(SYSTEMC_PREFIX)/lib-$(TARGET_ARCH)
SYSTEMC_AMS_INC_DIR = $(SYSTEMC_AMS_PREFIX)/include
SYSTEMC_AMS_LIB_DIR = $(SYSTEMC_AMS_PREFIX)/lib-$(TARGET_ARCH)

# LIBTOOL Setting
SHELL           = /bin/bash
LIBTOOL_DIR     = /usr/bin
LIBTOOL         = $(SHELL) $(LIBTOOL_DIR)/libtool

GXX             = g++
GCC             = gcc
LD              = g++

FLAGS           = -g -Wall -pedantic -Wno-long-long \
                 -DSC_INCLUDE_DYNAMIC_PROCESSES -fpermissive \
                 -I$(SYSTEMC_INC_DIR) \
				 -I$(SYSTEMC_AMS_INC_DIR)
LDFLAGS         = -L$(SYSTEMC_LIB_DIR) -L$(SYSTEMC_AMS_LIB_DIR) -lsystemc-ams -lsystemc -lm

SRCS = $(wildcard *.cpp )
OBJS=$(SRCS:.cpp=.o)

# main
main: $(OBJS)
	$(LIBTOOL) --mode=link --tag=CXX g++ -o $@ $(OBJS) $(LDFLAGS)

.cpp.o:
	$(LIBTOOL) --mode=compile g++ -c $(FLAGS) $<

%.o: %.c
	$(LIBTOOL) --mode=compile gcc -Wall -c $<

clean:
	rm -f main *.o *.lo
	rm -rf .libs
