.PHONY: all
all: iss

.PHONY: hardware
hardware:
	@cd hardware && $(MAKE)

.PHONY: iss
iss:
	@cd iss && $(MAKE)

.PHONY: clean-subdirs
clean-subdirs:
	@cd hardware && $(MAKE) clean
	@cd iss && $(MAKE) clean
	@cd software/cross/ && $(MAKE) clean
	@cd elf-loader/ && $(MAKE) clean
clean: clean-subdirs

ROOT=.
WITH_SDL=yesPlease
include $(ROOT)/Makefile.common

OWNFILES=iss/sc_main_iss.cpp iss/mb_wrapper.cpp iss/mb_wrapper.h \
	${wildcard hardware/*.h hardware/*.cpp} \

clang-format:
	$(CLANG_FORMAT) -i $(OWNFILES)
