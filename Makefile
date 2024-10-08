# https://makefiletutorial.com/
# Thanks to Job Vranish (https://spin.atomicobject.com/2016/08/26/makefile-c-projects/)

# https://stackoverflow.com/questions/2483182/recursive-wildcards-in-gnu-make
rwildcard=$(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))

ifneq ($(OS), Windows_NT)
    TARGET_EXEC := bobob
	UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S), Linux)
        LIB_FLAGS := -lraylib
    endif
    ifeq ($(UNAME_S), Darwin) 
        LIB_FLAGS := -lraylib -Llib/Mac/lib -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo
        LIB_INCLUDE := -Ilib/Mac/include 
    endif
else
	TARGET_EXEC := bobob.exe
    LIB_FLAGS := -lraylib -Llib/Win/lib -lopengl32 -lgdi32 -lwinmm
    LIB_INCLUDE := -Ilib/Win/include 
endif

BUILD_DIR := ./build
SRC_DIRS := ./src
TEST_SRC_DIRS := ./test

# Find all the C and C++ files we want to compile
SRCS := $(call rwildcard, $(SRC_DIRS), *.cpp)
TEST_SRCS := $(filter-out $(SRC_DIRS)/main.cpp, $(call rwildcard, $(SRC_DIRS), *.cpp)) $(call rwildcard, $(TEST_SRC_DIRS), *.cpp)

# Prepends BUILD_DIR and appends .o to every src file
# As an example, ./your_dir/hello.cpp turns into ./build/./your_dir/hello.cpp.o
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
TEST_OBJS := $(TEST_SRCS:%=$(BUILD_DIR)/%.test.o)

# String substitution (suffix version without %).
# As an example, ./build/hello.cpp.o turns into ./build/hello.cpp.d
DEPS := $(OBJS:.o=.d)
TEST_DEPS := $(TEST_OBJS:.o=.d)

# Every folder in ./src will need to be passed to GCC so that it can find header files
INC_DIRS := $(sort $(dir $(call rwildcard, $(SRC_DIRS), *.h)))
# Add a prefix to INC_DIRS. So moduleA would become -ImoduleA. GCC understands this -I flag
INC_FLAGS := $(addprefix -I,$(INC_DIRS)) $(LIB_INCLUDE)

# The -MMD and -MP flags together generate Makefiles for us!
# These files will have .d instead of .o as the output.
CPPFLAGS := $(INC_FLAGS) -MMD -MP -pedantic -Wall -Wextra -std=c++23

# The final build step.
$(BUILD_DIR)/$(TARGET_EXEC): $(OBJS)
	$(CXX) $(OBJS) -o $@ $(LDFLAGS) $(LIB_FLAGS)
		
# Build step for C++ source
$(BUILD_DIR)/%.cpp.o: %.cpp
ifneq ($(OS), Windows_NT)
	mkdir -p $(dir $@)
else
	if not exist "$(subst /,\,$(dir $@))" mkdir $(subst /,\,$(dir $@))
endif
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

.PHONY: clean
clean:
ifneq ($(OS), Windows_NT)
	find $(BUILD_DIR)/* -type f | grep -v "imgui/*" | xargs rm -r
else
	rm -r $(BUILD_DIR)
endif

.PHONY: test
test: CPPFLAGS += -DTESTING
test: $(BUILD_DIR)/$(TARGET_EXEC)_test
	./$(BUILD_DIR)/$(TARGET_EXEC)_test

# final test build step
$(BUILD_DIR)/$(TARGET_EXEC)_test: $(TEST_OBJS)
	$(CXX) $(TEST_OBJS) -o $(BUILD_DIR)/$(TARGET_EXEC)_test $(LDFLAGS) $(LIB_FLAGS)
# Build step for C++ source files in the test target
$(BUILD_DIR)/%.cpp.test.o: %.cpp
ifneq ($(OS), Windows_NT)
	mkdir -p $(dir $@)
else
	if not exist "$(subst /,\,$(dir $@))" mkdir $(subst /,\,$(dir $@))
endif
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

# Include the .d makefiles. The - at the front suppresses the errors of missing
# Makefiles. Initially, all the .d files will be missing, and we don't want those
# errors to show up.
-include $(DEPS)
