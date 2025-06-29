# ──────────────  Makefile  ──────────────
TARGET   := build/server          # final executable name
SRC      := src/main.c src/server.c
HEADERS  := src/macros.h        # dependency tracking

CC       := gcc
CFLAGS   := -Wall -Wextra -pedantic -std=c17 -O2
LIBS     := -lpthread       

all: $(TARGET)

# Compile rule
$(TARGET): $(SRC) $(HEADERS)
	$(CC) $(CFLAGS) $(SRC) $(LIBS) -o $(TARGET)

run: $(TARGET)
	./$(TARGET)

# Remove build artefacts
clean:
	rm -f $(TARGET)

.PHONY: all clean run
# ─────────────────────────────────────────
