### Makefile --- 
## 
## Filename     : Makefile
## Description  : Makefile for the MCP23017 module.
## Author       : Christophe Burki
## Maintainer   : Christophe Burki
## Created      : Sun Jun 22 11:51:00 2014
## Version      : 1.0.0
## Last-Updated : Wed Jul 16 12:31:18 2014 (7200 CEST)
##           By : Christophe Burki
##     Update # : 6
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

# test_mcp23017 : make PROG=test_mcp23017
# test_int_mcp23017 : make PROG=test_int_mcp23017

MODULE := module_mcp23017

ifndef PROG
TARGET := test_mcp23017
SOURCES := $(MODULE).cpp test_mcp23017.c
endif

ifeq ($(PROG), test_mcp23017)
TARGET := test_mcp23017
SOURCES := $(MODULE).cpp test_mcp23017.c
endif

ifeq ($(PROG), test_int_mcp23017)
TARGET := test_int_mcp23017
SOURCES := $(MODULE).cpp test_int_mcp23017.c
endif

include ../Config.mk
include $(GNUBLINMKDIR)/gnublin.mk

######################################################################
### Makefile ends here
