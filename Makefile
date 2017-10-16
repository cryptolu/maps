################################################################################
#
# University of Luxembourg
# Laboratory of Algorithmics, Cryptology and Security (LACS)
#
# arm_v7m_leakage simulator
#
# Copyright (C) 2017 University of Luxembourg
#
# Written in 2017 by Yann Le Corre <yann.lecorre@uni.lu>
#
# This simulator is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
#
# It is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, see <http://www.gnu.org/licenses/>.
#
################################################################################
################################################################################
#
# Re-generate data 
#
################################################################################

# remake everything
.PHONY: all
all: sim algo

# Make simulator
.PHONY: sim
sim: ./libsim
	$(MAKE) -C $</build clean
	$(MAKE) -C $</build cleaninstall
	$(MAKE) -C $</build install


# Algorithms
ALGO_DIRS := $(wildcard sec_*/.)
algo: $(ALGO_DIRS)
.PHONY: algo $(ALGO_DIRS)

$(ALGO_DIRS):
	echo $@
	$(MAKE) -C $@/sim/build clean
	$(MAKE) -C $@/sim/build
	$(MAKE) -C $@/fw/build clean
	$(MAKE) -C $@/fw/build data
