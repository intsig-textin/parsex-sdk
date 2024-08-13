CXX = c++
CXXFLAGS = -O3 -Wall -shared -std=c++17 -fPIC

PYTHON_VERSION = 3.6
PYTHON_INCLUDE = /usr/include/python3.6
LIB_PATH = /usr/lib/python3.6/config-3.6m-x86_64-linux-gnu
PYBIND11_INCLUDE = $(shell python3 -m pybind11 --includes)

INCLUDES = -I$(PYTHON_INCLUDE) -I../third_party/include/ -I/usr/include/python3.6m $(PYBIND11_INCLUDE)

TARGET_DIR = ../../python_sdk
TARGET = $(TARGET_DIR)/textin_parser$(shell python3-config --extension-suffix)

SRCS = pybind_parser.cpp ../src/TextInPaserEngine.cpp

$(TARGET): $(SRCS)
	mkdir -p $(TARGET_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $^ -o $@ -L$(LIB_PATH) $(LIB_PATH)/libpython3.6.so

clean:
	rm -f $(TARGET)
