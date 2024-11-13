all : debug

build:
	~/.platformio/penv/bin/pio debug -v

flash : build
	gdb -x gdbinit -ex 'load' -ex 'detach' -ex 'quit' .pio/build/genericCH32V003F4P6/firmware.elf

debug: build
	gdb -x gdbinit .pio/build/genericCH32V003F4P6/firmware.elf

clean:
	~/.platformio/penv/bin/pio run --target=clean
