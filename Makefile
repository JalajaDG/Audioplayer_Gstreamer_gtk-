# Compiler
CXX = g++

# Common flags
COMMON_FLAGS = -Wall -Wextra -Wpedantic -std=c++17 \
    `pkg-config --cflags gtk+-3.0 gstreamer-1.0 gstreamer-pbutils-1.0`

# Debug (default): symbols, no optimisation
CXXFLAGS = $(COMMON_FLAGS) -g -O0

# Release: optimised, no debug symbols
RELEASE_FLAGS = $(COMMON_FLAGS) -O2 -DNDEBUG

LDFLAGS = `pkg-config --libs gtk+-3.0 gstreamer-1.0 gstreamer-pbutils-1.0` -lpthread

# Enable sanitizers: make SANITIZE=1
ifeq ($(SANITIZE),1)
    CXXFLAGS += -fsanitize=address,undefined -fno-omit-frame-pointer
    LDFLAGS  += -fsanitize=address,undefined
endif

TARGET  = ap
BINDIR  = bin
BUILDDIR = build
PREFIX  ?= /usr/local

SRCS = src/player_state.cpp \
    src/audioPlayer.cpp \
    src/openFolder.cpp \
    src/printPlaylist.cpp \
    src/PlayAudio.cpp \
    src/pause.cpp \
    src/seek.cpp \
    src/seekForward.cpp \
    src/seekBackward.cpp \
    src/playNext.cpp \
    src/playPrev.cpp \
    src/repeat.cpp \
    src/shuffle.cpp \
    src/metadata.cpp \
    src/theme.cpp \
    src/sleepTimer.cpp \
    src/favourite.cpp \
    src/mute.cpp \
    src/volume.cpp

OBJS = $(SRCS:src/%.cpp=$(BUILDDIR)/%.o)

# ── Targets ───────────────────────────────────────────────────────────────────

.PHONY: all release debug clean install uninstall test

all: debug

debug: $(BINDIR)/$(TARGET)

release: CXXFLAGS = $(RELEASE_FLAGS)
release: $(BINDIR)/$(TARGET)

$(BINDIR)/$(TARGET): $(OBJS) | $(BINDIR)
	$(CXX) $(OBJS) -o $@ $(LDFLAGS)

$(BUILDDIR)/%.o: src/%.cpp | $(BUILDDIR)
	$(CXX) $(CXXFLAGS) -Iinclude -c $< -o $@

$(BINDIR) $(BUILDDIR):
	mkdir -p $@

# ── Tests ─────────────────────────────────────────────────────────────────────

test:
	@cd tests/src && $(MAKE) && cd ../.. && ./run_tests.sh

# ── Install / uninstall ───────────────────────────────────────────────────────

install: release
	install -Dm755 $(BINDIR)/$(TARGET) $(DESTDIR)$(PREFIX)/bin/$(TARGET)
	install -d $(DESTDIR)$(PREFIX)/share/audioplayer/icons
	install -m644 icons/* $(DESTDIR)$(PREFIX)/share/audioplayer/icons/
	install -d $(DESTDIR)$(PREFIX)/share/audioplayer/data
	install -m644 data/favourites.txt $(DESTDIR)$(PREFIX)/share/audioplayer/data/

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/$(TARGET)
	rm -rf $(DESTDIR)$(PREFIX)/share/audioplayer

# ── Clean ─────────────────────────────────────────────────────────────────────

clean:
	rm -f $(BUILDDIR)/*.o $(BINDIR)/$(TARGET)
