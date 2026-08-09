/**
 * @file kalah_engine.h
 * @brief KALAH Game Engine - Core Game Logic
 * 
 * Implements the rules and logic of the KALAH game
 */

#ifndef KALAH_ENGINE_H
#define KALAH_ENGINE_H

#include "kalah_model.h"

/* ============== GAME ENGINE ============== */

typedef struct {
    GameState *state;
    int move_history[100];  // Track moves for replay
    int move_count;
    int replay_enabled;
} GameEngine;

/* Function declarations */

// Engine management
GameEngine* engine_create(int initial_stones);
void engine_destroy(GameEngine *engine);
void engine_reset(GameEngine *engine, int initial_stones);

// Game rules & logic
int engine_validate_move(const GameEngine *engine, int player, int pile_index);
int engine_execute_move(GameEngine *engine, int player, int pile_index);
int engine_distribute_stones(GameEngine *engine, int player, int start_pile, int stones);
void engine_end_turn(GameEngine *engine);
int engine_check_game_over(GameEngine *engine);

// Game flow
void engine_new_game(GameEngine *engine);
void engine_continue_game(GameEngine *engine);
void engine_finish_game(GameEngine *engine);

// Queries
const GameState* engine_get_state(const GameEngine *engine);
int engine_get_valid_moves(const GameEngine *engine, int player, int *moves);

// Display
void engine_display_board(const GameEngine *engine);

#endif // KALAH_ENGINE_H
