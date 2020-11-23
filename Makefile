# export PATH := /Users/dennisren/arm-none-eabi/bin:$(PATH)
PYTHONPATH=/Applications/MBEDCLI.app/Contents/Resources/miniconda/bin/python
MBEDCLI=/Applications/MBEDCLI.app/Contents/Resources/miniconda/bin/mbed
MBEDOS=mbed-os
MBEDARGS=compile --toolchain GCC_ARM
BUILDPATH=$(CURDIR)/BUILD
DISTPATH=$(CURDIR)/DIST
PROFILEPATH=$(CURDIR)/profiles
DOCPATH=$(CURDIR)/docs/manual
.PHONY: dev deploy
all: dev deploy

dev:
	@echo "making for dev"
	mbed $(MBEDARGS) --target NUCLEO_F446RE --profile develop.json
	mkdir -p $(DISTPATH)
	mkdir -p $(DISTPATH)/DEV/
	cp $(BUILDPATH)/NUCLEO_F446RE/GCC_ARM-DEVELOP/possum_rta.bin $(DISTPATH)/DEV/
	cp $(BUILDPATH)/NUCLEO_F446RE/GCC_ARM-DEVELOP/possum_rta.elf $(DISTPATH)/DEV/

deploy:
	cp $(DISTPATH)/DEV/possum_rta.bin /Volumes/NODE_F446RE
clean:
	rm -rf $(BUILDPATH)
