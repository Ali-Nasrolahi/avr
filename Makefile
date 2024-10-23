all:
	cmake -Bbuild && cmake --build build -j 10

fresh:
	rm -rf build/* && cmake -Bbuild --fresh && cmake --build build -j 10

upload_328: all
	cmake --build build --target upload_avr-328p

upload_32a: all
	cmake --build build --target upload_avr-32a

isp:
	avrdude -v -p atmega328p -c arduino -P /dev/ttyUSB0 -b 57600 -D -U flash:w:./isp/ArduinoISP.ino.hex:i

clean:
	cmake --build build --target clean

.PHONY: isp
