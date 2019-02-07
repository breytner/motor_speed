# Makefile SystemC

ifndef SYSTEMC_PREFIX
   $(error Please set environment variable SYSTEMC_PREFIX)
endif
SYSTEMC = $(SYSTEMC_PREFIX)

ifndef SYSTEMC_AMS_PREFIX
   $(error Please set environment variable SYSTEMC_AMS_PREFIX -SystemC AMS copiled against SystemC 2.3.1a)
endif
SYSTEMC_AMS = $(SYSTEMC_AMS_PREFIX)

ifndef ENSITLM
 	ENSITLM = ../ensitlm/
endif


INCLUDES = -I$(ENSITLM) -I$(SYSTEMC)/include -I$(SYSTEMC_AMS)/include
CC = g++
CPPFLAGS = $(INCLUDES)
CXXFLAGS =  -std=gnu++11 -Wno-deprecated -O0 -g -Wall $(CXXEXTRAFLAGS)

LD = $(CC)
LDFLAGS = 

HEADERS = ../fixed_point_params.h ../address_map.h ../adc_params.h
SRCS = fast_bus.cpp memory.cpp uart.cpp adc.cpp dac.cpp dc_motor.cpp \
	de2tdf.cpp tdf2de.cpp probe.cpp adc_dac_helpers fp_conversions_helpers
BASE := $(SRCS)
BASE := $(BASE:%.cpp=%)
BASE := $(BASE:%.cc=%)
DEPS = $(BASE:%=%.d)
OBJS = $(BASE:%=%.o)

LIB=libhardware.a

all: $(LIB)

hardware: $(LIB)

clean: FORCE
	-rm -f $(DEPS) $(OBJS) *.out core $(PROGRAM) $(LIB)

debug:
	@echo "LIB=$(LIB)"
	@echo "SRCS=$(SRCS)"
	@echo "BASE=$(BASE)"
	@echo "OBJS=$(OBJS)"
	@echo "DEPS=$(DEPS)"

$(LIB): $(OBJS)
	ar crs $(LIB) $(OBJS)

%.o: %.cpp $(filter-out %.d, $(MAKEFILE_LIST)) $(HEADERS)
	$(CC) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

%.o: %.cc
	$(CC) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

%.d: ;

FORCE: ;