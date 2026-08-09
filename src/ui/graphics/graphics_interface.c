/**
 * @file graphics_interface.c
 * @brief KALAH SDL2 Graphics Implementation
 */

#include "graphics_interface.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* ============== INITIALIZATION ============== */

GraphicsContext* graphics_create(int initial_stones) {
    GraphicsContext *ctx = (GraphicsContext*)malloc(sizeof(GraphicsContext));
    if (!ctx) return NULL;
    
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "SDL initialization failed: %s\n", SDL_GetError());
        free(ctx);
        return NULL;
    }
    
    // Create window
    ctx->window = SDL_CreateWindow(
        WINDOW_TITLE,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        SDL_WINDOW_SHOWN
    );
    
    if (!ctx->window) {
        fprintf(stderr, "Window creation failed: %s\n", SDL_GetError());
        SDL_Quit();
        free(ctx);
        return NULL;
    }
    
    // Create renderer
    ctx->renderer = SDL_CreateRenderer(
        ctx->window,
        -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );
    
    if (!ctx->renderer) {
        fprintf(stderr, "Renderer creation failed: %s\n", SDL_GetError());
        SDL_DestroyWindow(ctx->window);
        SDL_Quit();
        free(ctx);
        return NULL;
    }
    
    // Create game engine
    ctx->engine = engine_create(initial_stones);
    if (!ctx->engine) {
        SDL_DestroyRenderer(ctx->renderer);
        SDL_DestroyWindow(ctx->window);
        SDL_Quit();
        free(ctx);
        return NULL;
    }
    
    ctx->running = 1;
    ctx->game_state = 0;  // Start at menu
    ctx->selected_pile = -1;
    ctx->menu_selected = 0;
    ctx->font_texture = NULL;
    
    return ctx;
}

void graphics_destroy(GraphicsContext *ctx) {
    if (!ctx) return;
    
    if (ctx->engine) {
        engine_destroy(ctx->engine);
    }
    
    if (ctx->renderer) {
        SDL_DestroyRenderer(ctx->renderer);
    }
    
    if (ctx->window) {
        SDL_DestroyWindow(ctx->window);
    }
    
    SDL_Quit();
    free(ctx);
}

/* ============== RENDERING PRIMITIVES ============== */

void graphics_clear(GraphicsContext *ctx, Color color) {
    if (!ctx || !ctx->renderer) return;
    
    SDL_SetRenderDrawColor(ctx->renderer, color.r, color.g, color.b, color.a);
    SDL_RenderClear(ctx->renderer);
}

void graphics_draw_rect(GraphicsContext *ctx, Rect r, Color color, int filled) {
    if (!ctx || !ctx->renderer) return;
    
    SDL_SetRenderDrawColor(ctx->renderer, color.r, color.g, color.b, color.a);
    
    SDL_Rect rect = {r.x, r.y, r.w, r.h};
    
    if (filled) {
        SDL_RenderFillRect(ctx->renderer, &rect);
    } else {
        SDL_RenderDrawRect(ctx->renderer, &rect);
    }
}

