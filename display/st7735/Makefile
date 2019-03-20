# ;
# Makefile:
#	tft_st7735 - 1,8" TFT (Sainsmart) library for the Raspberry Pi. Uses wiringPi
#
#	Copyright (c) 2014 Jürgen Schick
#################################################################################
# This file is part of tft_st7735:
#
#    tft_st7735 is free software: you can redistribute it and/or modify
#    it under the terms of the GNU Lesser General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    tft_st7735 is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU Lesser General Public License for more details.
#
#    You should have received a copy of the GNU Lesser General Public License
#    along with tft_st7735.  If not, see <http://www.gnu.org/licenses/>.
#################################################################################

DYN_VERS_MAJ=1
DYN_VERS_MIN=0

VERSION=$(DYN_VERS_MAJ).$(DYN_VERS_MIN)
DESTDIR=/usr
PREFIX=/local

STATIC =libtft_st7735.a
DYNAMIC=libtft_st7735.so.$(VERSION)

#DEBUG	= -g -O0
DEBUG	= -O2
CC	= g++
INCLUDE	= -I.
DEFS	= -D_GNU_SOURCE
CFLAGS	= $(DEBUG) $(DEFS) -Wformat=2 -Wall -Winline $(INCLUDE) -pipe -fPIC
SOURCEDIR = src

LIBS    =

# Should not alter anything below this line
###############################################################################

SRC	=	$(SOURCEDIR)/tft_st7735.cpp						\
        $(SOURCEDIR)/tft_field.cpp $(SOURCEDIR)/tft_manager.cpp  \
		$(SOURCEDIR)/glcdfont.cpp

OBJ	=	$(SRC:.cpp=.o)

all:		$(DYNAMIC)

static:		$(STATIC)

$(STATIC):	$(OBJ)
	@echo "[Link (Static)]"
	@ar rcs $(STATIC) $(OBJ)
	@ranlib $(STATIC)
#	@size   $(STATIC)

$(DYNAMIC):	$(OBJ)
	@echo "[Link (Dynamic)]"
	@$(CC) -shared -Wl,-soname,libtft_st7735.so -o libtft_st7735.so.$(VERSION) -lpthread $(OBJ)
#	@$(CC) -shared -Wl,-soname,libtft_st7735.so -o libtft_st7735.so.$(VERSION)

.cpp.o:
	@echo [Compile] $<
	@$(CC) -c $(CFLAGS) $< -o $@

.PHONEY:	clean
clean:
	@echo "[Clean]"
	@rm -f $(SOURCEDIR)/$(OBJ) $(OBJ_I2C) *~ core tags Makefile.bak libtft_st7735.*

.PHONEY:	tags
tags:	$(SRC)
	@echo [ctags]
	@ctags $(SRC)


.PHONEY:	install-headers
install-headers:
	@echo "[Install Headers]"
	@install -m 0755 -d			$(DESTDIR)$(PREFIX)/include
	@install -m 0644 $(SOURCEDIR)/tft_st7735.h		$(DESTDIR)$(PREFIX)/include
	@install -m 0644 $(SOURCEDIR)/tft_field.h		$(DESTDIR)$(PREFIX)/include
	@install -m 0644 $(SOURCEDIR)/tft_manager.h		$(DESTDIR)$(PREFIX)/include

.PHONEY:	install
install:	$(DYNAMIC) install-headers
	@echo "[Install Dynamic Lib]"
	@install -m 0755 -d						$(DESTDIR)$(PREFIX)/lib
	@install -m 0755 libtft_st7735.so.$(VERSION)			$(DESTDIR)$(PREFIX)/lib/libtft_st7735.so.$(VERSION)
	@ln -sf $(DESTDIR)$(PREFIX)/lib/libtft_st7735.so.$(VERSION)	$(DESTDIR)/lib/libtft_st7735.so
	@ldconfig

.PHONEY:	install-static
install-static:	$(STATIC) install-headers
	@echo "[Install Static Lib]"
	@install -m 0755 -d			$(DESTDIR)$(PREFIX)/lib
	@install -m 0755 libtft_st7735.a		$(DESTDIR)$(PREFIX)/lib

.PHONEY:	uninstall
uninstall:
	@echo "[UnInstall]"
	@rm -f $(DESTDIR)$(PREFIX)/include/tft_st7735.h
	@rm -f $(DESTDIR)$(PREFIX)/include/tft_field.h
	@rm -f $(DESTDIR)$(PREFIX)/include/tft_manager.h
	@rm -f $(DESTDIR)$(PREFIX)/lib/libtft_st7735.*
	@ldconfig


.PHONEY:	depend
depend:
	makedepend -Y $(SRC) $(SRC_I2C)

# DO NOT DELETE

tft_st7735.o: tft_st7735.h
tft_field.o: tft_st7735.h tft_field.h
tft_manager.o: tft_st7735.h tft_field.h
