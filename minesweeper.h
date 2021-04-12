// Launches the minesweeper game
void minesweeper();

// Struct that is used for storing grid coordinates
typedef struct gridlocation
{
    short xpos;
    short ypos;
} gridlocation;

// Gets a random integer between a range of [int, int] (lower and upper bound)
int getRandomNumber(int, int);

// Check if any two gridlocation structs are the same
int checkGridCoordinatesSame(gridlocation, gridlocation);

// Print the Instructions and Controls at the bottom of the screen
void printInstructions(int, int);

// Print Minesweeper logo
void printMinesweeperLogo(int, int);

// Print the game grid
void printGameGrid(int, int, char(*)[10]);

// Check if a specific character is at the cursor's position
int checkIfCharAtCursor(char(*)[10], gridlocation, char);

// Return the number of bombs as a char that are adjacent to this uncovered space
char getAdjacentBombNumber(gridlocation, gridlocation[]);

// Acts to consolidate loops for the getAdjacentBombNumber() function
int adjacentBombLoopHelper(int, int, gridlocation, gridlocation[]);

// Uncoveres all the "adjacent" safe spaces if a completely safe space with no marker was uncovered
void uncoverAdjacentSafeSpaces(char(*)[10], gridlocation, gridlocation[]);