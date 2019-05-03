// ConsoleSnake.cpp : This file contains the 'main' function. Program execution begins and ends there.
// Game made following javidx9 youtube tutorial: www.youtube.com/watch?v=e8lYLYlrGLg
/*
	Finished Creating game based on tutorial
	Changed that need to be make:
		FIXED: fix input speed issue (not accurate input speed too fast or not responsive)
		FIXED: Allows for double tap
		+ Fix input lose that occurs occasionally
*/

#include "pch.h"
using namespace std;

const static int nScreenWidth = 30;
const static int nScreenHeight = 30;

struct sSnakeSegment {
	int x;
	int y;
};

int main() {
	// Create Screen Buffer
	wchar_t * screen = new wchar_t[nScreenWidth * nScreenHeight];
	HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	SetConsoleActiveScreenBuffer(hConsole);
	DWORD dwByteWritten = 0;

	INPUT_RECORD ev; // Keyboard input event
	HANDLE hstdin = GetStdHandle(STD_INPUT_HANDLE); // Get console input handle
	GetConsoleMode(hstdin, &dwByteWritten);

	// Start game loop
	bool bContinue = true;
	while (bContinue) {
		// Init Snake
		list<sSnakeSegment> snake = { {15, 15}, {16, 15}, {17, 15}, {18, 15}, {19, 15}, {20, 15} };
		int nSnakeDirection = 3;

		// Init Food
		int nFoodX = 1;
		int nFoodY = 8;

		// Game Logic Variables
		int nScore = 0;
		bool bDead = false;

		// Input Variables
		bool bKey[4] = {false};

		while (!bDead) {
			// Timing =====================================================
			/*auto t1 = chrono::system_clock::now();
			while ((chrono::system_clock::now() - t1) < ((nSnakeDirection % 2 == 1) ? 120ms : 200ms)) {
				// Input ======================================================
				bKey[0] = (0x8000 & GetAsyncKeyState((unsigned char)('\x25'))) != 0;
				bKey[1] = (0x8000 & GetAsyncKeyState((unsigned char)('\x27'))) != 0;

				// Hold left
				if (bKey[0] && !bKey[2]) {
					nSnakeDirection++;
					if (nSnakeDirection == 4)
						nSnakeDirection = 0;
				}

				// Hold Right
				if (bKey[1] && !bKey[3]) {
					nSnakeDirection--;
					if (nSnakeDirection == -1)
						nSnakeDirection = 3;
				}
				
				bKey[0] = bKey[2];
				bKey[1] = bKey[3];
			}*/
			// Timing =====================================================
			this_thread::sleep_for(150ms);
			
			// Input ======================================================
			DWORD count; // Ignore
			
			// If kbhit
			if (WaitForSingleObject(hstdin, 0) == WAIT_OBJECT_0 && GetNumberOfConsoleInputEvents(hstdin, &count)) {
				// Get input events
				ReadConsoleInput(hstdin, &ev, 2, &count);

				if (ev.EventType == KEY_EVENT && !ev.Event.KeyEvent.bKeyDown)
					switch (ev.Event.KeyEvent.wVirtualKeyCode) {
					case '\x25':
						nSnakeDirection++;
						if (nSnakeDirection == 4)
							nSnakeDirection = 0;
						break;
					case '\x27':
						nSnakeDirection--;
						if (nSnakeDirection == -1)
							nSnakeDirection = 3;
						break;
					}
			}
			 
			// Game Logic =================================================
			// Update Snake Position
			switch (nSnakeDirection) {
				case 0: // UP
					snake.push_front({ snake.front().x, snake.front().y - 1 });
					break;
				case 1: // RIGHT
					snake.push_front({ snake.front().x + 1, snake.front().y });
					break;
				case 2: // DOWN
					snake.push_front({ snake.front().x, snake.front().y + 1 });
					break;
				case 3: // LEFT
					snake.push_front({ snake.front().x - 1, snake.front().y });
					break;
			}

			// Collision Detection
			// World Detection (Boundary)
			if (snake.front().x < 0 || snake.front().x >= nScreenWidth)
				bDead = true;
			if (snake.front().y < 3 || snake.front().y >= nScreenHeight)
				bDead = true;

			// Food Detection
			if (snake.front().x == nFoodX && snake.front().y == nFoodY) {
				nScore++;
				while (screen[nFoodY * nScreenWidth + nFoodX] != ' ') {
					nFoodX = rand() % nScreenWidth;
					nFoodY = (rand() % (nScreenHeight - 3)) + 3;
				}

				// Grow Snake by 1
				snake.push_back({ snake.back().x, snake.back().y });
			}

			// Snake Detection
			for (list<sSnakeSegment>::iterator i = snake.begin(); i != snake.end(); i++)
				if (i != snake.begin() && i->x == snake.front().x && i->y == snake.front().y)
					bDead = true;

			// Get rid of tail
			snake.pop_back();

			// Display =====================================================
			// Clear Screen
			for (int i = 0; i < nScreenWidth * nScreenHeight; i++)
				screen[i] = L' ';
		
			// Draw Stats & Border
			for (int i = 0; i < nScreenWidth; i++) {
				screen[i] = L'=';
				screen[2 * nScreenWidth + i] = L'=';
			}
			wsprintf(&screen[nScreenWidth], L"  S N A K E      SCORE: %d", nScore);
		
			// Draw Snake Body
			for (auto s : snake)
				screen[s.y * nScreenWidth + s.x] = bDead ? L'+' : L'O';

			// Draw Snake Head
			screen[snake.front().y * nScreenWidth + snake.front().x] = bDead ? L'X' : L'@';

			// Draw Food
			screen[nFoodY * nScreenWidth + nFoodX] = L'F';

			// Draw Game Over
			if (bDead) {
				wsprintf(&screen[14 * nScreenWidth + 10], L"Game Over!");
				wsprintf(&screen[15 * nScreenWidth], L" PRESS 'SPACE' TO PLAY AGAIN");
			}
			// Display Frame
			WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth * nScreenHeight, { 0, 0 }, &dwByteWritten);
		}

		// Wait for space
		while ((0x8000 & GetAsyncKeyState((unsigned char)(' '))) == 0)
			bDead = false;
	}

	return 0;
}