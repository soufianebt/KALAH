/**
 * @file cli_interface.c
 * @brief KALAH CLI Implementation
 */

#include "cli_interface.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ============== CONSTANTS ============== */

#define MENU_NEW_GAME 1
#define MENU_CONTINUE 2
#define MENU_HELP 3
#define MENU_EXIT 4

/* ============== INITIALIZATION ============== */

CLIContext* cli_create(int initial_stones) {
    CLIContext *ctx = (CLIContext*)malloc(sizeof(CLIContext));
    if (!ctx) return NULL;
    
    ctx->engine = engine_create(initial_stones);
    if (!ctx->engine) {
        free(ctx);
        return NULL;
    }
    
    ctx->running = 1;
    ctx->input_valid = 1;
    
    return ctx;
}

void cli_destroy(CLIContext *ctx) {
    if (!ctx) return;
    
    if (ctx->engine) {
        engine_destroy(ctx->engine);
    }
    free(ctx);
}

/* ============== DISPLAY FUNCTIONS ============== */

void cli_clear_screen(void) {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

void cli_show_title(void) {
    printf("\n");
    printf("==========================================================\n");
    printf("|                                                        |\n");
    printf("|                  *** KALAH ***                         |\n");
    printf("|                                                        |\n");
    printf("|           An Ancient Game of Strategy                  |\n");
    printf("|                                                        |\n");
    printf("==========================================================\n");
    printf("\n");
}

void cli_show_message(const char *message) {
    if (!message) return;
    printf("\n>> %s\n", message);
}

void cli_show_board(const CLIContext *ctx) {
    if (!ctx || !ctx->engine) return;
    
    cli_clear_screen();
    cli_show_title();
    engine_display_board(ctx->engine);
}

void cli_display_help(void) {
    cli_clear_screen();
    cli_show_title();
    
    printf("================================================== ==================================================\n");
    printf("|                       HOW TO PLAY                     |\n");
    printf("================================================== ==================================================\n\n");
    
    printf("KALAH RULES:\n");
    printf("============\n");
    printf("1. Each player has 6 piles and 1 Kalah (storage)\n");
    printf("2. Players alternate turns\n");
    printf("3. On your turn, choose a pile and distribute all stones\n");
    printf("4. Distribute stones counter-clockwise, one per pile\n");
    printf("5. If you land in your Kalah, you get an extra turn\n");
    printf("6. If you land in an empty pile on your side, capture\n");
    printf("7. Game ends when all piles on one side are empty\n");
    printf("8. Remaining stones go to respective Kalahs\n");
    printf("9. Player with most stones in Kalah wins\n\n");
    
    printf("CONTROLS:\n");
    printf("=========\n");
    printf("Enter the pile number (1-6) for your move\n");
    printf("Follow the on-screen prompts\n\n");
    
    printf("Press Enter to return to menu...");
    getchar();
}

/* ============== INPUT HANDLING ============== */

int cli_get_menu_choice(const char *prompt, int min, int max) {
    int choice = -1;
    
    while (choice < min || choice > max) {
        printf("%s (%d-%d): ", prompt, min, max);
        
        if (scanf("%d", &choice) != 1) {
            while (getchar() != '\n');  // Clear input buffer
            choice = -1;
            printf("Invalid input! Please enter a number.\n");
            continue;
        }
        
        if (choice < min || choice > max) {
            printf("Please enter a number between %d and %d.\n", min, max);
        }
    }
    
    return choice;
}

int cli_get_player_move(CLIContext *ctx, int player) {
    if (!ctx || !ctx->engine) return -1;
    
    int valid_moves[MAX_PILES];
    int move_count = engine_get_valid_moves(ctx->engine, player, valid_moves);
    
    if (move_count == 0) {
        cli_show_message("No valid moves available!");
        return -1;
    }
    
    printf("\nPlayer %d's turn\n", player + 1);
    printf("Valid piles: ");
    for (int i = 0; i < move_count; i++) {
        printf("%d ", valid_moves[i] + 1);
    }
    printf("\n");
    
    int choice = cli_get_menu_choice("Choose a pile", 1, MAX_PILES) - 1;
    
    if (choice < 0 || choice >= MAX_PILES) {
        cli_show_message("Invalid pile number!");
        return -1;
    }
    
    return choice;
}

/* ============== MENU INTERFACE ============== */

void cli_main_menu(CLIContext *ctx) {
    if (!ctx) return;
    
    int choice = -1;
    
    while (choice != MENU_EXIT) {
        cli_clear_screen();
        cli_show_title();
        
        printf("==========================================================\n");
        printf("|                  MAIN MENU                             |\n");
        printf("==========================================================\n\n");
        
        printf("1. New Game\n");
        printf("2. Continue Previous Game\n");
        printf("3. Help / Rules\n");
        printf("4. Exit\n\n");
        
        choice = cli_get_menu_choice("Select an option", 1, 4);
        
        switch (choice) {
            case MENU_NEW_GAME:
                engine_new_game(ctx->engine);
                cli_play_game(ctx);
                break;
            case MENU_CONTINUE:
                engine_continue_game(ctx->engine);
                cli_play_game(ctx);
                break;
            case MENU_HELP:
                cli_display_help();
                break;
            case MENU_EXIT:
                cli_clear_screen();
                printf("Thank you for playing KALAH! Goodbye!\n\n");
                ctx->running = 0;
                break;
        }
    }
}

/* ============== GAME LOOP ============== */

void cli_play_game(CLIContext *ctx) {
    if (!ctx || !ctx->engine) return;
    
    cli_show_board(ctx);
    cli_game_loop(ctx);
}

void cli_game_loop(CLIContext *ctx) {
    if (!ctx || !ctx->engine) return;
    
    GameEngine *engine = ctx->engine;
    const GameState *state = engine_get_state(engine);
    
    while (!state->game_over) {
        cli_show_board(ctx);
        
        int player = state->current_player;
        int pile = cli_get_player_move(ctx, player);
        
        if (pile >= 0) {
            if (engine_execute_move(engine, player, pile)) {
                cli_show_message("Move executed successfully!");
            } else {
                cli_show_message("Invalid move! Try again.");
            }
        }
        
        printf("\nPress Enter to continue...");
        getchar();
        getchar();  // Extra getchar for the newline after scanf
    }
    
    // Display end game screen
    cli_show_board(ctx);
    
    if (state->winner == -1) {
        printf("\n*** GAME OVER! IT'S A TIE! ***\n");
    } else {
        printf("\n*** GAME OVER! PLAYER %d WINS! ***\n", state->winner + 1);
        printf("Final Score - Player 1: %d, Player 2: %d\n",
               state->players[0].kalah.count,
               state->players[1].kalah.count);
    }
    
    printf("\nPress Enter to return to main menu...");
    getchar();
}

/* ============== MAIN ENTRY POINT ============== */

void cli_run(CLIContext *ctx) {
    if (!ctx) return;
    
    cli_main_menu(ctx);
}
