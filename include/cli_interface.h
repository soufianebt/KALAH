/**
 * @file cli_interface.h
 * @brief KALAH Command-Line Interface
 * 
 * Provides a text-based user interface for playing KALAH
 */

#ifndef CLI_INTERFACE_H
#define CLI_INTERFACE_H

#include "kalah_engine.h"

/* ============== CLI INTERFACE ============== */

typedef struct {
    GameEngine *engine;
    int running;
    int input_valid;
} CLIContext;

/* Function declarations */

// Initialization
CLIContext* cli_create(int initial_stones);
void cli_destroy(CLIContext *ctx);

// Main game loop
void cli_run(CLIContext *ctx);
void cli_main_menu(CLIContext *ctx);
void cli_play_game(CLIContext *ctx);
void cli_game_loop(CLIContext *ctx);

// User input
int cli_get_player_move(CLIContext *ctx, int player);
int cli_get_menu_choice(const char *prompt, int min, int max);
void cli_display_help(void);

// Display functions
void cli_show_title(void);
void cli_show_board(const CLIContext *ctx);
void cli_show_message(const char *message);
void cli_clear_screen(void);

#endif // CLI_INTERFACE_H
