CC = gcc
CFLAGS = -Wall -Wextra -std=c99
FRAMEWORKS = -framework Cocoa -framework IOKit -framework CoreVideo

# Chemins Homebrew sur macOS
INCLUDES = -I/opt/homebrew/include/SDL2
LIBS = -L/opt/homebrew/lib -lSDL2 -lSDL2_ttf $(FRAMEWORKS)

SRC = mastermind_sdl.c ../src/joueur.c ../src/score.c ../src/joker.c ../src/robot.c
TARGET = mastermind_sdl

all: $(TARGET)

$(TARGET): $(SRC)
    $(CC) $(CFLAGS) $(INCLUDES) -o $(TARGET) $(SRC) $(LIBS)

clean:
    rm -f $(TARGET)

run: $(TARGET)
    ./$(TARGET)

# Test SDL2 sur macOS
test-macos:
    @echo "=== Test macOS SDL2 ==="
    @echo "Homebrew SDL2:"
    @ls -la /opt/homebrew/include/SDL2/SDL.h 2>/dev/null || echo "SDL2 not found"
    @echo "Compilation test:"
    @echo '#include <SDL2/SDL.h>\nint main(){return 0;}' > test.c
    @$(CC) test.c -lSDL2 -framework Cocoa 2>/dev/null && echo "SDL2 OK" || echo "SDL2 error"
    @rm -f test.c a.out

.PHONY: all clean run test-macos
