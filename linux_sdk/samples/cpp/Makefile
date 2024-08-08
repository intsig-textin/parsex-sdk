COMMONPATH := $(shell pwd)/../..

CXX = g++
CXXFLAGS = -std=c++17 -I$(COMMONPATH)/third_party/include/ -I$(COMMONPATH)/include -I$(COMMONPATH)/src -I. -Wall -Wextra

ifdef DEBUG
CXXFLAGS += -DDEBUG
endif

BUILD_DIR = build

TARGET = test_program

SRCS = main.cpp $(COMMONPATH)/src/TextInPaserEngine.cpp

OBJS = $(SRCS:%.cpp=$(BUILD_DIR)/%.o)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS) $(LDFLAGS) $(LDLIBS)

$(BUILD_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR) $(TARGET)

.PHONY: clean
