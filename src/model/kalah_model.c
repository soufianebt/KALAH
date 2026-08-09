/**
 * @file kalah_model.c
 * @brief KALAH Game Model Implementation
 */

#include "kalah_model.h"
#include <string.h>

/* ============== PILE OPERATIONS ============== */

Pile pile_create(int capacity) {
    Pile p = {0, capacity};
    return p;
}

void pile_destroy(Pile *p) {
    if (p) {
        p->count = 0;
        p->capacity = 0;
    }
}

int pile_add(Pile *p, int stones) {
    if (!p || stones < 0) return -1;
    
    int to_add = (p->count + stones > p->capacity) 
                 ? (p->capacity - p->count) 
                 : stones;
    p->count += to_add;
    return to_add;
}

int pile_remove(Pile *p, int count) {
    if (!p || count < 0 || count > p->count) return -1;
    
    p->count -= count;
    return count;
}

int pile_is_empty(const Pile *p) {
    return !p || p->count == 0;
}

int pile_count(const Pile *p) {
    return p ? p->count : -1;
}

/* ============== GAME INITIALIZATION ============== */

GameState* game_create(int initial_stones) {
    GameState *game = (GameState*)malloc(sizeof(GameState));
    if (!game) return NULL;
    
    memset(game, 0, sizeof(GameState));
    game_reset(game, initial_stones);
    
    return game;
}

void game_destroy(GameState *game) {
    if (game) {
        free(game);
    }
}

void game_reset(GameState *game, int initial_stones) {
    if (!game) return;
    
    memset(game, 0, sizeof(GameState));
    
    // Initialize both players
    for (int p = 0; p < MAX_PLAYERS; p++) {
        game->players[p].kalah = pile_create(100);
        
        for (int i = 0; i < MAX_PILES; i++) {
            game->players[p].piles[i] = pile_create(100);
            game->players[p].piles[i].count = initial_stones;
        }
    }
    
    game->hand = pile_create(100);
    game->current_player = PLAYER_1;
    game->game_over = 0;
    game->winner = -1;
}

/* ============== GAME LOGIC ============== */

int game_is_over(const GameState *game) {
    if (!game) return 1;
    return game->game_over;
}

int game_get_winner(const GameState *game) {
    if (!game) return -1;
    return game->winner;
}

void game_calculate_winner(GameState *game) {
    if (!game) return;
    
    int score1 = game->players[PLAYER_1].kalah.count;
    int score2 = game->players[PLAYER_2].kalah.count;
    
    if (score1 > score2) {
        game->winner = PLAYER_1;
    } else if (score2 > score1) {
        game->winner = PLAYER_2;
    } else {
        game->winner = -1; // Tie
    }
}

int game_move(GameState *game, int player, int pile_index) {
    if (!game || player < 0 || player >= MAX_PLAYERS) return 0;
    if (pile_index < 0 || pile_index >= MAX_PILES) return 0;
    
    Pile *source = &game->players[player].piles[pile_index];
    
    if (pile_is_empty(source)) {
        return 0; // Invalid move: empty pile
    }
    
    // Capture stones and return count
    int stones = source->count;
    source->count = 0;
    
    return stones;
}

/* ============== GAME STATE QUERIES ============== */

int game_get_current_player(const GameState *game) {
    return game ? game->current_player : -1;
}

int game_get_player_score(const GameState *game, int player) {
    if (!game || player < 0 || player >= MAX_PLAYERS) return -1;
    return game->players[player].kalah.count;
}

int game_get_pile_count(const GameState *game, int player, int pile_index) {
    if (!game || player < 0 || player >= MAX_PLAYERS) return -1;
    if (pile_index < 0 || pile_index >= MAX_PILES) return -1;
    
    return game->players[player].piles[pile_index].count;
}

int game_all_piles_empty(const GameState *game, int player) {
    if (!game || player < 0 || player >= MAX_PLAYERS) return 1;
    
    for (int i = 0; i < MAX_PILES; i++) {
        if (!pile_is_empty(&game->players[player].piles[i])) {
            return 0;
        }
    }
    return 1;
}

/* ============== UTILITIES ============== */

void game_print_state(const GameState *game) {
    if (!game) return;
    
    printf("\n========== KALAH GAME STATE ==========\n");
    printf("Current Player: %d\n", game->current_player + 1);
    printf("Game Over: %s\n", game->game_over ? "YES" : "NO");
    
    printf("\nPlayer 1 (Kalah: %d)\n", game->players[0].kalah.count);
    printf("Piles: ");
    for (int i = 0; i < MAX_PILES; i++) {
        printf("[%d] ", game->players[0].piles[i].count);
    }
    printf("\n");
    
    printf("\nPlayer 2 (Kalah: %d)\n", game->players[1].kalah.count);
    printf("Piles: ");
    for (int i = 0; i < MAX_PILES; i++) {
        printf("[%d] ", game->players[1].piles[i].count);
    }
    printf("\n");
    printf("=====================================\n\n");
}

char* game_state_to_string(const GameState *game) {
    // This can be implemented for GUI purposes later
    return "Game State";
}
