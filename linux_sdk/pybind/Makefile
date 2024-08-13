CXX = c++
CXXFLAGS = -O3 -Wall -shared -std=c++17 -fPIC

PYTHON_VERSION = 3.12
PYTHON_INCLUDE = /opt/homebrew/opt/python@$(PYTHON_VERSION)/Frameworks/Python.framework/Versions/$(PYTHON_VERSION)/include/python$(PYTHON_VERSION)
PYTHON_LIB = /opt/homebrew/opt/python@$(PYTHON_VERSION)/Frameworks/Python.framework/Versions/$(PYTHON_VERSION)/lib
PYBIND11_INCLUDE = $(shell python3 -m pybind11 --includes)

INCLUDES = -I$(PYTHON_INCLUDE) -I../third_party/include/ $(PYBIND11_INCLUDE)

TARGET_DIR = ../../python
TARGET = $(TARGET_DIR)/textin_parser$(shell python3-config --extension-suffix)

SRCS = pybind_parser.cpp ../src/TextInPaserEngine.cpp

$(TARGET): $(SRCS)
	mkdir -p $(TARGET_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $^ -o $@ -L$(PYTHON_LIB) -lpython$(PYTHON_VERSION)

clean:
	rm -f $(TARGET)
