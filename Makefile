### Makefile --- 
## 
## Filename     : Makefile
## Description  : Top level make file for the gnublin modules.
## Author       : Christophe Burki
## Maintainer   : Christophe Burki 
## Created      : Fri Jun 13 19:48:42 2014
## Version      : 1.0.0
## Last-Updated : Sun Aug 31 19:51:38 2014 (7200 CEST)
##           By : Christophe Burki
##     Update # : 38
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

include Config.mk

MODULES := mcp230xx sc16is7x0 hd44780 atmega328p

all: ; $(foreach module,$(MODULES),(cd module_$(module); make) &&):

clean: ; $(foreach module,$(MODULES),(cd module_$(module); make clean) &&):

distclean: ; $(foreach module,$(MODULES),(cd module_$(module); make distclean) && (cd module_$(module); make python-module-clean) &&):
	rm -f *~

publish: ; $(foreach module,$(MODULES),(cd module_$(module); make publish) &&):

python-module: ; $(foreach module,$(MODULES),(cd module_$(module); make python-module) &&):

python-module-clean: ; $(foreach module,$(MODULES),(cd module_$(module); make python-module-clean) &&):

######################################################################
### Makefile ends here
