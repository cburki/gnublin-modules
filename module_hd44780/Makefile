### Makefile --- 
## 
## Filename     : Makefile
## Description  : Makefile for the hd44780 module.
## Author       : Christophe Burki
## Maintainer   : Christophe Burki
## Created      : Sun Jun 22 11:50:16 2014
## Version      : 1.0.0
## Last-Updated : Sun Aug 31 20:02:14 2014 (7200 CEST)
##           By : Christophe Burki
##     Update # : 13
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
MODULE := module_hd44780
SOURCES := $(MODULE).cpp test_hd44780.c

include ../Config.mk
include $(GNUBLINMKDIR)/gnublin.mk

CPPFLAGS += -I../module_mcp230xx -I../module_sc16is7x0
OBJECTS += ../module_mcp230xx/module_mcp230xx.o ../module_mcp230xx/module_mcp23017.o ../module_sc16is7x0/module_sc16is7x0.o ../module_sc16is7x0/module_sc16is750.o

######################################################################
### Makefile ends here
