TOOLS_PATH = "./tools/script/linux/install/"
DEPS_DIR = "./pf/dependencies/"
PLUGINS_DIR = "./pf/plugin/"
CUR_DIR = $(shell pwd)

simple: core plugins
	cd $(CUR_DIR)
	cd applications/pf_simple/proj.linux && cmake -Wno-dev ./ && make
	cp pf/dll/*.so applications/bin/
	cp pf/core/dll/*.so applications/bin/
	cp pf/dependencies/dll/*.so applications/bin/

env:
	cd $(CUR_DIR)
	cd $(TOOLS_PATH) && bash install_environment.sh --full --sudo

deps:
	cd $(CUR_DIR)
	cd $(DEPS_DIR)iconv/proj.linux && cmake -Wno-dev ./ && make
	cd $(CUR_DIR)
	cd $(DEPS_DIR)lua/5.3.0/proj.linux && cmake -Wno-dev ./ && make

plugins:
	cd $(CUR_DIR)
	cd $(PLUGINS_DIR)lua/proj.linux && cmake -Wno-dev ./ && make


core: env deps
	cd $(CUR_DIR)
	cd pf/core/proj.linux && cmake -Wno-dev ./ && make

all: simple

linux: simple
