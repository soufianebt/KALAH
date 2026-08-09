/**
 * @file graphics_interface.h
 * @brief KALAH SDL2 Graphics Interface
 * 
 * Provides a graphical user interface using SDL2
 */

#ifndef GRAPHICS_INTERFACE_H
#define GRAPHICS_INTERFACE_H

#include "kalah_engine.h"
#include <SDL2/SDL.h>

/* ============== CONSTANTS ============== */

#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 768
#define WINDOW_TITLE "KALAH - Ancient Game of Strategy"

/* Colors */
typedef struct {
    Uint8 r, g, b, a;
} Color;

#define COLOR_BLACK (Color){0, 0, 0, 255}
#define COLOR_WHITE (Color){255, 255, 255, 255}
#define COLOR_GRAY (Color){128, 128, 128, 255}
#define COLOR_DARK_GREEN (Color){34, 139, 34, 255}
#define COLOR_LIGHT_GREEN (Color){144, 238, 144, 255}
#define COLOR_GOLD (Color){255, 215, 0, 255}
#define COLOR_RED (Color){220, 20, 60, 255}

/* Modern wooden-board palette */
#define COLOR_WOOD_DARK (Color){87, 58, 41, 255}
#define COLOR_WOOD_MED (Color){122, 80, 51, 255}
#define COLOR_WOOD_LIGHT (Color){166, 116, 76, 255}
#define COLOR_CREAM (Color){237, 220, 191, 255}
#define COLOR_PIT_SHADOW (Color){60, 40, 28, 255}
#define COLOR_STONE (Color){45, 30, 20, 255}
#define COLOR_ACCENT_BLUE (Color){66, 135, 245, 255}
#define COLOR_ACCENT_RED (Color){235, 87, 87, 255}
#define COLOR_HIGHLIGHT (Color){255, 193, 7, 255}

/* ============== DATA STRUCTURES ============== */

typedef struct {
    int x, y, w, h;  // Position and size
} Rect;

typedef struct {
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *font_texture;
    GameEngine *engine;
    int running;
    int game_state;  // 0=menu, 1=playing, 2=gameover
    int selected_pile;  // -1 if none selected
    Rect pile_rects[12];  // 6 piles per player
    Rect kalah_rects[2];  // Kalah for each player
    int menu_selected;  // Currently highlighted menu option (0-3)
    Rect menu_rects[4]; // Clickable areas for menu options
} GraphicsContext;

/* ============== FUNCTION DECLARATIONS ============== */

// Initialization
GraphicsContext* graphics_create(int initial_stones);
void graphics_destroy(GraphicsContext *ctx);

// Main loop
int graphics_run(GraphicsContext *ctx);
void graphics_handle_events(GraphicsContext *ctx);
void graphics_update(GraphicsContext *ctx);
void graphics_render(GraphicsContext *ctx);

// Rendering
void graphics_clear(GraphicsContext *ctx, Color color);
void graphics_draw_rect(GraphicsContext *ctx, Rect r, Color color, int filled);
void graphics_draw_circle(GraphicsContext *ctx, int x, int y, int radius, Color color, int filled);
void graphics_draw_text(GraphicsContext *ctx, const char *text, int x, int y, Color color);
void graphics_draw_panel(GraphicsContext *ctx, Rect r, Color base, int bevel);

// Game rendering
void graphics_draw_menu(GraphicsContext *ctx);
void graphics_draw_board(GraphicsContext *ctx);
void graphics_draw_gameover(GraphicsContext *ctx);
void graphics_draw_pile(GraphicsContext *ctx, int player, int pile, int cx, int cy);
void graphics_draw_kalah(GraphicsContext *ctx, int player, int cx, int cy, int height);

// Input handling
int graphics_get_pile_at_position(GraphicsContext *ctx, int x, int y, int *player, int *pile);
int graphics_get_menu_option_at_position(GraphicsContext *ctx, int x, int y);
void graphics_activate_menu_option(GraphicsContext *ctx, int option);

#endif // GRAPHICS_INTERFACE_H
