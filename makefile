# ================================
# Project directories
# ================================
SRCDIR  := src
OBJDIR  := build/obj
BINDIR  := build

TARGET  := $(BINDIR)/nrp_enc

# ================================
# Compiler
# ================================
CXX       := g++
STANDARD  := -std=c++23

# ================================
# Compiler flags
# ================================
IGNORE_ASSERTVARS := -Wno-unused-but-set-variable
CXXFLAGS := -Wall -Wextra -Werror -O3 -DNDEBUG $(STANDARD) $(IGNORE_ASSERTVARS)

# ================================
# CaDiCaL
# ================================
CADICAL_INC     := ./solver/cadical
CADICAL_LIB_DIR := ./solver/cadical
CADICAL_LIB     := -lcadical

INCLUDES := -I$(CADICAL_INC)

# ================================
# Source files
# ================================
SOURCES := \
	interface.cpp \
	global_data.cpp \
	utils/version.cpp \
	utils/usage.cpp \
	utils/signal_handler.cpp \
	utils/pid_manager.cpp

# ================================
# Object files
# ================================
OBJECTS := $(SOURCES:%.cpp=$(OBJDIR)/%.o)

# ================================
# Default target
# ================================
.PHONY: all
all: $(TARGET)

# ================================
# Link
# ================================
$(TARGET): $(OBJECTS)
	@mkdir -p $(BINDIR)
	$(CXX) $(CXXFLAGS) $^ -L$(CADICAL_LIB_DIR) $(CADICAL_LIB) -o $@

# ================================
# Compile rules
# ================================
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# ================================
# Clean
# ================================
.PHONY: clean
clean:
	rm -rf $(OBJDIR) $(TARGET) *.a *~ *.out

# ================================
# Archive
# ================================
.PHONY: tar
tar:
	tar cfv nrp_enc.tar \
		$(SRCDIR) \
		makefile \
		$(CADICAL_LIB_DIR)/*.a \
		$(CADICAL_LIB_DIR)/*.hpp