void graphics_draw_circle(GraphicsContext *ctx, int cx, int cy, int radius, Color color, int filled) {
    if (!ctx || !ctx->renderer) return;
    
    SDL_SetRenderDrawColor(ctx->renderer, color.r, color.g, color.b, color.a);
    
    int x = radius - 1;
    int y = 0;
    int tx = 1;
    int ty = 1;
    int err = (tx - (radius << 1));
    
    while (x >= y) {
        // Draw circle points using Bresenham's algorithm
        if (filled) {
            SDL_RenderDrawLine(ctx->renderer, cx - x, cy + y, cx + x, cy + y);
            SDL_RenderDrawLine(ctx->renderer, cx - x, cy - y, cx + x, cy - y);
            SDL_RenderDrawLine(ctx->renderer, cx - y, cy + x, cx + y, cy + x);
            SDL_RenderDrawLine(ctx->renderer, cx - y, cy - x, cx + y, cy - x);
        } else {
            SDL_RenderDrawPoint(ctx->renderer, cx + x, cy + y);
            SDL_RenderDrawPoint(ctx->renderer, cx - x, cy + y);
            SDL_RenderDrawPoint(ctx->renderer, cx + x, cy - y);
            SDL_RenderDrawPoint(ctx->renderer, cx - x, cy - y);
            SDL_RenderDrawPoint(ctx->renderer, cx + y, cy + x);
            SDL_RenderDrawPoint(ctx->renderer, cx - y, cy + x);
            SDL_RenderDrawPoint(ctx->renderer, cx + y, cy - x);
            SDL_RenderDrawPoint(ctx->renderer, cx - y, cy - x);
        }
        
        if (err <= 0) {
            y += 1;
            err += ty;
            ty += 2;
        }
        
        if (err > 0) {
            x -= 1;
            err += tx - (radius << 1);
            tx += 2;
        }
    }
}

/* Rectangle with a simple light/dark bevel to fake a 3D wooden panel */
void graphics_draw_panel(GraphicsContext *ctx, Rect r, Color base, int bevel) {
    if (!ctx) return;
    
    graphics_draw_rect(ctx, r, base, 1);
    
    Color light = {(Uint8)SDL_min(255, base.r + 35), (Uint8)SDL_min(255, base.g + 35), (Uint8)SDL_min(255, base.b + 35), 255};
    Color dark = {(Uint8)(base.r > 35 ? base.r - 35 : 0), (Uint8)(base.g > 35 ? base.g - 35 : 0), (Uint8)(base.b > 35 ? base.b - 35 : 0), 255};
    
    Rect top = {r.x, r.y, r.w, bevel};
    Rect left = {r.x, r.y, bevel, r.h};
    Rect bottom = {r.x, r.y + r.h - bevel, r.w, bevel};
    Rect right = {r.x + r.w - bevel, r.y, bevel, r.h};
    
    graphics_draw_rect(ctx, top, light, 1);
    graphics_draw_rect(ctx, left, light, 1);
    graphics_draw_rect(ctx, bottom, dark, 1);
    graphics_draw_rect(ctx, right, dark, 1);
}

