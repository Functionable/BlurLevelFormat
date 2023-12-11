GXX=g++

BUILD_DIR=../build
MAIN_OBJECTS_DIR=$(BUILD_DIR)/objects
TEST_OBJECTS_DIR=$(BUILD_DIR)/test_objects
TEST_BINARY_DIR=$(BUILD_DIR)/test_binaries

TEST_DIR=../tests
CXXFLAGS=-no-pie -g -lz -Werror=return-type -std=c++17 
INCLUDES=-I ../include -I .

rwildcard=$(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))
LIB_OBJECTS=$(filter-out $(MAIN_OBJECTS_DIR)/main.o,$(call rwildcard,$(MAIN_OBJECTS_DIR), *.o))

TESTSUBDIRS = $(call rwildcard,$(TEST_DIR),*.cpp)
TESTSRCDIRS = $(shell find $(TEST_DIR) -type d)
TESTDIRS = $(OBJFOLDER) $(subst $(TEST_DIR),$(TEST_OBJECTS_DIR),$(TESTSRCDIRS))
BINDIRS = $(OBJFOLDER) $(subst $(TEST_DIR),$(TEST_BINARY_DIR),$(TESTSRCDIRS))
TESTOBJECTNAMES := $(subst $(TEST_DIR),$(TEST_OBJECTS_DIR),$(TESTSUBDIRS:%.cpp=%.o))
TESTBINARYNAMES := $(subst $(TEST_DIR),$(TEST_BINARY_DIR),$(TESTSUBDIRS:%.cpp=%.a))

yellow=\033[1;33m
green=\033[1;32m
red=\033[1;31m
no_color=\033[0m

PASS_DIALOG="$(green)PASSED$(no_color)"
FAIL_DIALOG="$(red)FAILED$(no_color)"

TESTS_PASSED=$(PASS_DIALOG)

build_dirs: $(MAIN_OBJECTS_DIR) $(TEST_OBJECTS_DIR) $(TEST_BINARY_DIR)
source_tree: $(TESTDIRS) $(BINDIRS)
tests: $(TESTOBJECTNAMES) $(TESTBINARYNAMES)

$(MAIN_OBJECTS_DIR):
	@echo -e "$(no_color)Making directory $(yellow)'$@' $(no_color)... $(no_color)"
	@mkdir -p $@

$(TEST_OBJECTS_DIR):
	@echo -e "$(no_color)Making directory $(yellow)'$@' $(no_color)... $(no_color)"
	@mkdir -p $@

$(TEST_BINARY_DIR):
	@echo -e "$(no_color)Making directory $(yellow)'$@' $(no_color)... $(no_color)"
	@mkdir -p $@

$(TEST_BINARY_DIR)/%.a: $(TEST_OBJECTS_DIR)/%.o
	@echo -e "$(no_color)Running test '$(yellow)$@$(no_color)'..."
	@$(GXX) $(CXXFLAGS) $< $(TEST_OBJECTS_DIR)/test.o $(LIB_OBJECTS) $(INCLUDES) -o $@
	@echo $@: >> test.log
	@$@ >> test.log && echo -e $@ $(PASS_DIALOG) || { echo -e $@ $(FAIL_DIALOG); exit 1; }
	@echo TEST END >> test.log

$(TEST_BINARY_DIR)/test.a: ;

$(TEST_OBJECTS_DIR)/%.o: $(TEST_DIR)/%.cpp
	@echo -e "$(no_color)Building test '$(yellow)$@$(no_color)'..."
	@$(GXX) $(INCLUDES) $(CXXFLAGS) -c $< -o $@

$(TEST_OBJECTS_DIR)/%: $(TEST_DIR)/%
	@echo -e "$(no_color)Creating $(yellow)$@ $(no_color)from the source tree... $(no_color)"
	@mkdir -p $@

$(TEST_BINARY_DIR)/%: $(TEST_DIR)/%
	@echo -e "$(no_color)Creating $(yellow)$@ $(no_color)from the source tree... $(no_color)"
	@mkdir -p $@

clean: 
	@rm -rf $(TEST_BINARY_DIR)
	@rm -rf $(TEST_OBJECTS_DIR)
	@echo '' > test.log

test: clean build_dirs source_tree tests
	@echo -e ALL TESTS $(TESTS_PASSED)