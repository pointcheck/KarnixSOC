#include <stdio.h>
#include <stdlib.h> // for rand()
#include "tetris.h"
#include "cga.h"

const int tetrominoes[7][16] = {
	{0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0},  // I
	{0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0},  // O
	{0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 0, 0, 0, 0, 0},  // S
	{0, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0},  // Z
	{0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0},  // T
	{0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0},  // L
	{0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 1, 0, 0, 0, 0}   // J
};

char arena[A_HEIGHT][A_WIDTH];

int score = 0;
int gameOver = 0;
int currTetrominoIdx;
int currRotation = 0;
int currX = A_WIDTH / 2;
int currY = 0;

void newTetromino() {
	currTetrominoIdx = rand() % 7;
	currRotation = 0;
	currX = (A_WIDTH / 2) - (T_WIDTH / 2);
	currY = 0;
	gameOver = !validPos(currTetrominoIdx, currRotation, currX, currY);
}


void newGame() {
	score = 0;
	gameOver = 0;
	currRotation = 0;
	newTetromino();
}

int validPos(int tetromino, int rotation, int posX, int posY) {
	for (int x = 0; x < T_WIDTH; x++) {
		for (int y = 0; y < T_HEIGHT; y++) {
			int index = rotate(x, y, rotation);
			if (1 != tetrominoes[tetromino][index]) {
				continue;
			}

			int arenaX = x + posX;
			int arenaY = y + posY;
			if (0 > arenaX || A_WIDTH <= arenaX || A_HEIGHT <= arenaY) {
				return 0;
			}

			int arenaXY = arena[arenaY][arenaX];
			if (0 <= arenaY && 1 == arenaXY) {
				return 0;
			}
		}
	}
	return 1;
}

int rotate(int x, int y, int rotation) {
	switch (rotation % 4) {
	case 0:
		return x + y * T_WIDTH;
	case 1:
		return 12 + y - (x * T_WIDTH);
	case 2:
		return 15 - (y * T_WIDTH) - x;
	case 3:
		return 3 - y + (x * T_WIDTH);
	default:
		return 0;
	}
}

void processInputs(int event) {
		switch (event) {
		case TETRIS_EVENT_UP: {  //Rotate Tetromino 
		int nextRotation = (currRotation + 1) % 4;
		if (validPos(currTetrominoIdx, nextRotation, currX, currY)) {
			currRotation = nextRotation;
		}
	} break;
		case TETRIS_EVENT_LEFT:  // Move Left
			if (validPos(currTetrominoIdx, currRotation, currX - 1, currY)) {
				currX--;
			}
			break;
		case TETRIS_EVENT_RIGHT:  // Move Right
			if (validPos(currTetrominoIdx, currRotation, currX + 1, currY)) {
				currX++;
			}
			break;
		case TETRIS_EVENT_DOWN:  // Move Down 
			if (validPos(currTetrominoIdx, currRotation, currX, currY + 1)) {
				currY++;
			}
			break;
		}
}

int moveDown() {
	if (validPos(currTetrominoIdx, currRotation, currX, currY + 1)) {
		currY++;
		return 1;
	}
	return 0;
}

void addToArena() {
	for (int y = 0; y < T_HEIGHT; y++) {
		for (int x = 0; x < T_WIDTH; x++) {
			int index = rotate(x, y, currRotation);
			if (1 != tetrominoes[currTetrominoIdx][index]) {
				continue;
			}

			int arenaX = currX + x;
			int arenaY = currY + y;
			int xInRange = (0 <= arenaX) && (arenaX < A_WIDTH);
			int yInRange = (0 <= arenaY) && (arenaY < A_HEIGHT);
			if (xInRange && yInRange) {
				arena[arenaY][arenaX] = 1;
			}
		}
	}
}

void checkLines() {
	int clearedLines = 0;

	for (int y = A_HEIGHT - 1; y >= 0; y--) {
		int lineFull = 1;
		for (int x = 0; x < A_WIDTH; x++) {
			if (0 == arena[y][x]) {
				lineFull = 0;
				break;
			}
		}

		if (!lineFull) {
			continue;
		}

		clearedLines++;
		for (int yy = y; yy > 0; yy--) {
			for (int xx = 0; xx < A_WIDTH; xx++) {
				arena[yy][xx] = arena[yy - 1][xx];
			}
		}

		for (int xx = 0; xx < A_WIDTH; xx++) {
			arena[0][xx] = 0;
		}
		y++;
	}

	if (0 < clearedLines) {
		score += 100 * clearedLines;
	}
}

void drawArena() {
	char buffer[1024];
	int bufferIndex = 0;

	snprintf(buffer, 512, "\t\t\tScore: %d", score);
	cga_text_print(CGA->FB, 0, 0, 15, 0, buffer);
	cga_set_cursor_xy(40, 0);

	for (int y = 0; y < A_HEIGHT + 1; y++) {
		buffer[bufferIndex++] = '\t';
		buffer[bufferIndex++] = '\t';
		buffer[bufferIndex++] = '\t';
		buffer[bufferIndex++] = 0x1b;
		buffer[bufferIndex++] = 'F';
		buffer[bufferIndex++] = '5';
		buffer[bufferIndex++] = ';';
		buffer[bufferIndex++] = 0x92;
		buffer[bufferIndex++] = 0x1b;
		buffer[bufferIndex++] = 'F';
		buffer[bufferIndex++] = '1';
		buffer[bufferIndex++] = '5';
		buffer[bufferIndex++] = ';';

		for (int x = 0; x < A_WIDTH; x++) {
		if(y == A_HEIGHT) {
			buffer[bufferIndex++] = 0x1b;
			buffer[bufferIndex++] = 'F';
			buffer[bufferIndex++] = '5';
			buffer[bufferIndex++] = ';';
			buffer[bufferIndex++] = 0x92;
			continue;
		}

			int rotatedPos = rotate(x - currX, y - currY, currRotation);
			int validX = x >= currX && x < currX + T_WIDTH;
			int validY = y >= currY && y < currY + T_HEIGHT;
			int xyFilled = 1 == tetrominoes[currTetrominoIdx][rotatedPos];

			if (1 == arena[y][x] || (validX && validY && xyFilled)) {
				buffer[bufferIndex++] = 0x91;
			} else {
				buffer[bufferIndex++] = ' ';
			}
		}

		buffer[bufferIndex++] = 0x1b;
		buffer[bufferIndex++] = 'F';
		buffer[bufferIndex++] = '5';
		buffer[bufferIndex++] = ';';
		buffer[bufferIndex++] = 0x92;
		buffer[bufferIndex++] = '\r';
		buffer[bufferIndex++] = '\n';
	}

	buffer[bufferIndex] = '\0';

	cga_text_print(CGA->FB, 0, 2, 15, 0, buffer);

}