/* 5x7 monospace bitmap font, each row's low 5 bits = pixels (bit4=leftmost) */
static const unsigned char* glyph_for(char c) {
    static const unsigned char A[7] = {0x0E,0x11,0x11,0x1F,0x11,0x11,0x11};
    static const unsigned char B[7] = {0x1E,0x11,0x11,0x1E,0x11,0x11,0x1E};
    static const unsigned char C[7] = {0x0F,0x10,0x10,0x10,0x10,0x10,0x0F};
    static const unsigned char D[7] = {0x1E,0x11,0x11,0x11,0x11,0x11,0x1E};
    static const unsigned char E[7] = {0x1F,0x10,0x10,0x1E,0x10,0x10,0x1F};
    static const unsigned char F[7] = {0x1F,0x10,0x10,0x1E,0x10,0x10,0x10};
    static const unsigned char G[7] = {0x0F,0x10,0x10,0x17,0x11,0x11,0x0F};
    static const unsigned char H[7] = {0x11,0x11,0x11,0x1F,0x11,0x11,0x11};
    static const unsigned char I[7] = {0x0E,0x04,0x04,0x04,0x04,0x04,0x0E};
    static const unsigned char J[7] = {0x07,0x02,0x02,0x02,0x02,0x12,0x0C};
    static const unsigned char K[7] = {0x11,0x12,0x14,0x18,0x14,0x12,0x11};
    static const unsigned char L[7] = {0x10,0x10,0x10,0x10,0x10,0x10,0x1F};
    static const unsigned char M[7] = {0x11,0x1B,0x15,0x15,0x11,0x11,0x11};
    static const unsigned char N[7] = {0x11,0x19,0x15,0x15,0x13,0x11,0x11};
    static const unsigned char O[7] = {0x0E,0x11,0x11,0x11,0x11,0x11,0x0E};
    static const unsigned char P[7] = {0x1E,0x11,0x11,0x1E,0x10,0x10,0x10};
    static const unsigned char Q[7] = {0x0E,0x11,0x11,0x11,0x15,0x12,0x0D};
    static const unsigned char R[7] = {0x1E,0x11,0x11,0x1E,0x14,0x12,0x11};
    static const unsigned char S[7] = {0x0F,0x10,0x10,0x0E,0x01,0x01,0x1E};
    static const unsigned char T[7] = {0x1F,0x04,0x04,0x04,0x04,0x04,0x04};
    static const unsigned char U[7] = {0x11,0x11,0x11,0x11,0x11,0x11,0x0E};
    static const unsigned char V[7] = {0x11,0x11,0x11,0x11,0x11,0x0A,0x04};
    static const unsigned char W[7] = {0x11,0x11,0x11,0x15,0x15,0x15,0x0A};
    static const unsigned char X[7] = {0x11,0x11,0x0A,0x04,0x0A,0x11,0x11};
    static const unsigned char Y[7] = {0x11,0x11,0x0A,0x04,0x04,0x04,0x04};
    static const unsigned char Z[7] = {0x1F,0x01,0x02,0x04,0x08,0x10,0x1F};
    static const unsigned char D0[7] = {0x0E,0x11,0x13,0x15,0x19,0x11,0x0E};
    static const unsigned char D1[7] = {0x04,0x0C,0x04,0x04,0x04,0x04,0x0E};
    static const unsigned char D2[7] = {0x0E,0x11,0x01,0x02,0x04,0x08,0x1F};
    static const unsigned char D3[7] = {0x1F,0x02,0x04,0x02,0x01,0x11,0x0E};
    static const unsigned char D4[7] = {0x02,0x06,0x0A,0x12,0x1F,0x02,0x02};
    static const unsigned char D5[7] = {0x1F,0x10,0x1E,0x01,0x01,0x11,0x0E};
    static const unsigned char D6[7] = {0x06,0x08,0x10,0x1E,0x11,0x11,0x0E};
    static const unsigned char D7[7] = {0x1F,0x01,0x02,0x04,0x08,0x08,0x08};
    static const unsigned char D8[7] = {0x0E,0x11,0x11,0x0E,0x11,0x11,0x0E};
    static const unsigned char D9[7] = {0x0E,0x11,0x11,0x0F,0x01,0x02,0x0C};
    static const unsigned char DOT[7] = {0,0,0,0,0,0x0C,0x0C};
    static const unsigned char COMMA[7] = {0,0,0,0,0,0x0C,0x08};
    static const unsigned char COLON[7] = {0,0x0C,0x0C,0,0x0C,0x0C,0};
    static const unsigned char DASH[7] = {0,0,0,0x1F,0,0,0};
    static const unsigned char APOS[7] = {0x08,0x08,0,0,0,0,0};
    static const unsigned char BANG[7] = {0x04,0x04,0x04,0x04,0x04,0,0x04};
    static const unsigned char STAR[7] = {0,0x15,0x0E,0x1F,0x0E,0x15,0};
    static const unsigned char SPACE[7] = {0,0,0,0,0,0,0};

    char u = (char)toupper((unsigned char)c);
    switch (u) {
        case 'A': return A; case 'B': return B; case 'C': return C;
        case 'D': return D; case 'E': return E; case 'F': return F;
        case 'G': return G; case 'H': return H; case 'I': return I;
        case 'J': return J; case 'K': return K; case 'L': return L;
        case 'M': return M; case 'N': return N; case 'O': return O;
        case 'P': return P; case 'Q': return Q; case 'R': return R;
        case 'S': return S; case 'T': return T; case 'U': return U;
        case 'V': return V; case 'W': return W; case 'X': return X;
        case 'Y': return Y; case 'Z': return Z;
        case '0': return D0; case '1': return D1; case '2': return D2;
        case '3': return D3; case '4': return D4; case '5': return D5;
        case '6': return D6; case '7': return D7; case '8': return D8;
        case '9': return D9;
        case '.': return DOT; case ',': return COMMA; case ':': return COLON;
        case '-': return DASH; case '\'': return APOS; case '!': return BANG;
        case '*': return STAR;
        default: return SPACE;
    }
}

