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
# Top-level makefile (mainly used for maintenance)
#
################################################################################

ALGO_DIRS := $(wildcard sec_*/.)

.PHONY: compile_fw
compile_fw:
	@for dir in $(ALGO_DIRS); do \
		echo "-- compiling FW in $$dir"; \
		$(MAKE) -C $$dir/fw/build; \
	done

.PHONY: compile_sim
compile_sim: compile_lib
	@for dir in $(ALGO_DIRS); do \
		echo "-- compiling SIM in $$dir"; \
		$(MAKE) -C $$dir/sim/build; \
	done

.PHONY: compile_lib
compile_lib:
	echo "-- compiling library"; \
	$(MAKE) -C libsim/build install; \

.PHONY: check
check: compile_lib compile_sim compile_fw
	@for dir in $(ALGO_DIRS); do \
		echo "-- checking $$dir"; \
		$(MAKE) -C $$dir/fw/build check; \
	done

.PHONY: clean
clean:
	@for dir in $(ALGO_DIRS); do \
		echo "-- cleaning directory $$dir"; \
		$(MAKE) -C $$dir/sim/build clean; \
		$(MAKE) -C $$dir/fw/build clean; \
	done

