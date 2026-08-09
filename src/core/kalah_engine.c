/**
 * @file kalah_engine.c
 * @brief KALAH Game Engine Implementation
 */

#include "kalah_engine.h"
#include <string.h>

/* ============== ENGINE MANAGEMENT ============== */

GameEngine* engine_create(int initial_stones) {
    GameEngine *engine = (GameEngine*)malloc(sizeof(GameEngine));
    if (!engine) return NULL;
    
    engine->state = game_create(initial_stones);
    if (!engine->state) {
        free(engine);
        return NULL;
    }
    
    engine->move_count = 0;
    engine->replay_enabled = 0;
    memset(engine->move_history, -1, sizeof(engine->move_history));
    
    return engine;
}

void engine_destroy(GameEngine *engine) {
    if (!engine) return;
    
    if (engine->state) {
        game_destroy(engine->state);
    }
    free(engine);
}

void engine_reset(GameEngine *engine, int initial_stones) {
    if (!engine) return;
    
    game_reset(engine->state, initial_stones);
    engine->move_count = 0;
    memset(engine->move_history, -1, sizeof(engine->move_history));
}

/* ============== GAME RULES & LOGIC ============== */

int engine_validate_move(const GameEngine *engine, int player, int pile_index) {
    if (!engine || !engine->state) return 0;
    
    if (player < 0 || player >= MAX_PLAYERS) return 0;
    if (pile_index < 0 || pile_index >= MAX_PILES) return 0;
    
    // Check if it's the player's turn
    if (engine->state->current_player != player) return 0;
    
    // Check if pile is not empty
    if (game_get_pile_count(engine->state, player, pile_index) <= 0) return 0;
    
    return 1;
}

/* Board modeled as 14 circular slots: 0-5 = P1 piles, 6 = P1 kalah,
 * 7-12 = P2 piles, 13 = P2 kalah. Sowing skips the opponent's kalah slot. */
#define BOARD_SLOTS 14
#define BOARD_P1_KALAH 6
#define BOARD_P2_KALAH 13

int engine_execute_move(GameEngine *engine, int player, int pile_index) {
    if (!engine_validate_move(engine, player, pile_index)) {
        return 0;
    }
    
    // Capture stones from the chosen pile
    int stones = game_move(engine->state, player, pile_index);
    if (stones <= 0) return 0;
    
    // Store move in history
    if (engine->move_count < 100) {
        engine->move_history[engine->move_count++] = pile_index;
    }
    
    // Sow the captured stones around the board (pile is already emptied by game_move)
    int last_slot = engine_distribute_stones(engine, player, pile_index, stones);
    
    GameState *state = engine->state;
    int own_kalah_slot = (player == PLAYER_1) ? BOARD_P1_KALAH : BOARD_P2_KALAH;
    int extra_turn = (last_slot == own_kalah_slot);
    
    // Capture rule: last stone landed in an own, previously-empty pit
    if (!extra_turn) {
        int own_base = (player == PLAYER_1) ? 0 : MAX_PILES + 1;
        if (last_slot >= own_base && last_slot < own_base + MAX_PILES) {
            int local_index = last_slot - own_base;
            Player *me = &state->players[player];
            if (me->piles[local_index].count == 1) {
                int opposite_index = MAX_PILES - 1 - local_index;
                Player *opp = &state->players[1 - player];
                int captured = me->piles[local_index].count + opp->piles[opposite_index].count;
                me->kalah.count += captured;
                me->piles[local_index].count = 0;
                opp->piles[opposite_index].count = 0;
            }
        }
    }
    
    // Check if game is over
    if (!engine_check_game_over(engine) && !extra_turn) {
        engine_end_turn(engine);
    }
    
    return 1;
}

/* Board modeled as 14 circular slots: 0-5 = P1 piles, 6 = P1 kalah,
 * 7-12 = P2 piles, 13 = P2 kalah. Sowing skips the opponent's kalah slot. */
#define BOARD_SLOTS 14
#define BOARD_P1_KALAH 6
#define BOARD_P2_KALAH 13

static int pile_to_slot(int player, int pile_index) {
    return (player == PLAYER_1) ? pile_index : (MAX_PILES + 1 + pile_index);
}

static int is_opponent_kalah(int player, int slot) {
    return (player == PLAYER_1 && slot == BOARD_P2_KALAH) ||
           (player == PLAYER_2 && slot == BOARD_P1_KALAH);
}

