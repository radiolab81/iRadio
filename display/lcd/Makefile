# Makefile:
#	lcdpcf8574 - Drive HD44780 based LCD via two pins on Raspberry pi - I²C
#
#	Copyright (c) 2013 Prageeth Karunadheera. http://karunadheera.com/
#################################################################################
#    lcdpcf8574 is free software: you can redistribute it and/or modify
#    it under the terms of the GNU Lesser General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    lcdpcf8574 is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU Lesser General Public License for more details.
#
#    You should have received a copy of the GNU Lesser General Public License
#    along with wiringPi.  If not, see <http://www.gnu.org/licenses/>.
#################################################################################

displayd: liblcdpcf8574.so displayd.o
	$(CXX) -Wall -L. displayd.cpp -lrt -llcdpcf8574 -lwiringPi -lbcm2835 -o displayd
liblcdpcf8574.so: lcdpcf8574.cpp
	$(CXX) -fPIC -c lcdpcf8574.cpp -o lcdpcf8574.o
	$(CXX) -shared -Wl,-soname,liblcdpcf8574.so.1 -lwiringPi -o liblcdpcf8574.so.1.0 lcdpcf8574.o
	ln -sf liblcdpcf8574.so.1.0 liblcdpcf8574.so
	ln -sf liblcdpcf8574.so.1.0 liblcdpcf8574.so.1
install: liblcdpcf8574.so
	cp lcdpcf8574.h /usr/local/include/lcdpcf8574.h
	cp liblcdpcf8574.so.1.0 /usr/local/lib/
	ln -sf /usr/local/lib/liblcdpcf8574.so.1.0 /usr/local/lib/liblcdpcf8574.so
	ln -sf /usr/local/lib/liblcdpcf8574.so.1.0 /usr/local/lib/liblcdpcf8574.so.1
	ldconfig
clean:
	@rm *.o *.so *.so.*
