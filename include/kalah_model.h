/**
 * @file kalah_model.h
 * @brief KALAH Game Data Model
 * @author KALAH Development Team
 * 
 * Centralizes all game data structures and basic game model
 */

#ifndef KALAH_MODEL_H
#define KALAH_MODEL_H

#include <stdio.h>
#include <stdlib.h>

/* ============== CONSTANTS ============== */
#define MAX_PILES 6
#define MAX_PLAYERS 2
#define PLAYER_1 0
#define PLAYER_2 1

/* ============== DATA STRUCTURES ============== */

/**
 * @struct Pile
 * @brief Represents a stone pile in the game
 */
typedef struct {
    int count;      // Current number of stones
    int capacity;   // Maximum capacity
} Pile;

/**
 * @struct Player
 * @brief Represents a player with their piles and kalah
 */
typedef struct {
    Pile kalah;                   // Player's score container
    Pile piles[MAX_PILES];        // 6 game piles per player
} Player;

/**
 * @struct GameState
 * @brief Represents the complete game state
 */
typedef struct {
    Player players[MAX_PLAYERS];
    Pile hand;                    // Temporary hand pile
    int current_player;           // Current turn player
    int last_pile;                // Last pile touched
    int game_over;                // Game state flag
    int winner;                   // Winner player (1-based, 0 if tie)
} GameState;

/* ============== FUNCTION DECLARATIONS ============== */

// Pile operations
Pile pile_create(int capacity);
void pile_destroy(Pile *p);
int pile_add(Pile *p, int stones);
int pile_remove(Pile *p, int count);
int pile_is_empty(const Pile *p);
int pile_count(const Pile *p);

// Game initialization
GameState* game_create(int initial_stones);
void game_destroy(GameState *game);
void game_reset(GameState *game, int initial_stones);

// Game logic
int game_is_over(const GameState *game);
int game_get_winner(const GameState *game);
void game_calculate_winner(GameState *game);
int game_move(GameState *game, int player, int pile_index);

// Game state queries
int game_get_current_player(const GameState *game);
int game_get_player_score(const GameState *game, int player);
int game_get_pile_count(const GameState *game, int player, int pile_index);
int game_all_piles_empty(const GameState *game, int player);

// Utilities
void game_print_state(const GameState *game);
char* game_state_to_string(const GameState *game);

#endif // KALAH_MODEL_H
