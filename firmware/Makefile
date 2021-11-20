include ../include/generated/variables.mak
include $(SOC_DIRECTORY)/software/common.mak

ifeq ($(CPU),blackparrot)
BP_LIBS =  -L$(BP_LITEX_SOFTWARE)
BP_FLAGS = -lgcc
endif

CFLAGS += -I$(FW_DIRECTORY)/include
CFLAGS += -DI2C_FREQ_HZ=400

vpath %.c  $(FW_DIRECTORY)
vpath %.S  $(FW_DIRECTORY)

OBJECTS =  	crt0.o \
			main.o	

all: fw.bin
	$(PYTHON) -m litex.soc.software.memusage fw.elf $(CURDIR)/../include/generated/regions.ld $(TRIPLE)

%.bin: %.elf
	$(OBJCOPY) -O binary $< $@
ifneq ($(OS),Windows_NT)
	chmod -x $@
endif
ifeq ($(CPUENDIANNESS),little)
	$(PYTHON) -m litex.soc.software.mkmscimg $@ --little
else
	$(PYTHON) -m litex.soc.software.mkmscimg $@
endif

fw.elf: $(FW_DIRECTORY)/linker.ld $(OBJECTS)

vpath %.a $(PACKAGES:%=../%)

%.elf: $(LIBS:%=%.a)
	$(CC) $(LDFLAGS) -T $(FW_DIRECTORY)/linker.ld -N -o $@ \
		$(OBJECTS) \
		$(PACKAGES:%=-L../%) \
		-Wl,--whole-archive \
		-Wl,--gc-sections \
		$(LIBS:lib%=-l%)
		
ifneq ($(OS),Windows_NT)
	chmod -x $@
endif

# pull in dependency info for *existing* .o files
-include $(OBJECTS:.o=.d)

%.o: %.c
	$(compile)

%.o: %.S
	$(assemble)


clean:
	$(RM) $(OBJECTS) fw.elf fw.bin .*~ *~

.PHONY: all clean
