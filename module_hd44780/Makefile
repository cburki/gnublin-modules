### Makefile --- 
## 
## Filename     : Makefile
## Description  : Makefile for the hd44780 module.
## Author       : Christophe Burki
## Maintainer   : Christophe Burki
## Created      : Sun Jun 22 11:50:16 2014
## Version      : 1.0.0
## Last-Updated : Sun Jun 22 11:50:44 2014 (7200 CEST)
##           By : Christophe Burki
##     Update # : 2
## URL          : 
## Keywords     : 
## Compatibility: 
## 
######################################################################
## 
### Commentary   : 
## 
## 
## 
######################################################################
## 
### Change log:
## 
## 
######################################################################
## 
## This program is free software; you can redistribute it and/or modify
## it under the terms of the GNU General Public License version 3 as
## published by the Free Software Foundation.
## 
## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
## 
## You should have received a copy of the GNU General Public License
## along with this program; see the file LICENSE.  If not, write to the
## Free Software Foundation, Inc., 51 Franklin Street, Fifth
## ;; Floor, Boston, MA 02110-1301, USA.

## 
######################################################################
## 
### Code         :

TARGET := test_hd44780
SOURCES := module_hd44780.cpp test_hd44780.c

include ../Config.mk
include $(GNUBLINMKDIR)/gnublin.mk

CPPFLAGS += -I../module_mcp23017
OBJECTS += ../module_mcp23017/module_mcp23017.o


python-module: $(OBJECTS)
	@echo "%module gnublin_module_hd44780" > gnublin_module_hd44780.i
	@echo "%include \"std_string.i\"" >> gnublin_module_hd44780.i
	@echo "%{" >> gnublin_module_hd44780.i
	@echo "#include \"module_hd44780.h\"" >> gnublin_module_hd44780.i
	@echo "%}" >> gnublin_module_hd44780.i
	@echo "#define BOARD $(BOARD)" >> gnublin_module_hd44780.i
	@echo "%include \"module_hd44780.h\"" >> gnublin_module_hd44780.i
	swig2.0 -c++ -python gnublin_module_hd44780.i
	$(GCC) $(CPPFLAGS) -fpic -I $(GNUBLINAPIDIR)/python2.7/ -c gnublin_module_hd44780_wrap.cxx
	$(GCC) $(CPPFLAGS) -fpic -c module_hd44780.cpp
	$(GCC) -shared gnublin_module_hd44780_wrap.o module_hd44780.o $(GNUBLINAPIDIR)/gnublin.o ../module_mcp23017/module_mcp23017.o -o _gnublin_module_hd44780.so

python-module-clean : clean
	rm -f *.i *.py *.pyc *_wrap.cxx _*.so

######################################################################
### Makefile ends here
