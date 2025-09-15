# Compiler and flags
CXX = g++
CXXFLAGS = -Wall -g `pkg-config --cflags gtk+-3.0 gstreamer-1.0 gstreamer-pbutils-1.0`
LDFLAGS = `pkg-config --libs gtk+-3.0 gstreamer-1.0 gstreamer-pbutils-1.0` -lpthread

# Enable sanitizers if SANITIZE=1
ifeq ($(SANITIZE),1)
    CXXFLAGS += -fsanitize=address,undefined -fno-omit-frame-pointer
    LDFLAGS  += -fsanitize=address,undefined
endif


# Executable name
TARGET = ap

# Source files
SRCS = audioPlayer.cpp \
       openFolder.cpp \
       printPlaylist.cpp \
       PlayAudio.cpp \
       pause.cpp \
       seek.cpp \
       seekForward.cpp \
       seekBackward.cpp \
       playNext.cpp \
       playPrev.cpp \
       repeat.cpp \
       shuffle.cpp \
       metadata.cpp \
       theme.cpp \
       sleepTimer.cpp \
       favourite.cpp \
       mute.cpp \
       volume.cpp
      

# Object files
OBJS = $(SRCS:.cpp=.o)

# Default rule
all: $(TARGET)

# Linking
$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $(TARGET) $(LDFLAGS)

# Compilation rule
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean rule
clean:
	rm -f $(OBJS) $(TARGET)
