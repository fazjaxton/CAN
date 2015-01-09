SOURCES=CAN.cpp CAN.h mcp2515.cpp mcp2515.h mcp2515_regs.h my_spi.h spi.cpp

all: doc mainpage.dox

doc: mainpage.dox doxyconfig $(SOURCES)
	doxygen doxyconfig

mainpage.dox: mainpage.txt maindoc.awk
	fold -s -w 70 < mainpage.txt | awk -f maindoc.awk > mainpage.dox

clean:
	rm -rf mainpage.dox doc

