CXX=c++
AR=ar
CFLAGS=-g -Isrc
CXXFLAGS=-std=c++11
LFLAGS=

BIN_DIR=bin
LIB_DIR=lib

.PHONY: bootstrap
bootstrap:
	@$(MAKE) all

include mk/mongoosemm.Mk
include mk/example.Mk
-include depends.Mk

.PHONY: all
all: $(BIN_DIR) $(LIB_DIR) $(MONGOOSEMM_LIB) $(EXAMPLE_BIN)

.PHONY: help
help:
	@echo ""
	@echo "all     - Build entire project"
	@echo "clean   - Clean up project"
	@echo "clean_o - Clean up just the generated objects"
	@echo "help    - This help menu"
	@echo ""

$(BIN_DIR):
	mkdir $@

$(LIB_DIR):
	mkdir $@

.PHONY: clean
clean:
	@$(MAKE) clean_o
	rm -f $(EXAMPLE_BIN)
	rm -f $(MONGOOSEMM_LIB)
	rm -rf $(LIB_DIR)
	rm -rf $(BIN_DIR)

.PHONY: clean_o
clean_o:
	rm -f $(MONGOOSEMM_OBJ)
	rm -f $(EXAMPLE_OBJ)
	rm -f depends.Mk
