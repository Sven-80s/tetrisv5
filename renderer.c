#include "renderer.h"

#include <ncurses.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief Interne Konstanten für Layout
 */
#define BOARD_OFFSET_Y 2
#define BOARD_OFFSET_X 2
#define SIDEBAR_OFFSET_X 28

bool renderer_init(void) {
    initscr();
    
    if (!has_colors()) {
        endwin();
        fprintf(stderr, "Error: Terminal does not support colors\n");
        return false;
    }
    
    if (start_color() != OK) {
        endwin();
        fprintf(stderr, "Error: Failed to initialize colors\n");
        return false;
    }
    
    cbreak();
    noecho();
    nodelay(stdscr, TRUE);
    keypad(stdscr, TRUE);
    curs_set(0);
    
    /* Farbpaare initialisieren */
    init_pair(COLOR_I, COLOR_CYAN, COLOR_BLACK);
    init_pair(COLOR_O, COLOR_YELLOW, COLOR_BLACK);
    init_pair(COLOR_T, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(COLOR_S, COLOR_GREEN, COLOR_BLACK);
    init_pair(COLOR_Z, COLOR_RED, COLOR_BLACK);
    init_pair(COLOR_J, COLOR_BLUE, COLOR_BLACK);
    init_pair(COLOR_L, COLOR_WHITE, COLOR_BLACK);  /* Orange nicht verfügbar, weiß nutzen */
    init_pair(COLOR_EMPTY, COLOR_BLACK, COLOR_BLACK);
    
    return true;
}

void renderer_cleanup(void) {
    endwin();
}

void renderer_render(const Game *game) {
    if (game == NULL) return;
    
    clear();
    
    /* Titel */
    attron(A_BOLD);
    mvprintw(0, BOARD_OFFSET_X, "T E T R I S");
    attroff(A_BOLD);
    
    renderer_render_board(game, BOARD_OFFSET_Y, BOARD_OFFSET_X);
    renderer_render_sidebar(game, BOARD_OFFSET_Y, SIDEBAR_OFFSET_X);
    renderer_render_help(BOARD_OFFSET_Y + 15, SIDEBAR_OFFSET_X);
    
    if (game->state == STATE_PAUSED) {
        renderer_render_pause();
    }
    
    if (game->state == STATE_GAME_OVER) {
        renderer_render_game_over(game->score);
    }
    
    refresh();
}

void renderer_render_board(const Game *game, int start_y, int start_x) {
    if (game == NULL) return;
    
    /* Virtuelles Display erstellen (Kopie des Boards) */
    int display[BOARD_HEIGHT][BOARD_WIDTH];
    memcpy(display, game->board, sizeof(display));
    
    /* Aktuelles Piece auf das Display rendern */
    renderer_render_piece_to_display(game, display);
    
    /* Board mit Rahmen zeichnen */
    /* Obere Rahmenlinie */
    mvaddch(start_y - 1, start_x - 1, ACS_ULCORNER);
    for (int x = 0; x < BOARD_WIDTH; x++) {
        mvaddch(start_y - 1, start_x + x, ACS_HLINE);
    }
    mvaddch(start_y - 1, start_x + BOARD_WIDTH, ACS_URCORNER);
    
    /* Seiten und Inhalt */
    for (int y = 0; y < BOARD_HEIGHT; y++) {
        mvaddch(start_y + y, start_x - 1, ACS_VLINE);
        
        for (int x = 0; x < BOARD_WIDTH; x++) {
            int color = display[y][x];
            
            if (color != 0) {
                attron(COLOR_PAIR(color));
                attron(A_BOLD);
                mvaddch(start_y + y, start_x + x, BLOCK_CHAR);
                attroff(A_BOLD);
                attroff(COLOR_PAIR(color));
            } else {
                mvaddch(start_y + y, start_x + x, EMPTY_CHAR);
            }
        }
        
        mvaddch(start_y + y, start_x + BOARD_WIDTH, ACS_VLINE);
    }
    
    /* Untere Rahmenlinie */
    mvaddch(start_y + BOARD_HEIGHT, start_x - 1, ACS_LLCORNER);
    for (int x = 0; x < BOARD_WIDTH; x++) {
        mvaddch(start_y + BOARD_HEIGHT, start_x + x, ACS_HLINE);
    }
    mvaddch(start_y + BOARD_HEIGHT, start_x + BOARD_WIDTH, ACS_LRCORNER);
}

void renderer_render_piece_to_display(const Game *game, int display[BOARD_HEIGHT][BOARD_WIDTH]) {
    if (game == NULL) return;
    
    const int *shape = tetromino_get_shape(game->current_piece.type, 
                                            game->current_piece.rotation);
    if (shape == NULL) return;
    
    int color = tetromino_get_color(game->current_piece.type);
    
    for (int row = 0; row < TETROMINO_SIZE; row++) {
        for (int col = 0; col < TETROMINO_SIZE; col++) {
            if (shape[row * TETROMINO_SIZE + col] != 0) {
                int board_x = game->current_piece.x + col;
                int board_y = game->current_piece.y + row;
                
                if (board_y >= 0 && board_y < BOARD_HEIGHT && 
                    board_x >= 0 && board_x < BOARD_WIDTH) {
                    display[board_y][board_x] = color;
                }
            }
        }
    }
}

void renderer_render_sidebar(const Game *game, int start_y, int start_x) {
    if (game == NULL) return;
    
    /* Score */
    attron(A_BOLD);
    mvprintw(start_y, start_x, "SCORE");
    attroff(A_BOLD);
    mvprintw(start_y + 1, start_x, "%d", game->score);
    
    /* Level */
    attron(A_BOLD);
    mvprintw(start_y + 3, start_x, "LEVEL");
    attroff(A_BOLD);
    mvprintw(start_y + 4, start_x, "%d", game->level);
    
    /* Lines */
    attron(A_BOLD);
    mvprintw(start_y + 6, start_x, "LINES");
    attroff(A_BOLD);
    mvprintw(start_y + 7, start_x, "%d", game->lines_cleared);
    
    /* Next Piece */
    attron(A_BOLD);
    mvprintw(start_y + 9, start_x, "NEXT");
    attroff(A_BOLD);
    renderer_render_next_piece(&game->next_piece, start_y + 10, start_x);
}

void renderer_render_next_piece(const Tetromino *next, int start_y, int start_x) {
    if (next == NULL) return;
    
    const int *shape = tetromino_get_shape(next->type, 0);
    if (shape == NULL) return;
    
    int color = tetromino_get_color(next->type);
    
    for (int row = 0; row < 4; row++) {
        for (int col = 0; col < 4; col++) {
            int shape_value = shape[row * TETROMINO_SIZE + col];
            
            if (shape_value != 0) {
                attron(COLOR_PAIR(color));
                attron(A_BOLD);
                mvaddch(start_y + row, start_x + col, BLOCK_CHAR);
                attroff(A_BOLD);
                attroff(COLOR_PAIR(color));
            } else {
                mvaddch(start_y + row, start_x + col, ' ');
            }
        }
    }
}

void renderer_render_pause(void) {
    int center_y = BOARD_HEIGHT / 2 + BOARD_OFFSET_Y;
    int center_x = BOARD_WIDTH / 2 + BOARD_OFFSET_X - 4;
    
    attron(A_BOLD | A_REVERSE);
    mvprintw(center_y - 1, center_x - 2, "              ");
    mvprintw(center_y, center_x - 2, "   P A U S E D   ");
    mvprintw(center_y + 1, center_x - 2, "              ");
    attroff(A_BOLD | A_REVERSE);
}

void renderer_render_game_over(int final_score) {
    int center_y = BOARD_HEIGHT / 2 + BOARD_OFFSET_Y;
    int center_x = BOARD_WIDTH / 2 + BOARD_OFFSET_X - 6;
    
    attron(A_BOLD | A_REVERSE);
    mvprintw(center_y - 2, center_x - 2, "                    ");
    mvprintw(center_y - 1, center_x - 2, "   G A M E  O V E R   ");
    mvprintw(center_y, center_x - 2, "  Score: %d          ", final_score);
    mvprintw(center_y + 1, center_x - 2, "  Press Q to quit    ");
    mvprintw(center_y + 2, center_x - 2, "                    ");
    attroff(A_BOLD | A_REVERSE);
}

void renderer_render_help(int start_y, int start_x) {
    attron(A_BOLD);
    mvprintw(start_y, start_x, "CONTROLS");
    attroff(A_BOLD);
    mvprintw(start_y + 1, start_x, "Arrow: Move");
    mvprintw(start_y + 2, start_x, "Up/Space: Rotate");
    mvprintw(start_y + 3, start_x, "P: Pause");
    mvprintw(start_y + 4, start_x, "Q: Quit");
}

void renderer_refresh_screen(const Game *game) {
    renderer_render(game);
}

int renderer_get_color_pair(int value) {
    if (value >= COLOR_I && value <= COLOR_EMPTY) {
        return value;
    }
    return COLOR_EMPTY;
}

void renderer_show_error(const char *message) {
    endwin();
    fprintf(stderr, "Error: %s\n", message);
    exit(EXIT_FAILURE);
}
