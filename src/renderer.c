/**
 * @file renderer.c
 * @brief Rendering module implementation
 */

#include "renderer.h"

#include <ncurses.h>
#include <string.h>

/**
 * @brief Internal flag to track initialization state
 */
static int renderer_initialized = 0;

/**
 * @brief ncurses color pair for each tetromino type
 */
static const int TETRO_COLOR_PAIRS[TETRO_COUNT] = {
    COLOR_I,  /* I - Cyan */
    COLOR_O,  /* O - Yellow */
    COLOR_T,  /* T - Magenta */
    COLOR_S,  /* S - Green */
    COLOR_Z,  /* Z - Red */
    COLOR_J,  /* J - Blue */
    COLOR_L   /* L - White */
};

/**
 * @brief ncurses color constant for each pair
 */
static const int COLOR_VALUES[TETRO_COUNT] = {
    COLOR_CYAN,    /* I */
    COLOR_YELLOW,  /* O */
    COLOR_MAGENTA, /* T */
    COLOR_GREEN,   /* S */
    COLOR_RED,     /* Z */
    COLOR_BLUE,    /* J */
    COLOR_WHITE    /* L */
};

/**
 * @brief Character used to render filled cells
 */
static const char CELL_CHAR[] = "██";

/**
 * @brief Character used for empty cells (just spaces)
 */
static const char EMPTY_CELL[] = "  ";

void renderer_init(void)
{
    if (renderer_initialized) {
        return;
    }

    /* Initialize ncurses */
    initscr();
    
    /* Enable colors */
    if (has_colors()) {
        start_color();
        use_default_colors();
        
        /* Initialize color pairs for each tetromino */
        for (int i = 0; i < TETRO_COUNT; i++) {
            init_pair(TETRO_COLOR_PAIRS[i], COLOR_BLACK, COLOR_VALUES[i]);
        }
        
        /* Default color pair for empty cells */
        init_pair(8, COLOR_WHITE, COLOR_BLACK);
    }
    
    /* Don't show cursor */
    curs_set(0);
    
    /* Disable line buffering and echo */
    cbreak();
    noecho();
    
    /* Enable special keys */
    keypad(stdscr, TRUE);
    
    /* Clear screen */
    clear();
    
    renderer_initialized = 1;
}

void renderer_cleanup(void)
{
    if (!renderer_initialized) {
        return;
    }
    
    /* Show cursor again */
    curs_set(1);
    
    /* End ncurses mode */
    endwin();
    
    renderer_initialized = 0;
}

/**
 * @brief Draw a single cell
 */
static void draw_cell(int x, int y, int color_pair, int filled)
{
    attron(COLOR_PAIR(color_pair));
    if (filled) {
        mvprintw(y, x, "%s", CELL_CHAR);
    } else {
        mvprintw(y, x, "%s", EMPTY_CELL);
    }
    attroff(COLOR_PAIR(color_pair));
}

/**
 * @brief Get color pair for a board cell value
 */
static int get_cell_color_pair(Cell cell)
{
    if (cell >= COLOR_I && cell <= COLOR_L) {
        return cell;
    }
    return 8; /* Default black */
}

void renderer_draw_board(const Board *board, const Tetromino *current)
{
    if (!renderer_initialized || board == NULL) {
        return;
    }
    
    int start_x = BOARD_DISPLAY_X;
    int start_y = BOARD_DISPLAY_Y;
    
    /* Draw top border */
    mvprintw(start_y - 1, start_x, "┌");
    for (int x = 0; x < BOARD_WIDTH; x++) {
        printw("──");
    }
    printw("┐");
    
    /* Draw board cells */
    for (int y = 0; y < BOARD_HEIGHT; y++) {
        mvprintw(start_y + y, start_x, "│");
        
        for (int x = 0; x < BOARD_WIDTH; x++) {
            Cell cell = board->cells[y][x];
            int color_pair = get_cell_color_pair(cell);
            int filled = (cell != 0);
            
            draw_cell(start_x + 1 + x * BOARD_CELL_WIDTH, start_y + y, 
                     color_pair, filled);
        }
        
        mvprintw(start_y + y, start_x + 1 + BOARD_WIDTH * BOARD_CELL_WIDTH, "│");
    }
    
    /* Draw current piece if provided */
    if (current != NULL) {
        const int (*shape)[TETRO_MATRIX_SIZE] = tetromino_get_shape(current->type, current->rotation);
        int color_pair = tetromino_get_color(current->type);
        
        if (shape != NULL && color_pair >= 0) {
            for (int y = 0; y < TETRO_MATRIX_SIZE; y++) {
                for (int x = 0; x < TETRO_MATRIX_SIZE; x++) {
                    if (shape[y][x]) {
                        int board_x = current->x + x;
                        int board_y = current->y + y;
                        
                        /* Only draw if within board bounds */
                        if (board_x >= 0 && board_x < BOARD_WIDTH &&
                            board_y >= 0 && board_y < BOARD_HEIGHT) {
                            draw_cell(start_x + 1 + board_x * BOARD_CELL_WIDTH, 
                                     start_y + board_y, color_pair, 1);
                        }
                    }
                }
            }
        }
    }
    
    /* Draw bottom border */
    mvprintw(start_y + BOARD_HEIGHT, start_x, "└");
    for (int x = 0; x < BOARD_WIDTH; x++) {
        printw("──");
    }
    printw("┘");
}

