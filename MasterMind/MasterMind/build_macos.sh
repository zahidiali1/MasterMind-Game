#!/bin/bash
# build_macos.sh - Script de compilation pour macOS

echo "🔨 Construction de Mastermind pour macOS..."

# Vérifier SDL2
if [ ! -f "/opt/homebrew/lib/libSDL2.dylib" ]; then
    echo "❌ SDL2 non trouvé. Installation..."
    brew install sdl2
fi

# Créer les dossiers
mkdir -p bin

# 1. Compiler version SDL (sans texte)
echo "🎮 Compilation version SDL sans texte..."
clang -Wall -Wextra -std=c99 \
    -I/opt/homebrew/include/SDL2 \
    -L/opt/homebrew/lib \
    mastermind_sdl_no_ttf.c \
    ../src/joueur.c \
    ../src/score.c \
    ../src/joker.c \
    ../src/robot.c \
    -lSDL2 \
    -framework Cocoa -framework IOKit -framework CoreVideo \
    -o bin/mastermind_sdl

# 2. Compiler version console
echo "📟 Compilation version console..."
clang -Wall -Wextra -std=c99 \
    main.c \
    ../src/joueur.c \
    ../src/score.c \
    ../src/joker.c \
    ../src/robot.c \
    -o bin/mastermind_console

# Vérifier
if [ -f "bin/mastermind_sdl" ]; then
    echo "✅ Version SDL compilée: bin/mastermind_sdl"
    chmod +x bin/mastermind_sdl
fi

if [ -f "bin/mastermind_console" ]; then
    echo "✅ Version console compilée: bin/mastermind_console"
    chmod +x bin/mastermind_console
fi

echo ""
echo "🚀 COMMANDES D'EXÉCUTION:"
echo "   🎮 ./bin/mastermind_sdl      (interface graphique)"
echo "   📟 ./bin/mastermind_console  (version texte)"
echo ""
echo "🎯 CONTRÔLES:"
echo "   - Souris: Cliquez sur les boutons"
echo "   - Clavier: 1=2Joueurs, 2=Robot, 3=Scores, 4=Quitter, ESC=Quitter"
