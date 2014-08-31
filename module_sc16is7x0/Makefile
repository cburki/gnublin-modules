### Makefile --- 
## 
## Filename     : Makefile
## Description  : Makefile for the SC16IS750 module.
## Author       : Christophe Burki
## Maintainer   : Christophe Burki
## Created      : Sun Jun 22 11:51:54 2014
## Version      : 1.0.0
## Last-Updated : Sun Aug 31 16:21:19 2014 (7200 CEST)
##           By : Christophe Burki
##     Update # : 15
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
## ;; Floor, Boston, MA 02110-1301, USA.TARGET := test_sc16is750

## 
######################################################################
## 
### Code         :

TARGET := test_sc16is750
MODULES := module_sc16is7x0 module_sc16is740 module_sc16is750
MODOBJECTS := $(addsuffix .o, $(MODULES))
SOURCES := $(addsuffix .cpp, $(MODULES))
LIBRARY := gnublin_module_sc16is7x0.a

SOURCES += test_sc16is750.c

include ../Config.mk
include $(GNUBLINMKDIR)/gnublin.mk


lib : $(MODOBJECTS)
	$(AR) rcs $(LIBRARY) $(MODOBJECTS)

python-module:
	@echo "%module gnublin_module_sc16is7x0" > gnublin_module_sc16is7x0.i
	@echo "%include \"std_string.i\"" >> gnublin_module_sc16is7x0.i
	@echo "%{" >> gnublin_module_sc16is7x0.i
	@echo "#include \"module_sc16is7x0.h\"" >> gnublin_module_sc16is7x0.i
	@echo "#include \"module_sc16is740.h\"" >> gnublin_module_sc16is7x0.i
	@echo "#include \"module_sc16is750.h\"" >> gnublin_module_sc16is7x0.i
	@echo "%}" >> gnublin_module_sc16is7x0.i
	@echo "#define BOARD $(BOARD)" >> gnublin_module_sc16is7x0.i
	@echo "%include \"module_sc16is7x0.h\"" >> gnublin_module_sc16is7x0.i
	@echo "%include \"module_sc16is740.h\"" >> gnublin_module_sc16is7x0.i
	@echo "%include \"module_sc16is750.h\"" >> gnublin_module_sc16is7x0.i
	swig2.0 -c++ -python gnublin_module_sc16is7x0.i
	$(GCC) $(CPPFLAGS) -fpic -I $(GNUBLINAPIDIR)/python2.7/ -c gnublin_module_sc16is7x0_wrap.cxx
	$(GCC) $(CPPFLAGS) -fpic -c module_sc16is7x0.cpp
	$(GCC) $(CPPFLAGS) -fpic -c module_sc16is740.cpp
	$(GCC) $(CPPFLAGS) -fpic -c module_sc16is750.cpp
	$(GCC) -shared gnublin_module_sc16is7x0_wrap.o $(MODOBJECTS) $(GNUBLINAPIDIR)/gnublin.o -o _gnublin_module_sc16is7x0.so

######################################################################
### Makefile ends here