#define GLYPH_PIXEL 3
#define GLYPH_SPACING 1

void graphics_draw_text(GraphicsContext *ctx, const char *text, int x, int y, Color color) {
    if (!ctx || !text) return;

    int cursor_x = x;
    for (const char *p = text; *p != '\0'; p++) {
        const unsigned char *glyph = glyph_for(*p);
        for (int row = 0; row < 7; row++) {
            for (int col = 0; col < 5; col++) {
                if (glyph[row] & (0x10 >> col)) {
                    Rect px = {cursor_x + col * GLYPH_PIXEL, y + row * GLYPH_PIXEL, GLYPH_PIXEL, GLYPH_PIXEL};
                    graphics_draw_rect(ctx, px, color, 1);
                }
            }
        }
        cursor_x += (5 * GLYPH_PIXEL) + GLYPH_SPACING * GLYPH_PIXEL;
    }
}

/* ============== GAME RENDERING ============== */

/* Draws up to 12 stones as small dots in a compact grid inside the pit */
static void draw_stone_dots(GraphicsContext *ctx, int cx, int cy, int count) {
    int shown = count > 12 ? 12 : count;
    int cols = 4;
    int spacing = 12;
    int start_x = cx - ((cols - 1) * spacing) / 2;
    int start_y = cy - 10;
    
    for (int i = 0; i < shown; i++) {
        int col = i % cols;
        int row = i / cols;
        graphics_draw_circle(ctx, start_x + col * spacing, start_y + row * spacing, 3, COLOR_STONE, 1);
    }
}

void graphics_draw_pile(GraphicsContext *ctx, int player, int pile, int cx, int cy) {
    if (!ctx || !ctx->engine) return;
    
    GameState *state = ctx->engine->state;
    int count = state->players[player].piles[pile].count;
    int radius = 42;
    
    int is_active_player = (ctx->engine->state->current_player == player);
    int is_selected = (ctx->selected_pile == pile && is_active_player);
    
    // Pit shadow / depth ring, then cream-colored pit face
    graphics_draw_circle(ctx, cx, cy, radius + 4, COLOR_PIT_SHADOW, 1);
    graphics_draw_circle(ctx, cx, cy, radius, COLOR_CREAM, 1);
    graphics_draw_circle(ctx, cx, cy, radius, COLOR_WOOD_DARK, 0);
    
    // Highlight ring for a selectable/selected pit belonging to the active player
    if (is_selected) {
        graphics_draw_circle(ctx, cx, cy, radius + 6, COLOR_HIGHLIGHT, 0);
    } else if (is_active_player && count > 0) {
        graphics_draw_circle(ctx, cx, cy, radius + 2, (player == 0) ? COLOR_ACCENT_BLUE : COLOR_ACCENT_RED, 0);
    }
    
    draw_stone_dots(ctx, cx, cy, count);
    
    char count_text[8];
    sprintf(count_text, "%d", count);
    graphics_draw_text(ctx, count_text, cx - 6, cy + 14, COLOR_BLACK);
    
    // Store bounding box for click detection
    Rect pile_rect = {cx - radius, cy - radius, radius * 2, radius * 2};
    ctx->pile_rects[player * 6 + pile] = pile_rect;
}

