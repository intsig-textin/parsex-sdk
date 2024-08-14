CXX = c++
CXXFLAGS = -O3 -Wall -shared -std=c++17 -fPIC

# Detect current Python version
PYTHON_VERSION = $(shell python3 -c 'import sys; print(f"{sys.version_info.major}.{sys.version_info.minor}")')
PYTHON_INCLUDE = $(shell python3 -c 'from distutils.sysconfig import get_python_inc; print(get_python_inc())')
PYTHON_LIB = $(shell python3-config --prefix)/lib
PYBIND11_INCLUDE = $(shell python3 -m pybind11 --includes)

INCLUDES = -I$(PYTHON_INCLUDE) -I../third_party/include/ $(PYBIND11_INCLUDE)

TARGET_DIR = ../../python_sdk
TARGET = $(TARGET_DIR)/textin_parser$(shell python3-config --extension-suffix)

SRCS = pybind_parser.cpp ../src/TextInPaserEngine.cpp

# Default target
all: $(TARGET)

# Target to build the shared object
$(TARGET): $(SRCS)
	@echo "Building with Python $(PYTHON_VERSION)..."
	mkdir -p $(TARGET_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $^ -o $@ -L$(PYTHON_LIB) -lpython$(PYTHON_VERSION)

# Clean up
clean:
	rm -f $(TARGET)
