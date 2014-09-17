### Makefile --- 
## 
## Filename     : Makefile
## Description  : Makefile for the MCP230XX module.
## Author       : Christophe Burki
## Maintainer   : Christophe Burki
## Created      : Sun Jun 22 11:51:00 2014
## Version      : 1.0.0
## Last-Updated : Wed Sep  3 17:12:31 2014 (7200 CEST)
##           By : Christophe Burki
##     Update # : 61
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

# test_mcp23017 : make TARGET=test_mcp23017
# test_int_mcp23017 : make TARGET=test_int_mcp23017

MODULES := module_mcp230xx module_mcp23017 module_mcp23009
MODOBJECTS := $(addsuffix .o, $(MODULES))
SOURCES := $(addsuffix .cpp, $(MODULES))
LIBRARY := gnublin_module_mcp230xx.a

ifndef TARGET
TARGET := test_mcp23017
endif

SOURCES += $(TARGET).c


include ../Config.mk
include $(GNUBLINMKDIR)/gnublin.mk


lib : $(MODOBJECTS)
	$(AR) rcs $(LIBRARY) $(MODOBJECTS)

python-module :: $(MODOBJECTS)
	@echo "%module gnublin_module_mcp230xx" > gnublin_module_mcp230xx.i
	@echo "%include \"std_string.i\"" >> gnublin_module_mcp230xx.i
	@echo "%{" >> gnublin_module_mcp230xx.i
	@echo "#include \"module_mcp230xx.h\"" >> gnublin_module_mcp230xx.i
	@echo "#include \"module_mcp23017.h\"" >> gnublin_module_mcp230xx.i
	@echo "#include \"module_mcp23009.h\"" >> gnublin_module_mcp230xx.i
	@echo "%}" >> gnublin_module_mcp230xx.i
	@echo "#define BOARD $(BOARD)" >> gnublin_module_mcp230xx.i
	@echo "%include \"module_mcp230xx.h\"" >> gnublin_module_mcp230xx.i
	@echo "%include \"module_mcp23017.h\"" >> gnublin_module_mcp230xx.i
	@echo "%include \"module_mcp23009.h\"" >> gnublin_module_mcp230xx.i
	swig2.0 -c++ -python gnublin_module_mcp230xx.i
	$(GCC) $(CPPFLAGS) -fpic -I $(GNUBLINAPIDIR)/python2.7/ -c gnublin_module_mcp230xx_wrap.cxx
	$(GCC) $(CPPFLAGS) -fpic -c module_mcp230xx.cpp
	$(GCC) $(CPPFLAGS) -fpic -c module_mcp23017.cpp
	$(GCC) $(CPPFLAGS) -fpic -c module_mcp23009.cpp
	$(GCC) -shared gnublin_module_mcp230xx_wrap.o $(MODOBJECTS) $(GNUBLINAPIDIR)/gnublin.o -o _gnublin_module_mcp230xx.so


######################################################################
### Makefile ends here