void graphics_draw_kalah(GraphicsContext *ctx, int player, int cx, int cy, int height) {
    if (!ctx || !ctx->engine) return;
    
    GameState *state = ctx->engine->state;
    int score = state->players[player].kalah.count;
    int width = 90;
    
    Rect kalah_rect = {cx - width / 2, cy - height / 2, width, height};
    Color accent = (player == 0) ? COLOR_ACCENT_BLUE : COLOR_ACCENT_RED;
    
    // Rounded look: cream body capped with an accent-colored circle top and bottom
    graphics_draw_rect(ctx, kalah_rect, COLOR_PIT_SHADOW, 1);
    Rect inner = {kalah_rect.x + 4, kalah_rect.y + width / 2, width - 8, height - width};
    graphics_draw_rect(ctx, inner, COLOR_CREAM, 1);
    graphics_draw_circle(ctx, cx, kalah_rect.y + width / 2, width / 2 - 4, COLOR_CREAM, 1);
    graphics_draw_circle(ctx, cx, kalah_rect.y + height - width / 2, width / 2 - 4, COLOR_CREAM, 1);
    graphics_draw_rect(ctx, kalah_rect, accent, 0);
    
    draw_stone_dots(ctx, cx, cy, score);
    
    char score_text[8];
    sprintf(score_text, "%d", score);
    graphics_draw_text(ctx, score_text, cx - 6, cy + 40, COLOR_BLACK);
    
    ctx->kalah_rects[player] = kalah_rect;
}

void graphics_draw_board(GraphicsContext *ctx) {
    if (!ctx || !ctx->engine) return;
    
    GameState *state = ctx->engine->state;
    
    graphics_clear(ctx, COLOR_WOOD_MED);
    
    // Header bar showing title and whose turn it is
    Rect header = {0, 0, WINDOW_WIDTH, 70};
    graphics_draw_panel(ctx, header, COLOR_WOOD_DARK, 4);
    graphics_draw_text(ctx, "KALAH", 30, 25, COLOR_CREAM);
    
    Color turn_color = (state->current_player == 0) ? COLOR_ACCENT_BLUE : COLOR_ACCENT_RED;
    graphics_draw_circle(ctx, WINDOW_WIDTH / 2 - 90, 35, 8, turn_color, 1);
    char turn_text[50];
    sprintf(turn_text, "Player %d's Turn", state->current_player + 1);
    graphics_draw_text(ctx, turn_text, WINDOW_WIDTH / 2 - 70, 25, COLOR_CREAM);
    
    // Wooden board panel
    Rect board = {50, 110, WINDOW_WIDTH - 100, WINDOW_HEIGHT - 200};
    graphics_draw_panel(ctx, board, COLOR_WOOD_LIGHT, 6);
    
    int kalah_cx_left = 130;
    int kalah_cx_right = WINDOW_WIDTH - 130;
    int kalah_cy = board.y + board.h / 2;
    int kalah_height = board.h - 60;
    
    // Player 2's store (left) and Player 1's store (right), classic Mancala layout
    graphics_draw_kalah(ctx, 1, kalah_cx_left, kalah_cy, kalah_height);
    graphics_draw_kalah(ctx, 0, kalah_cx_right, kalah_cy, kalah_height);
    
    int pit_area_x = kalah_cx_left + 90;
    int pit_area_w = kalah_cx_right - 90 - pit_area_x;
    int pit_spacing = pit_area_w / 6;
    int top_row_y = board.y + 110;
    int bottom_row_y = board.y + board.h - 110;
    
    // Player 2 (top row, displayed right-to-left so stones flow toward their store on the left)
    for (int i = 0; i < 6; i++) {
        int cx = pit_area_x + pit_spacing * (5 - i) + pit_spacing / 2;
        graphics_draw_pile(ctx, 1, i, cx, top_row_y);
    }
    
    // Player 1 (bottom row, left-to-right toward their store on the right)
    for (int i = 0; i < 6; i++) {
        int cx = pit_area_x + pit_spacing * i + pit_spacing / 2;
        graphics_draw_pile(ctx, 0, i, cx, bottom_row_y);
    }
    
    graphics_draw_text(ctx, "PLAYER 2", kalah_cx_left - 40, top_row_y - 90, COLOR_CREAM);
    graphics_draw_text(ctx, "PLAYER 1", kalah_cx_right - 40, bottom_row_y + 60, COLOR_CREAM);
    
    SDL_RenderPresent(ctx->renderer);
}

