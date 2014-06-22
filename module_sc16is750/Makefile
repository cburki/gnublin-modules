### Makefile --- 
## 
## Filename     : Makefile
## Description  : Makefile for the SC16IS750 module.
## Author       : Christophe Burki
## Maintainer   : Christophe Burki
## Created      : Sun Jun 22 11:51:54 2014
## Version      : 1.0.0
## Last-Updated : Sun Jun 22 11:52:16 2014 (7200 CEST)
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
## ;; Floor, Boston, MA 02110-1301, USA.TARGET := test_sc16is750

## 
######################################################################
## 
### Code         :

SOURCES := module_sc16is750.cpp test_sc16is750.c

include ../Config.mk
include $(GNUBLINMKDIR)/gnublin.mk


python-module: $(OBJECTS)
	@echo "%module gnublin_module_sc16is750" > gnublin_module_sc16is750.i
	@echo "%include \"std_string.i\"" >> gnublin_module_sc16is750.i
	@echo "%{" >> gnublin_module_sc16is750.i
	@echo "#include \"module_sc16is750.h\"" >> gnublin_module_sc16is750.i
	@echo "%}" >> gnublin_module_sc16is750.i
	@echo "#define BOARD $(BOARD)" >> gnublin_module_sc16is750.i
	@echo "%include \"module_sc16is750.h\"" >> gnublin_module_sc16is750.i
	swig2.0 -c++ -python gnublin_module_sc16is750.i
	$(GCC) $(CPPFLAGS) -fpic -I $(GNUBLINAPIDIR)/python2.7/ -c gnublin_module_sc16is750_wrap.cxx
	$(GCC) $(CPPFLAGS) -fpic -c module_sc16is750.cpp
	$(GCC) -shared gnublin_module_sc16is750_wrap.o module_sc16is750.o $(GNUBLINAPIDIR)/gnublin.o -o _gnublin_module_sc16is750.so

python-module-clean : clean
	rm -f *.i *.py *.pyc *_wrap.cxx _*.so

######################################################################
### Makefile ends here
