#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "../src/joueur.h"
#include "../src/score.h"
#include "../src/joker.h"
#include "../src/robot.h"


#define SCREEN_WIDTH 1000
#define SCREEN_HEIGHT 700
#define CODE_LENGTH 5
#define MAX_ATTEMPTS 20
#define DIGIT_SIZE 60
#define DIGIT_SPACING 20


SDL_Window* g_window = NULL;
SDL_Renderer* g_renderer = NULL;
TTF_Font* g_font_large = NULL;
TTF_Font* g_font_medium = NULL;
TTF_Font* g_font_small = NULL;

int g_running = 1;
int g_game_mode = 0;
int g_game_state = 0;

Joueur g_joueur1, g_joueur2;
int g_current_player = 2;
int g_current_attempt = 0;
int g_current_digit = 0;
int g_digits[CODE_LENGTH] = {0};
int g_attempts[MAX_ATTEMPTS][CODE_LENGTH];
int g_results[MAX_ATTEMPTS][2];
int g_game_won = 0;
int g_joker_available = 0;

int init_sdl(void) {
    printf("Initialisation SDL2...\n");
    
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Erreur SDL_Init: %s\n", SDL_GetError());
        return 0;
    }
    
    g_window = SDL_CreateWindow("MASTERMIND",
                               SDL_WINDOWPOS_CENTERED,
                               SDL_WINDOWPOS_CENTERED,
                               SCREEN_WIDTH, SCREEN_HEIGHT,
                               SDL_WINDOW_SHOWN);
    
    if (!g_window) {
        printf("Erreur fenêtre: %s\n", SDL_GetError());
        SDL_Quit();
        return 0;
    }
    
    g_renderer = SDL_CreateRenderer(g_window, -1,
                                   SDL_RENDERER_ACCELERATED);
    
    if (!g_renderer) {
        printf("Erreur renderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(g_window);
        SDL_Quit();
        return 0;
    }
    
    if (TTF_Init() < 0) {
        printf("TTF_Init échoué\n");
    }
    
    g_font_large = TTF_OpenFont("/Library/Fonts/Arial Unicode.ttf", 36);
    g_font_medium = TTF_OpenFont("/Library/Fonts/Arial Unicode.ttf", 24);
    g_font_small = TTF_OpenFont("/Library/Fonts/Arial Unicode.ttf", 18);
    
    if (!g_font_large) {
        g_font_large = TTF_OpenFont(NULL, 36);
        g_font_medium = TTF_OpenFont(NULL, 24);
        g_font_small = TTF_OpenFont(NULL, 18);
    }
    
    srand((unsigned int)time(NULL));
    return 1;
}

void cleanup_sdl(void) {
    if (g_font_small) TTF_CloseFont(g_font_small);
    if (g_font_medium) TTF_CloseFont(g_font_medium);
    if (g_font_large) TTF_CloseFont(g_font_large);
    if (g_renderer) SDL_DestroyRenderer(g_renderer);
    if (g_window) SDL_DestroyWindow(g_window);
    TTF_Quit();
    SDL_Quit();
}