static void add_stone_to_slot(GameState *state, int slot) {
    if (slot == BOARD_P1_KALAH) {
        state->players[PLAYER_1].kalah.count++;
    } else if (slot == BOARD_P2_KALAH) {
        state->players[PLAYER_2].kalah.count++;
    } else if (slot < BOARD_P1_KALAH) {
        state->players[PLAYER_1].piles[slot].count++;
    } else {
        state->players[PLAYER_2].piles[slot - (MAX_PILES + 1)].count++;
    }
}

int engine_distribute_stones(GameEngine *engine, int player, int start_pile, int stones) {
    if (!engine || !engine->state) return -1;
    if (stones <= 0) return -1;
    
    GameState *state = engine->state;
    int slot = pile_to_slot(player, start_pile);
    int last_slot = slot;
    
    while (stones > 0) {
        slot = (slot + 1) % BOARD_SLOTS;
        if (is_opponent_kalah(player, slot)) {
            continue;  // opponent's kalah is skipped, does not consume a stone
        }
        add_stone_to_slot(state, slot);
        last_slot = slot;
        stones--;
    }
    
    return last_slot;
}

void engine_end_turn(GameEngine *engine) {
    if (!engine || !engine->state) return;
    
    // Switch to next player
    engine->state->current_player = (engine->state->current_player + 1) % MAX_PLAYERS;
}

int engine_check_game_over(GameEngine *engine) {
    if (!engine || !engine->state) return 1;
    
    // Game ends when all piles of both players are empty
    int player1_empty = game_all_piles_empty(engine->state, PLAYER_1);
    int player2_empty = game_all_piles_empty(engine->state, PLAYER_2);
    
    if (player1_empty && player2_empty) {
        engine->state->game_over = 1;
        game_calculate_winner(engine->state);
        return 1;
    }
    
    return 0;
}

/* ============== GAME FLOW ============== */

void engine_new_game(GameEngine *engine) {
    if (!engine) return;
    
    engine_reset(engine, 4); // Standard 4 stones per pile
}

void engine_continue_game(GameEngine *engine) {
    if (!engine) return;
    
    engine->state->game_over = 0;
    engine->state->current_player = PLAYER_1;
}

void engine_finish_game(GameEngine *engine) {
    if (!engine || !engine->state) return;
    
    engine->state->game_over = 1;
    game_calculate_winner(engine->state);
}

/* ============== QUERIES ============== */

const GameState* engine_get_state(const GameEngine *engine) {
    return engine ? engine->state : NULL;
}

int engine_get_valid_moves(const GameEngine *engine, int player, int *moves) {
    if (!engine || !moves) return 0;
    
    int count = 0;
    for (int i = 0; i < MAX_PILES; i++) {
        if (engine_validate_move(engine, player, i)) {
            moves[count++] = i;
        }
    }
    
    return count;
}

/* ============== DISPLAY ============== */

void engine_display_board(const GameEngine *engine) {
    if (!engine || !engine->state) return;
    
    GameState *state = engine->state;
    
    printf("\n");
    printf("============================================\n");
    printf("|        KALAH GAME BOARD                  |\n");
    printf("============================================\n\n");
    
    printf("              PLAYER 2\n");
    printf("    +---------------------+\n");
    printf("    |  ");
    for (int i = MAX_PILES - 1; i >= 0; i--) {
        printf("[%d] ", state->players[PLAYER_2].piles[i].count);
    }
    printf("|\n");
    printf(" [%d]|                     |[%d]\n", 
           state->players[PLAYER_2].kalah.count,
           state->players[PLAYER_1].kalah.count);
    printf("    |  ");
    for (int i = 0; i < MAX_PILES; i++) {
        printf("[%d] ", state->players[PLAYER_1].piles[i].count);
    }
    printf("|\n");
    printf("    +---------------------+\n");
    printf("              PLAYER 1\n\n");
    
    printf("Current Turn: Player %d\n", state->current_player + 1);
    printf("Status: %s\n", state->game_over ? "GAME OVER" : "IN PROGRESS");
    
    if (state->game_over) {
        if (state->winner == -1) {
            printf("Result: TIE\n");
        } else {
            printf("Winner: Player %d\n", state->winner + 1);
        }
    }
    printf("\n");
}
