#include <iostream>
#include <vector>
#include <random>
#include <deque>
#include <ncurses.h>
#include <cstdlib>

// Randomness
std::random_device rd;
std::mt19937 gen(rd());
std::uniform_int_distribution<> dist(0, 9);

class Board {
    protected:
        int board_size = 10;
        int apple_counter = 0;
        const char apple = 'o';
        const char empty = '.';

    private:
        std::vector<std::vector<char>> board;

    public:
    // Constructor
        Board() : board(board_size, std::vector<char>(board_size, empty)) {}

        // Spawns apples
        void apples() {
            while (apple_counter < 3) {
                int pos1 = dist(gen);
                int pos2 = dist(gen);

                if (board[pos1][pos2] != empty) {continue;}

                board[pos1][pos2] = apple;
                apple_counter++;
            }
        }

        void print_board() {
            clear();
            for (int i = 0; i < board_size; i++) {
                for (int j = 0; j < board_size; j++) {
                    mvprintw(i, j * 2, "%c ", board[i][j]);
                }
            }
            refresh();
        }

        class Snake {
            private:
                char previous = ' ';

            protected:
                const char snake_char = '#';
                int score = 0;
                std::deque<std::pair<int,int>> body;

            public:
                Board& outer;
                Snake(Board& b) : outer(b) {}

                // Spawns snake
                void spawn() {
                    int middle = outer.board_size/2;
                    body.push_front({middle, middle});
                    outer.board[middle][middle] = snake_char;
                }

                // Movement and validations for the snake
                // First it checks if the spot in front of the snake is good and then it places it
                void move() {
                    int kp = getch();
                    if (kp == ERR) {kp = previous;}

                    int headY = body.front().first;
                    int headX = body.front().second;
                    int newX = headX, newY = headY;

                    // Keybinds
                    switch (kp) {
                        case KEY_UP: case 'w': previous = 'w'; newY--; break;
                        case KEY_DOWN: case 's': previous = 's'; newY++; break;
                        case KEY_RIGHT: case 'd': previous = 'd'; newX++; break;
                        case KEY_LEFT: case 'a': previous = 'a'; newX--; break;
                        default: return;
                    }
                    // Out of bounds check
                    if (newX > outer.board_size - 1 || newX < 0 || newY > outer.board_size - 1 || newY < 0) {
                        end();
                    }

                    // Ate apple
                    bool ate_apple = (outer.board[newY][newX] == outer.apple);
                    if (outer.board[newY][newX] == snake_char) end();

                    // Moves the snake forward
                    body.push_front({newY, newX});
                    outer.board[newY][newX] = snake_char;

                    if (ate_apple) {
                        outer.apple_counter--;
                        score++;
                        outer.apples();
                    }
                    // Adds to the tail
                    else {
                        std::pair<int,int> tail = body.back();
                        outer.board[tail.first][tail.second] = outer.empty;
                        body.pop_back();
                    }
                }

                // End of the game
                void end() {
                    endwin();
                    std::cout << "\nGAME OVER\n";
                    std::cout << "Score: " << score << "\n";
                    std::cout << "Max possible score: " << outer.board_size * outer.board_size << "\n";
                    exit(0);
                }
        };
};

int main() {
    // ncruses initialisation
    initscr();
    noecho();
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);
    curs_set(0);
    timeout(500); // Set the time to your liking (its in ms)

    Board board;
    Board::Snake snake(board);

    // Initial spawn of obects
    snake.spawn();
    board.apples();
    // Game loop
    while (1) {
        board.print_board();
        snake.move();
    }
    return 0;
}