void graphics_draw_menu(GraphicsContext *ctx) {
    if (!ctx) return;
    
    graphics_clear(ctx, COLOR_DARK_GREEN);
    
    // Draw title
    graphics_draw_text(ctx, "*** KALAH ***", WINDOW_WIDTH / 2 - 60, 150, COLOR_GOLD);
    graphics_draw_text(ctx, "Ancient Game of Strategy", WINDOW_WIDTH / 2 - 120, 200, COLOR_WHITE);
    
    // Draw menu options, highlighting the currently selected one
    static const char *labels[4] = {"1. New Game", "2. Continue", "3. Help", "4. Exit"};
    int ys[4] = {300, 350, 400, 450};
    for (int i = 0; i < 4; i++) {
        Color color = (ctx->menu_selected == i) ? COLOR_GOLD : COLOR_WHITE;
        Rect option_rect = {WINDOW_WIDTH / 2 - 70, ys[i] - 5, 160, 30};
        if (ctx->menu_selected == i) {
            graphics_draw_rect(ctx, option_rect, COLOR_LIGHT_GREEN, 0);
        }
        graphics_draw_text(ctx, labels[i], WINDOW_WIDTH / 2 - 60, ys[i], color);
        ctx->menu_rects[i] = option_rect;
    }
    
    SDL_RenderPresent(ctx->renderer);
}

void graphics_draw_gameover(GraphicsContext *ctx) {
    if (!ctx || !ctx->engine) return;
    
    graphics_clear(ctx, COLOR_DARK_GREEN);
    
    GameState *state = ctx->engine->state;
    
    graphics_draw_text(ctx, "GAME OVER", WINDOW_WIDTH / 2 - 70, 150, COLOR_GOLD);
    
    if (state->winner == -1) {
        graphics_draw_text(ctx, "IT'S A TIE!", WINDOW_WIDTH / 2 - 60, 250, COLOR_WHITE);
    } else {
        char winner_text[50];
        sprintf(winner_text, "Player %d WINS!", state->winner + 1);
        graphics_draw_text(ctx, winner_text, WINDOW_WIDTH / 2 - 80, 250, COLOR_WHITE);
    }
    
    char score_text[100];
    sprintf(score_text, "Score - P1: %d, P2: %d", 
            state->players[0].kalah.count, 
            state->players[1].kalah.count);
    graphics_draw_text(ctx, score_text, WINDOW_WIDTH / 2 - 100, 350, COLOR_WHITE);
    
    graphics_draw_text(ctx, "Press SPACE to return to menu", WINDOW_WIDTH / 2 - 150, 450, COLOR_WHITE);
    
    SDL_RenderPresent(ctx->renderer);
}

/* ============== INPUT HANDLING ============== */

int graphics_get_pile_at_position(GraphicsContext *ctx, int x, int y, int *player, int *pile) {
    if (!ctx) return 0;
    
    // Check piles
    for (int p = 0; p < 2; p++) {
        for (int i = 0; i < 6; i++) {
            Rect r = ctx->pile_rects[p * 6 + i];
            if (x >= r.x && x <= r.x + r.w && y >= r.y && y <= r.y + r.h) {
                *player = p;
                *pile = i;
                return 1;
            }
        }
    }
    
    return 0;
}

int graphics_get_menu_option_at_position(GraphicsContext *ctx, int x, int y) {
    if (!ctx) return -1;
    
    for (int i = 0; i < 4; i++) {
        Rect r = ctx->menu_rects[i];
        if (x >= r.x && x <= r.x + r.w && y >= r.y && y <= r.y + r.h) {
            return i;
        }
    }
    
    return -1;
}