void render_text(TTF_Font* font, const char* text, int x, int y, SDL_Color color) {
    if (!font || !g_renderer) return;
    
    SDL_Surface* surface = TTF_RenderText_Solid(font, text, color);
    if (!surface) return;
    
    SDL_Texture* texture = SDL_CreateTextureFromSurface(g_renderer, surface);
    SDL_Rect rect = {x, y, surface->w, surface->h};
    SDL_RenderCopy(g_renderer, texture, NULL, &rect);
    
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

void draw_circle(int x, int y, int radius, SDL_Color color) {
    SDL_SetRenderDrawColor(g_renderer, color.r, color.g, color.b, color.a);
    
    for (int w = 0; w < radius * 2; w++) {
        for (int h = 0; h < radius * 2; h++) {
            int dx = radius - w;
            int dy = radius - h;
            if (dx*dx + dy*dy <= radius * radius) {
                SDL_RenderDrawPoint(g_renderer, x + w - radius, y + h - radius);
            }
        }
    }
}

void draw_digit(int digit, int x, int y, int selected) {
    SDL_Color color;
    if (selected) {
        color = (SDL_Color){255, 200, 0, 255};
    } else {
        color = (SDL_Color){100, 100, 100, 255};
    }
    
    draw_circle(x + DIGIT_SIZE/2, y + DIGIT_SIZE/2, DIGIT_SIZE/2 - 5, color);

    char digit_str[2];
    sprintf(digit_str, "%d", digit);
    SDL_Color white = {255, 255, 255, 255};
    render_text(g_font_medium, digit_str,
               x + DIGIT_SIZE/2 - 10,
               y + DIGIT_SIZE/2 - 15,
               white);
}

void draw_button(int x, int y, int w, int h, const char* label, int hover) {
    if (hover) {
        SDL_SetRenderDrawColor(g_renderer, 70, 130, 180, 255);
    } else {
        SDL_SetRenderDrawColor(g_renderer, 30, 60, 90, 255);
    }
    
    SDL_Rect rect = {x, y, w, h};
    SDL_RenderFillRect(g_renderer, &rect);

    SDL_SetRenderDrawColor(g_renderer, 255, 255, 255, 255);
    SDL_RenderDrawRect(g_renderer, &rect);
  
    if (g_font_medium) {
        SDL_Color white = {255, 255, 255, 255};
        SDL_Surface* surface = TTF_RenderText_Solid(g_font_medium, label, white);
        if (surface) {
            SDL_Texture* texture = SDL_CreateTextureFromSurface(g_renderer, surface);
            SDL_Rect text_rect = {
                x + (w - surface->w) / 2,
                y + (h - surface->h) / 2,
                surface->w,
                surface->h
            };
            SDL_RenderCopy(g_renderer, texture, NULL, &text_rect);
            SDL_FreeSurface(surface);
            SDL_DestroyTexture(texture);
        }
    }
}


void start_game_2players(void) {
    printf("Démarrage partie 2 joueurs\n");
    g_game_mode = 1;
    g_game_state = 0;
    g_current_attempt = 0;
    g_current_digit = 0;
    g_game_won = 0;
    g_joker_available = 0;
    creer_joueur(&g_joueur1, "Joueur 1");
    creer_joueur(&g_joueur2, "Joueur 2");
    for (int i = 0; i < CODE_LENGTH; i++) {
        g_digits[i] = 0;
    }
}

void start_game_vs_robot(void) {
    printf("Démarrage partie vs Robot\n");
    g_game_mode = 2;
    g_game_state = 0;
    g_current_attempt = 0;
    g_current_digit = 0;
    g_game_won = 0;
    g_joker_available = 0;
    creer_joueur(&g_joueur2, "Joueur");
    creer_joueur(&g_joueur1, "ROBOT");
    for (int i = 0; i < CODE_LENGTH; i++) {
        g_joueur1.code_secret[i] = rand() % 10;
    }
}

void check_code(void) {
    int bien_places = 0, mal_places = 0;
    int secret_copy[CODE_LENGTH];
    int attempt_copy[CODE_LENGTH];

    for (int i = 0; i < CODE_LENGTH; i++) {
        secret_copy[i] = g_joueur1.code_secret[i];
        attempt_copy[i] = g_digits[i];
    }

    for (int i = 0; i < CODE_LENGTH; i++) {
        if (attempt_copy[i] == secret_copy[i]) {
            bien_places++;
            attempt_copy[i] = secret_copy[i] = -1;
        }
    }
    for (int i = 0; i < CODE_LENGTH; i++) {
        if (attempt_copy[i] != -1) {
            for (int j = 0; j < CODE_LENGTH; j++) {
                if (secret_copy[j] == attempt_copy[i]) {
                    mal_places++;
                    secret_copy[j] = -1;
                    break;
                }
            }
        }
    }

    g_results[g_current_attempt][0] = bien_places;
    g_results[g_current_attempt][1] = mal_places;

    for (int i = 0; i < CODE_LENGTH; i++) {
        g_attempts[g_current_attempt][i] = g_digits[i];
    }
 
    if (bien_places == CODE_LENGTH) {
        g_game_won = 1;
        g_game_state = 2;

        printf("Victoire en %d essais!\n", g_current_attempt + 1);
        g_joueur2.score = g_current_attempt + 1;
        
        char response;
        printf("Voulez-vous sauvegarder votre score? (o/n): ");
        scanf(" %c", &response);
        
        if (response == 'o' || response == 'O') {
            enregistrer_score(g_joueur2, 1, (g_game_mode == 2) ? 1 : 0);
        }
    } else {
        g_current_attempt++;
        g_current_digit = 0;
    
        for (int i = 0; i < CODE_LENGTH; i++) {
            g_digits[i] = 0;
        }
    
        if (g_current_attempt >= 10 && !g_joker_available) {
            g_joker_available = 1;
        }
  
        if (g_current_attempt >= MAX_ATTEMPTS) {
            g_game_state = 2;
        }
    }
}

void use_joker(void) {
    if (!g_joker_available) return;
    
    printf("Joker utilise!\n");
    g_joker_available = 0;

    g_current_attempt += 3;

    int position = rand() % CODE_LENGTH;
    printf("Indice: Le chiffre à la position %d est %d\n",
           position + 1, g_joueur1.code_secret[position]);
}

void draw_main_menu(void) {
    SDL_SetRenderDrawColor(g_renderer, 20, 30, 40, 255);
    SDL_RenderClear(g_renderer);

    SDL_Color gold = {255, 215, 0, 255};
    render_text(g_font_large, "   MASTERMIND  ",
                SCREEN_WIDTH / 2 - 150, 80, gold);

    int center_x = SCREEN_WIDTH/2 - 125;
    int start_y = 200;
    
    draw_button(center_x, start_y, 250, 60, "2 JOUEURS", 0);
    draw_button(center_x, start_y + 80, 250, 60, "VS ROBOT", 0);
    draw_button(center_x, start_y + 160, 250, 60, "MEILLEURS SCORES", 0);
    draw_button(center_x, start_y + 240, 250, 60, "QUITTER", 0);
    SDL_Color gray = {180, 180, 180, 255};
    render_text(g_font_small, "         Cliquez sur un bouton ESC pour quitter",
               SCREEN_WIDTH/2 - 200, SCREEN_HEIGHT - 50, gray);
}

void draw_code_entry(void) {
    
    SDL_SetRenderDrawColor(g_renderer, 15, 25, 35, 255);
    SDL_RenderClear(g_renderer);
    
 
    SDL_Color white = {255, 255, 255, 255};
    const char* title = (g_game_mode == 1) ?
                       "JOUEUR 1: Entrez le code secret" :
                       "ROBOT: Code secret genere";
    render_text(g_font_large, title, 100, 50, white);
    
    render_text(g_font_medium, "Entrez 5 chiffres (0-9) avec le clavier",
               100, 120, white);
    render_text(g_font_medium, "BACKSPACE pour effacer ENTER pour valider",
               100, 150, white);
    
    int start_x = 200;
    int y = 250;
    
    for (int i = 0; i < CODE_LENGTH; i++) {
        draw_digit(g_digits[i], start_x + i * (DIGIT_SIZE + DIGIT_SPACING), y,
                  (i == g_current_digit));
    }
    
    draw_button(400, 400, 200, 50, "VALIDER", 0);
    
    if (g_game_mode == 2) {
        render_text(g_font_medium, "Le robot a genere son code secret",
                   100, 500, white);
        draw_button(400, 550, 200, 50, "COMMENCER", 0);
    }
}

void draw_game_screen(void) {
    SDL_SetRenderDrawColor(g_renderer, 15, 25, 35, 255);
    SDL_RenderClear(g_renderer);
    
    SDL_Color white = {255, 255, 255, 255};
    const char* mode = (g_game_mode == 1) ? "2 JOUEURS" : "VS ROBOT";
    char title[100];
    sprintf(title, "MASTERMIND - %s", mode);
    render_text(g_font_large, title, 50, 30, white);
    
    char attempt_info[50];
    sprintf(attempt_info, "Essai %d/%d", g_current_attempt + 1, MAX_ATTEMPTS);
    render_text(g_font_medium, attempt_info, 50, 80, white);
    
    render_text(g_font_medium, "Votre tentative:", 50, 130, white);
    
    int start_x = 50;
    int y = 180;
    
    for (int i = 0; i < CODE_LENGTH; i++) {
        draw_digit(g_digits[i], start_x + i * (DIGIT_SIZE + DIGIT_SPACING), y,
                  (i == g_current_digit));
    }
    
    render_text(g_font_medium, "Historique:", 500, 130, white);
    
    for (int attempt = 0; attempt < g_current_attempt; attempt++) {
        y = 180 + attempt * (DIGIT_SIZE + 10);
        
        for (int i = 0; i < CODE_LENGTH; i++) {
            draw_digit(g_attempts[attempt][i],
                      500 + i * (DIGIT_SIZE/2 + 5),
                      y, 0);
        }
        
        char result[50];
        sprintf(result, "%d Bien Place %d Mal Place",
                g_results[attempt][0],
                g_results[attempt][1]);
        render_text(g_font_small, result,
                   500 + CODE_LENGTH * (DIGIT_SIZE/2 + 5) + 20,
                   y + 10, white);
    }
    
    draw_button(50, 300, 200, 50, "VALIDER", 0);
    
    if (g_joker_available) {
        draw_button(300, 300, 200, 50, "UTILISER JOKER", 0);
    }
    
    render_text(g_font_small, "0-9: Chiffres BACKSPACE: Effacer ENTER: Valider",
               50, 380, white);
    render_text(g_font_small, "ESC: Retour au menu", 50, 410, white);
}

void draw_game_over(void) {
    SDL_SetRenderDrawColor(g_renderer, 15, 25, 35, 255);
    SDL_RenderClear(g_renderer);
    
    SDL_Color white = {255, 255, 255, 255};
    
    if (g_game_won) {
        SDL_Color gold = {255, 215, 0, 255};
        render_text(g_font_large, "VICTOIRE !",
                   SCREEN_WIDTH/2 - 100, 100, gold);
        
        char message[100];
        sprintf(message, "Vous avez trouve en %d essais!", g_current_attempt + 1);
        render_text(g_font_medium, message,
                   SCREEN_WIDTH/2 - 150, 180, white);
    } else {
        SDL_Color red = {255, 50, 50, 255};
        render_text(g_font_large, "DÉFAITE",
                   SCREEN_WIDTH/2 - 80, 100, red);
        render_text(g_font_medium, "Vous avez epuise tous vos essais",
                   SCREEN_WIDTH/2 - 150, 180, white);
    }
    
    render_text(g_font_medium, "Code secret:",
               SCREEN_WIDTH/2 - 200, 250, white);
    
    int start_x = SCREEN_WIDTH/2 - 150;
    int y = 300;
    
    for (int i = 0; i < CODE_LENGTH; i++) {
        draw_digit(g_joueur1.code_secret[i],
                  start_x + i * (DIGIT_SIZE + DIGIT_SPACING),
                  y, 0);
    }
    
    draw_button(SCREEN_WIDTH/2 - 150, 400, 300, 60, "REJOUER", 0);
    draw_button(SCREEN_WIDTH/2 - 150, 480, 300, 60, "MENU PRINCIPAL", 0);
}


void main_loop(void) {
    SDL_Event event;
    int mouse_x, mouse_y;
    
    while (g_running) {
        SDL_GetMouseState(&mouse_x, &mouse_y);
        
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    g_running = 0;
                    break;
                    
                case SDL_KEYDOWN:
                    if (g_game_state == 0 || g_game_state == 1) {
                        
                        if (event.key.keysym.sym >= SDLK_0 &&
                            event.key.keysym.sym <= SDLK_9) {
                            
                            int digit = event.key.keysym.sym - SDLK_0;
                            if (g_current_digit < CODE_LENGTH) {
                                g_digits[g_current_digit] = digit;
                                g_current_digit++;
                                
                                if (g_current_digit >= CODE_LENGTH) {
                                    g_current_digit = CODE_LENGTH - 1;
                                }
                            }
                        }
                        
                        else if (event.key.keysym.sym == SDLK_BACKSPACE) {
                            if (g_current_digit > 0) {
                                g_current_digit--;
                                g_digits[g_current_digit] = 0;
                            }
                        }
                        
                        else if (event.key.keysym.sym == SDLK_RETURN) {
                            if (g_game_state == 0) {
                                for (int i = 0; i < CODE_LENGTH; i++) {
                                    g_joueur1.code_secret[i] = g_digits[i];
                                }
                                g_game_state = 1;
                                g_current_digit = 0;
                                
                                for (int i = 0; i < CODE_LENGTH; i++) {
                                    g_digits[i] = 0;
                                }
                            }
                            else if (g_game_state == 1) {
                                int complete = 1;
                                for (int i = 0; i < CODE_LENGTH; i++) {
                                    if (g_digits[i] == 0) complete = 0;
                                }
                                
                                if (complete) {
                                    check_code();
                                }
                            }
                        }
                        
                        else if (event.key.keysym.sym == SDLK_ESCAPE) {
                            if (g_game_state == 0 || g_game_state == 1) {
                                g_game_state = 0;
                                g_game_mode = 0;
                            }
                        }
                    }
                    break;
                    
                case SDL_MOUSEBUTTONDOWN:
                    if (event.button.button == SDL_BUTTON_LEFT) {
                        int x = event.button.x;
                        int y = event.button.y;
                        
                        if (g_game_mode == 0) {
                            if (x > 375 && x < 625) {
                                if (y > 200 && y < 260) {
                                    start_game_2players();
                                }
                                else if (y > 280 && y < 340) {
                                    start_game_vs_robot();
                                }
                                else if (y > 360 && y < 420) {
                                    printf("\n=== SCORES ===\n");
                                    afficher_meilleurs_scores();
                                    printf("=============\n\n");
                                }
                                else if (y > 440 && y < 500) {
                                    g_running = 0;
                                }
                            }
                        }
                        else if (g_game_state == 0) {
                            if (x > 400 && x < 600 && y > 400 && y < 450) {
                                
                                for (int i = 0; i < CODE_LENGTH; i++) {
                                    g_joueur1.code_secret[i] = g_digits[i];
                                }
                                g_game_state = 1;
                                g_current_digit = 0;
                                
                                for (int i = 0; i < CODE_LENGTH; i++) {
                                    g_digits[i] = 0;
                                }
                            }
                            
                            if (g_game_mode == 2 &&
                                x > 400 && x < 600 && y > 550 && y < 600) {
                                g_game_state = 1;
                            }
                        }
                        else if (g_game_state == 1) {
                            if (x > 50 && x < 250 && y > 300 && y < 350) {
                                check_code();
                            }
                            else if (g_joker_available &&
                                     x > 300 && x < 500 && y > 300 && y < 350) {
                                use_joker();
                            }
                        }
                        else if (g_game_state == 2) {
                            if (x > 350 && x < 650) {
                                if (y > 400 && y < 460) {
                                    if (g_game_mode == 1) start_game_2players();
                                    else start_game_vs_robot();
                                }
                                else if (y > 480 && y < 540) {
                                    g_game_mode = 0;
                                    g_game_state = 0;
                                }
                            }
                        }
                    }
                    break;
            }
        }
        
        if (g_game_mode == 0) {
            draw_main_menu();
        }
        else if (g_game_state == 0) {
            draw_code_entry();
        }
        else if (g_game_state == 1) {
            draw_game_screen();
        }
        else if (g_game_state == 2) {
            draw_game_over();
        }
        
        SDL_RenderPresent(g_renderer);
        SDL_Delay(16);
    }
}

int main(int argc, char* argv[]) {
    printf("\n================================\n");
    printf("            MASTERMIND      \n");
    printf("================================\n\n");
    
    if (!init_sdl()) {
        printf("Échec initialisation\n");
        return 1;
    }
    
    main_loop();
    cleanup_sdl();
    
    printf("\nProgramme terminé\n");
    return 0;
}

