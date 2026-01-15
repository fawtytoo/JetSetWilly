# jetset willy

TARGET = jetsetwilly

DESKTOP = jetsetwilly.desktop
ICON = jetsetwilly.png

LOCAL = ~/.local

CC = gcc

CFLAGS = -O -MMD `sdl2-config --cflags`
LDFLAGS = `sdl2-config --libs`

SRC = src
O = linux

OBJS = $(O)/main.o $(O)/video.o $(O)/loader.o $(O)/title.o $(O)/audio.o $(O)/miner.o $(O)/levels.o $(O)/game.o $(O)/die.o $(O)/gameover.o $(O)/robots.o $(O)/rope.o $(O)/cheat.o $(O)/misc.o $(O)/codes.o

BUILD = -DBUILD=\"1.0.0\ \ $(shell date +'%y.%m.%d')\"

all:	dir $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)

$(O)/loader.o:
	$(CC) $(CFLAGS) $(BUILD) -c $(SRC)/loader.c -o $@

$(O)/%.o:	$(SRC)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(O) $(TARGET)

install: all
	cp $(TARGET) $(LOCAL)/bin/
	cp debian/$(DESKTOP) $(LOCAL)/share/applications/
	cp debian/$(ICON) $(LOCAL)/share/icons/

uninstall:
	rm $(LOCAL)/bin/$(TARGET)
	rm $(LOCAL)/share/applications/$(DESKTOP)
	rm $(LOCAL)/share/icons/$(ICON)

dir:
	@mkdir -p $(O)

-include $(O)/*.d

# jetset willy
