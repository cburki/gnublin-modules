### Makefile --- 
## 
## Filename     : Makefile
## Description  : Top level make file for the gnublin modules.
## Author       : Christophe Burki
## Maintainer   : Christophe Burki 
## Created      : Fri Jun 13 19:48:42 2014
## Version      : 1.0.0
## Last-Updated : Sun Aug 31 20:19:39 2014 (7200 CEST)
##           By : Christophe Burki
##     Update # : 40
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

MODULES := module_mcp230xx module_sc16is7x0 module_hd44780

all: ; $(foreach module,$(MODULES),(cd $(module); make) &&):

clean: ; $(foreach module,$(MODULES),(cd $(module); make clean) &&):

distclean: ; $(foreach module,$(MODULES),(cd $(module); make distclean) &&):
	rm -f *~

publish: ; $(foreach module,$(MODULES),(cd $(module); make publish) &&):

python-module: ; $(foreach module,$(MODULES),(cd $(module); make python-module) &&):

python-module-clean: ; $(foreach module,$(MODULES),(cd $(module); make python-module-clean) &&):

######################################################################
### Makefile ends here
