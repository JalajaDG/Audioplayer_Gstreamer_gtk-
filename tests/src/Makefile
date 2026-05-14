# Test Makefile for building and running tests

CXX = g++
CXXFLAGS = -Wall -g -I../../include `pkg-config --cflags gtk+-3.0 gstreamer-1.0 gstreamer-pbutils-1.0`
LDFLAGS = `pkg-config --libs gtk+-3.0 gstreamer-1.0 gstreamer-pbutils-1.0` -lpthread

TEST_SRCS = test_file_handling.cpp test_playlist.cpp test_volume.cpp
TEST_BINS = $(TEST_SRCS:.cpp=)

all: $(TEST_BINS)

# Build each test binary
%: %.cpp
	$(CXX) $(CXXFLAGS) $< -o ../build/$@ $(LDFLAGS)

clean:
	rm -f ../build/test_file_handling ../build/test_playlist ../build/test_volume
