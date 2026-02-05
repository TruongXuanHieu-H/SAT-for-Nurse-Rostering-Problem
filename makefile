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

# ================================
# PBLib
# ================================
PBLIB_INC   := /usr/local/include
PBLIB_LIB_DIR := /usr/local/lib
PBLIB_LIB   := -lpb

INCLUDES := -I$(CADICAL_INC) -I$(PBLIB_INC)

# ================================
# Source files
# ================================
SOURCES := \
	encoder/nrp_encoder_bdd.cpp \
	encoder/nrp_encoder_scl.cpp \
	encoder/sat_solver_cadical.cpp \
	encoder/sat_solver.cpp \
	encoder/nrp_solver.cpp \
	encoder/var_handler.cpp \
	utils/version.cpp \
	utils/usage.cpp \
	utils/signal_handler.cpp \
	utils/pid_manager.cpp \
	global_data.cpp \
	interface.cpp 

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
	$(CXX) $(CXXFLAGS) $^ -L$(CADICAL_LIB_DIR) $(CADICAL_LIB) -L$(PBLIB_LIB_DIR) $(PBLIB_LIB) -o $@

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