void graphics_activate_menu_option(GraphicsContext *ctx, int option) {
    if (!ctx) return;
    
    switch (option) {
        case 0:  // New Game
        case 1:  // Continue (no saved state yet, behaves like new game)
            engine_new_game(ctx->engine);
            ctx->game_state = 1;
            break;
        case 2:  // Help - handled by CLI/help text; no-op here for now
            break;
        case 3:  // Exit
            ctx->running = 0;
            break;
    }
}

/* ============== GAME LOOP ============== */

void graphics_handle_events(GraphicsContext *ctx) {
    if (!ctx) return;
    
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                ctx->running = 0;
                break;
            
            case SDL_KEYDOWN:
                if (ctx->game_state == 0) {  // Menu
                    if (event.key.keysym.sym == SDLK_UP) {
                        ctx->menu_selected = (ctx->menu_selected + 3) % 4;
                    } else if (event.key.keysym.sym == SDLK_DOWN) {
                        ctx->menu_selected = (ctx->menu_selected + 1) % 4;
                    } else if (event.key.keysym.sym == SDLK_RETURN || event.key.keysym.sym == SDLK_KP_ENTER) {
                        graphics_activate_menu_option(ctx, ctx->menu_selected);
                    } else if (event.key.keysym.sym == SDLK_1) {
                        graphics_activate_menu_option(ctx, 0);
                    } else if (event.key.keysym.sym == SDLK_2) {
                        graphics_activate_menu_option(ctx, 1);
                    } else if (event.key.keysym.sym == SDLK_3) {
                        graphics_activate_menu_option(ctx, 2);
                    } else if (event.key.keysym.sym == SDLK_4) {
                        graphics_activate_menu_option(ctx, 3);
                    }
                } else if (ctx->game_state == 2) {  // Game Over
                    if (event.key.keysym.sym == SDLK_SPACE) {
                        ctx->game_state = 0;
                    }
                }
                break;
            
            case SDL_MOUSEBUTTONDOWN:
                if (ctx->game_state == 0) {  // Menu
                    int option = graphics_get_menu_option_at_position(ctx, event.button.x, event.button.y);
                    if (option != -1) {
                        ctx->menu_selected = option;
                        graphics_activate_menu_option(ctx, option);
                    }
                } else if (ctx->game_state == 1) {  // Playing
                    int player, pile;
                    if (graphics_get_pile_at_position(ctx, event.button.x, event.button.y, &player, &pile)) {
                        if (player == ctx->engine->state->current_player) {
                            if (engine_execute_move(ctx->engine, player, pile)) {
                                if (engine_get_state(ctx->engine)->game_over) {
                                    ctx->game_state = 2;
                                }
                            }
                        }
                    }
                }
                break;

            case SDL_MOUSEMOTION:
                if (ctx->game_state == 0) {  // Menu
                    int option = graphics_get_menu_option_at_position(ctx, event.motion.x, event.motion.y);
                    if (option != -1) {
                        ctx->menu_selected = option;
                    }
                }
                break;
        }
    }
}

void graphics_update(GraphicsContext *ctx) {
    if (!ctx || !ctx->engine) return;
    
    // Update game state if needed
    // This is where game logic updates would go
}

void graphics_render(GraphicsContext *ctx) {
    if (!ctx) return;
    
    switch (ctx->game_state) {
        case 0:
            graphics_draw_menu(ctx);
            break;
        case 1:
            graphics_draw_board(ctx);
            break;
        case 2:
            graphics_draw_gameover(ctx);
            break;
    }
}

int graphics_run(GraphicsContext *ctx) {
    if (!ctx) return 1;
    
    while (ctx->running) {
        graphics_handle_events(ctx);
        graphics_update(ctx);
        graphics_render(ctx);
        SDL_Delay(16);  // ~60 FPS
    }
    
    return 0;
}
