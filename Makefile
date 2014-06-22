### Makefile --- 
## 
## Filename     : Makefile
## Description  : Top level make file for the gnublin modules.
## Author       : Christophe Burki
## Maintainer   : Christophe Burki 
## Created      : Fri Jun 13 19:48:42 2014
## Version      : 1.0.0
## Last-Updated : Sun Jun 22 11:45:45 2014 (7200 CEST)
##           By : Christophe Burki
##     Update # : 34
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

MODULES := mcp23017 sc16is750 hd44780

all: ; $(foreach module,$(MODULES),(cd module_$(module); make) &&):

clean: ; $(foreach module,$(MODULES),(cd module_$(module); make clean) &&):

distclean: ; $(foreach module,$(MODULES),(cd module_$(module); make distclean) && (cd module_$(module); make python-module-clean) &&):
	rm -f *~

python-module: ; $(foreach module,$(MODULES),(cd module_$(module); make python-module) &&):

python-module-clean: ; $(foreach module,$(MODULES),(cd module_$(module); make python-module-clean) &&):

######################################################################
### Makefile ends here