void renderer_draw_next_piece(TetrominoType next_type)
{
    if (!renderer_initialized) {
        return;
    }
    
    int box_x = SIDEBAR_X;
    int box_y = 3;
    int color_pair = tetromino_get_color(next_type);
    
    /* Draw "NEXT" label */
    mvprintw(1, box_x + 6, "NEXT");
    
    /* Draw preview box border */
    mvprintw(box_y, box_x, "┌────────┐");
    for (int y = 1; y <= 4; y++) {
        mvprintw(box_y + y, box_x, "│");
        mvprintw(box_y + y, box_x + 8, "│");
    }
    mvprintw(box_y + 5, box_x, "└────────┘");
    
    /* Get shape for next piece */
    const int (*shape)[TETRO_MATRIX_SIZE] = tetromino_get_shape(next_type, 0);
    if (shape == NULL || color_pair < 0) {
        return;
    }
    
    /* Calculate offset to center the piece in the 4x4 box */
    int min_x = TETRO_MATRIX_SIZE, max_x = -1;
    int min_y = TETRO_MATRIX_SIZE, max_y = -1;
    
    for (int y = 0; y < TETRO_MATRIX_SIZE; y++) {
        for (int x = 0; x < TETRO_MATRIX_SIZE; x++) {
            if (shape[y][x]) {
                if (x < min_x) min_x = x;
                if (x > max_x) max_x = x;
                if (y < min_y) min_y = y;
                if (y > max_y) max_y = y;
            }
        }
    }
    
    int piece_width = (max_x - min_x + 1) * 2;
    int piece_height = max_y - min_y + 1;
    int offset_x = (8 - piece_width) / 2;
    int offset_y = (4 - piece_height) / 2;
    
    /* Draw the piece centered in the box */
    for (int y = 0; y < TETRO_MATRIX_SIZE; y++) {
        for (int x = 0; x < TETRO_MATRIX_SIZE; x++) {
            if (shape[y][x]) {
                int draw_x = box_x + 1 + offset_x + (x - min_x) * 2;
                int draw_y = box_y + 1 + offset_y + (y - min_y);
                draw_cell(draw_x, draw_y, color_pair, 1);
            }
        }
    }
}

void renderer_draw_score(int score, int level, int lines)
{
    if (!renderer_initialized) {
        return;
    }
    
    int start_x = SIDEBAR_X;
    int start_y = 10;
    
    /* SCORE */
    mvprintw(start_y, start_x, "SCORE");
    mvprintw(start_y + 1, start_x, "%5d", score);
    
    /* LEVEL */
    mvprintw(start_y + 3, start_x, "LEVEL");
    mvprintw(start_y + 4, start_x, "%2d", level);
    
    /* LINES */
    mvprintw(start_y + 6, start_x, "LINES");
    mvprintw(start_y + 7, start_x, "%3d", lines);
}

void renderer_draw_controls(void)
{
    if (!renderer_initialized) {
        return;
    }
    
    int start_x = SIDEBAR_X;
    int start_y = 19;
    
    mvprintw(start_y, start_x, "CONTROLS");
    mvprintw(start_y + 1, start_x, "←→↓  Move");
    mvprintw(start_y + 2, start_x, "↑    Rotate");
    mvprintw(start_y + 3, start_x, "Space Drop");
    mvprintw(start_y + 4, start_x, "Z    Rotate↺");
    mvprintw(start_y + 5, start_x, "P    Pause");
    mvprintw(start_y + 6, start_x, "Q    Quit");
}

void renderer_draw_sidebar(const GameState *game)
{
    if (!renderer_initialized || game == NULL) {
        return;
    }
    
    renderer_draw_next_piece(game->next.type);
    renderer_draw_score(game->score, game->level, game->lines);
    renderer_draw_controls();
}

void renderer_draw_game(const GameState *game)
{
    if (!renderer_initialized || game == NULL) {
        return;
    }
    
    /* Clear screen only on full redraw - for now just redraw everything */
    clear();
    
    /* Draw main components */
    renderer_draw_board(&game->board, &game->current);
    renderer_draw_sidebar(game);
    
    /* Draw overlays if needed */
    if (game->is_paused) {
        renderer_draw_pause();
    } else if (!game->is_running) {
        renderer_draw_game_over(game->score);
    }
    
    /* Refresh screen */
    refresh();
}

void renderer_draw_pause(void)
{
    if (!renderer_initialized) {
        return;
    }
    
    int center_x = BOARD_DISPLAY_X + BOARD_WIDTH_CHARS / 2 - 4;
    int center_y = BOARD_DISPLAY_Y + BOARD_HEIGHT / 2;
    
    /* Draw overlay box */
    attron(A_REVERSE);
    mvprintw(center_y - 1, center_x, "          ");
    mvprintw(center_y, center_x, "  PAUSED  ");
    mvprintw(center_y + 1, center_x, "          ");
    attroff(A_REVERSE);
}

void renderer_draw_game_over(int score)
{
    if (!renderer_initialized) {
        return;
    }
    
    int center_x = BOARD_DISPLAY_X + BOARD_WIDTH_CHARS / 2 - 6;
    int center_y = BOARD_DISPLAY_Y + BOARD_HEIGHT / 2 - 1;
    
    /* Draw overlay box with game over message */
    attron(A_REVERSE);
    mvprintw(center_y - 1, center_x, "              ");
    mvprintw(center_y, center_x, "  GAME OVER   ");
    mvprintw(center_y + 1, center_x, "  Score: %5d", score);
    mvprintw(center_y + 2, center_x, "              ");
    attroff(A_REVERSE);
}
