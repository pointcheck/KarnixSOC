#define A_WIDTH  10  // Arena width
#define A_HEIGHT 20  // Arena height
#define T_WIDTH  4   // Tetromino width
#define T_HEIGHT 4   // Tetromino height

enum {
	TETRIS_EVENT_RIGHT = 0,
	TETRIS_EVENT_LEFT,
	TETRIS_EVENT_UP,
	TETRIS_EVENT_DOWN,
};

extern char arena[A_HEIGHT][A_WIDTH];

extern int score;
extern int gameOver;
extern int currTetrominoIdx;
extern int currRotation;
extern int currX;
extern int currY; 

// Start new game
void newGame();

// Generate a new tetromino
void newTetromino();

// Determine if a position is valid
int validPos(int tetromino, int rotation, int posX, int posY);

// Rotate tetromino
int rotate(int x, int y, int rotation);

// Process keyboard inputs
void processInputs();

// Move tetromino down
int moveDown();

// Add tetromino to arena
void addToArena();

// Check if lines can be cleared
void checkLines();

// Draw arena
void drawArena();
