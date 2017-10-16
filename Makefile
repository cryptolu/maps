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
