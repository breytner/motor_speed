
ifeq ($(origin SYSTEMC_PREFIX),undefined)
  $(error "Please define SYSTEMC_PREFIX")
endif

ifeq ($(origin SYSTEMC_AMS_PREFIX),undefined)
   $(error Please set environment variable SYSTEMC_AMS_PREFIX -SystemC AMS copiled against SystemC 2.3.1a)
endif

ifeq ($(origin RABBITS_INSTALL_DIR),undefined)
   $(error Please set environment variable SYSTEMC_AMS_PREFIX -SystemC AMS copiled against SystemC 2.3.1a)
endif

WRAPPER_PREFIX=$(RABBITS_INSTALL_DIR)


EXTRALDLIBS += ./hardware/libhardware.a ./elf-loader/libloader.a
INCLUDES = -I./hardware -I./ensitlm
CXXFLAGS = $(INCLUDES)


all: cpu main

example-cpu.o : example-cpu.cc
		g++ -c example-cpu.cc -o example-cpu.o -std=gnu++11 \
		$(CXXFLAGS) \
		-I${WRAPPER_PREFIX}/include \
		-I${SYSTEMC_PREFIX}/include -I${SYSTEMC_AMS_PREFIX}/include

cpu: ${WRAPPER_PREFIX}/include example-cpu.o $(EXTRALDLIBS) fixed_point_params.h address_map.h adc_params.h
	g++ -rdynamic example-cpu.o $(EXTRALDLIBS) -o $@ \
		-L${WRAPPER_PREFIX}/lib -L${SYSTEMC_PREFIX}/lib -L${SYSTEMC_PREFIX}/lib-linux64 \
		-L${SYSTEMC_AMS_PREFIX}/lib-linux64 \
		-Wl,-Bstatic -lsystemc-ams -Wl,-Bdynamic \
		-lcpu-wrapper -lsystemc -Wl,-rpath=${WRAPPER_PREFIX}
	
.PHONY: ./hardware/libhardware.a
./hardware/libhardware.a:
	@cd ./hardware && $(MAKE) hardware

.PHONY: main
main:
	@cd ./cross && $(MAKE)


.PHONY: ./elf-loader/libloader.a
./elf-loader/libloader.a:
	@cd ./elf-loader/ && $(MAKE)

.PHONY: all clean


clean:
	rm -f example-cpu example-cpu.o

