#include <stdlib.h>
#include <curses.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <minesweeper.h>

void minesweeper() {
    GameStart:
    // This seeds the random number generator that we need to determine the bomb positions later
    srand(time(NULL));

    // ------------------------- Set up Curses -------------------------
    initscr();
    short row,col;
	getmaxyx(stdscr,row,col);				

    // TODO: Finish Win Condition
    // TODO: Clean up function at bottom
    // Fix surrounding count bug if possbile

    // ------------------------- Minesweeper Game -------------------------

    // Fill in the board with randomly placed bombs (10 bombs for a 9x9 board)
    gridlocation bomblocations[10];
    for (int i = 0; i < 10; i++) {
        bomblocations[i].xpos = getRandomNumber(0, 8);
        bomblocations[i].ypos = getRandomNumber(0, 8);
    }

    // Let's make sure we have no duplicate bomb positions
    checkAndSetEqualityLoop:
        for (int i = 0; i < 10; i++) {
            for (int j = 0; j < 10; j++) {
                if (i == j) continue;
                if (checkGridCoordinatesSame(bomblocations[i], bomblocations[j])) {
                    bomblocations[i].xpos = getRandomNumber(0, 8);
                    bomblocations[i].ypos = getRandomNumber(0, 8);
                    // Let's just start the loop over if these are the same
                    goto checkAndSetEqualityLoop;
                }
            }
        }
    
    // Initialize our player cursor
    gridlocation gameCursorPosition;
    gameCursorPosition.xpos = 0;
    gameCursorPosition.ypos = 0;

    // Create all the board's lines
    char linesInformation[9][10];
    for(int i = 0; i != 9; i++) {
        strcpy(linesInformation[i], "?????????");
    }
    
    // Main Game Logic Loop
    int uncovered;
    short gameOn = 1;
    while(gameOn) {
        refresh();
        printMinesweeperLogo(row, col);
        printInstructions(row, col);

        // Print our board with the cursor position
        char cursorMovement[9][10];
        for(int i = 0; i < 9; i++) {
            strcpy(cursorMovement[i], linesInformation[i]);
        }
        cursorMovement[gameCursorPosition.ypos][gameCursorPosition.xpos] = 'C';
        printGameGrid(row, col, cursorMovement);

        // Listen for Cursor Movement, Flag, or Check
        int ch;
        ch = getch();
        switch(ch) {
            //Restart
            case 'r':
                    goto GameStart;
            // Move Right
            case 'd': 
                    if(gameCursorPosition.xpos != 8) gameCursorPosition.xpos += 1;
                    break;
            // Move Left
            case 'a': 
                    if(gameCursorPosition.xpos != 0) gameCursorPosition.xpos -= 1;
                    break;
            // Move Down
            case 's': 
                    if(gameCursorPosition.ypos != 8) gameCursorPosition.ypos += 1;
                    break;
            // Move Up
            case 'w': 
                    if(gameCursorPosition.ypos != 0) gameCursorPosition.ypos -= 1;
                    break;
            // Place a flag or remove a flag from the cursor's position
            case 'f':
                    // Place Flag
                    if(checkIfCharAtCursor(linesInformation, gameCursorPosition, '?')) {
                        linesInformation[gameCursorPosition.ypos][gameCursorPosition.xpos] = 'F';
                    }
                    // Remove Flag
                    else if(checkIfCharAtCursor(linesInformation, gameCursorPosition, 'F')) {
                        linesInformation[gameCursorPosition.ypos][gameCursorPosition.xpos] = '?';
                    }
                    break;
            // Uncover the space the cursor is at
            case ' ':
                    // Iterate over all the bomb locations so we can see if we're on one
                    for(int j = 0; j < 10; j++) {
                        // If this is activated, the player has lost
                        if(bomblocations[j].xpos == gameCursorPosition.xpos && bomblocations[j].ypos == gameCursorPosition.ypos) {
                            mvprintw(row/2 + 0,(col-32)/4,"%s", "Sorry, you hit a bomb and lost!");
                            mvprintw(row/2 + 1,(col-50)/4,"%s", "Press any two keys to exit, or press R to restart");
                            // TODO: Uncover all the bombs on the board?
                            char nextch = getch();
                            if(nextch == 'r') goto GameStart;
                            getch();
                            gameOn = 0;
                        }
                    }
                    // If we've made it here, that means we haven't hit a bomb
                    char adjacentNum = getAdjacentBombNumber(gameCursorPosition, bomblocations);
                    if(adjacentNum == '0') {
                        linesInformation[gameCursorPosition.ypos][gameCursorPosition.xpos] = ' ';
                        uncoverAdjacentSafeSpaces(linesInformation, gameCursorPosition, bomblocations);
                    }
                    else {
                        linesInformation[gameCursorPosition.ypos][gameCursorPosition.xpos] = adjacentNum;
                    }
                    break;
            default:
                    break;
        }
    }
    endwin();
}

int getRandomNumber(int lowerBound, int upperBound) {
    int num = (rand() % (upperBound - lowerBound + 1)) + lowerBound;
    return num;
}

int checkGridCoordinatesSame(gridlocation coordinate1, gridlocation coordinate2) {
    if(coordinate1.xpos == coordinate2.xpos && coordinate1.ypos == coordinate2.ypos) {
        return 1;
    }
    return 0;
}

void printInstructions(int row, int col) {
    mvprintw(row/2 + 8,(col-12)/2,"%s", "Instructions");
    mvprintw(row/2 + 9,(col-73)/2,"%s", "Story: You are 'C' and your mission is to mark all the mines in a field");
    mvprintw(row/2 + 10,(col-50)/2,"%s", "Objective: Uncover every tile except for the bomb!");
    mvprintw(row/2 + 12,(col-8)/2,"%s", "Controls");
    mvprintw(row/2 + 13,(col-36)/2,"%s", "WASD - Move Up, Left, Down, or Right");
    mvprintw(row/2 + 14,(col-21)/2,"%s", "R - Restart the Game");
    mvprintw(row/2 + 15,(col-72)/2,"%s", "F - Place or Remove flag (Just used to show where you think the bomb is)");
    mvprintw(row/2 + 16,(col-41)/2,"%s", "Space - Uncover the Tile below the cursor");
    mvprintw(row/2 + 18,(col-105)/2,"%s", "Disclaimer - There's a bug with it tracking the bombs surrounding the tile. Most tiles work but not some");
}

void printMinesweeperLogo(int row, int col) {
    mvprintw(row/2 - 14,(col-17)/2,"%s", "Quinton Swenski's");
    mvprintw(row/2 - 13,(col-73)/2,"%s", "#     #                                                                  ");
    mvprintw(row/2 - 12,(col-73)/2,"%s", "##   ## # #    # ######  ####  #    # ###### ###### #####  ###### #####  ");
    mvprintw(row/2 - 11,(col-73)/2,"%s", "# # # # # ##   # #      #      #    # #      #      #    # #      #    # ");
    mvprintw(row/2 - 10,(col-73)/2,"%s", "#  #  # # # #  # #####   ####  #    # #####  #####  #    # #####  #    # ");
    mvprintw(row/2 - 9 ,(col-73)/2,"%s", "#     # # #  # # #           # # ## # #      #      #####  #      #####  ");
    mvprintw(row/2 - 8 ,(col-73)/2,"%s", "#     # # #   ## #      #    # ##  ## #      #      #      #      #   #  ");
    mvprintw(row/2 - 7 ,(col-73)/2,"%s", "#     # # #    # ######  ####  #    # ###### ###### #      ###### #    # ");
}

void printGameGrid(int row, int col, char linesInformation[9][10]) {
    mvprintw(row/2 - 5,(col-strlen("+---------+"))/2,"%s", "+---------+");
    for(int i = -4; i < 6; i++) {
        mvprintw(row/2 + i,(col-9)/2,"%s", linesInformation[i + 4]);
    }
    mvprintw(row/2 + 5,(col-strlen("+---------+"))/2,"%s", "+---------+");
}

int checkIfCharAtCursor(char linesInformation[9][10], gridlocation gameCursorPosition, char inputChar) {
    if(linesInformation[gameCursorPosition.ypos][gameCursorPosition.xpos] == inputChar) {
        return 1;
    }
    else {
        return 0;
    }
}

char getAdjacentBombNumber(gridlocation gameCursorPosition, gridlocation bomblocations[10]) {
    int runningTotal = 0;

    // Iterates through all the bomb positions to count the amount of bombs adjacent to the uncovered tile
    for(int i = -1; i < 2; i++) {
        for(int j = -1; j < 2; j++) {
            if(i == 0 && j == 0) continue;
            runningTotal = runningTotal + adjacentBombLoopHelper(i, j, gameCursorPosition, bomblocations);
        }
    }
    char returnChar = runningTotal + '0';
    return returnChar;
}

int adjacentBombLoopHelper(int xPosMod, int yPosMod, gridlocation gameCursorPosition, gridlocation bomblocations[10]) {
    for(int i = 0; i < 10; i++) {
        if(((gameCursorPosition.xpos + xPosMod) < 0) || ((gameCursorPosition.ypos + yPosMod) > 8)) continue;
        if((bomblocations[i].xpos == (gameCursorPosition.xpos + xPosMod)) && (bomblocations[i].ypos == (gameCursorPosition.ypos + yPosMod))) {
            return 1;
        }
    }
    return 0;
}

void uncoverAdjacentSafeSpaces(char linesInformation[9][10], gridlocation currentBox, gridlocation bomblocations[10]) {

    // Will clean this up if I can get the bomb checking code to work properly

    //Check Left
    // If the box is on the left wall, don't need to check for an open space
    if(currentBox.xpos > 0){
        // We only need to try to 'unmask' the tile if its a question mark
        if(linesInformation[currentBox.ypos][currentBox.xpos - 1] == '?') {
            // If its a bomb, don't unmask it, just leave
            if(!adjacentBombLoopHelper(-1, 0, currentBox, bomblocations)) {
                gridlocation nextBox;
                nextBox.ypos = currentBox.ypos;
                nextBox.xpos = currentBox.xpos - 1;
                // Set the tile to what it should be
                char bombs = getAdjacentBombNumber(nextBox, bomblocations);
                if(bombs == '0') {
                    linesInformation[nextBox.ypos][nextBox.xpos] = ' ';
                }
                else {
                    linesInformation[nextBox.ypos][nextBox.xpos] = bombs;
                }
                if(linesInformation[nextBox.ypos][nextBox.xpos] == ' ') {
                    uncoverAdjacentSafeSpaces(linesInformation, nextBox, bomblocations);
                }
            }
        }
    }
    //Check Up
    if(currentBox.ypos > 0){
        // We only need to try to 'unmask' the tile if its a question mark
        if(linesInformation[currentBox.ypos - 1][currentBox.xpos] == '?') {
            // If its a bomb, don't unmask it, just leave
            if(!adjacentBombLoopHelper(0, -1, currentBox, bomblocations)) {
                gridlocation nextBox;
                nextBox.ypos = currentBox.ypos - 1;
                nextBox.xpos = currentBox.xpos;
                // Set the tile to what it should be
                char bombs = getAdjacentBombNumber(nextBox, bomblocations);
                if(bombs == '0') {
                    linesInformation[nextBox.ypos][nextBox.xpos] = ' ';
                }
                else {
                    linesInformation[nextBox.ypos][nextBox.xpos] = bombs;
                }
                if(linesInformation[nextBox.ypos][nextBox.xpos] == ' ') {
                    uncoverAdjacentSafeSpaces(linesInformation, nextBox, bomblocations);
                }
            }
        }
    }
    //Check Right
    if(currentBox.xpos < 8){
        // We only need to try to 'unmask' the tile if its a question mark
        if(linesInformation[currentBox.ypos][currentBox.xpos + 1] == '?') {
            // If its a bomb, don't unmask it, just leave
            if(!adjacentBombLoopHelper(1, 0, currentBox, bomblocations)) {
                gridlocation nextBox;
                nextBox.ypos = currentBox.ypos;
                nextBox.xpos = currentBox.xpos + 1;
                // Set the tile to what it should be
                char bombs = getAdjacentBombNumber(nextBox, bomblocations);
                if(bombs == '0') {
                    linesInformation[nextBox.ypos][nextBox.xpos] = ' ';
                }
                else {
                    linesInformation[nextBox.ypos][nextBox.xpos] = bombs;
                }
                if(linesInformation[nextBox.ypos][nextBox.xpos] == ' ') {
                    uncoverAdjacentSafeSpaces(linesInformation, nextBox, bomblocations);
                }
            }
        }
    }
    //Check Down
    if(currentBox.ypos < 8){
        // We only need to try to 'unmask' the tile if its a question mark
        if(linesInformation[currentBox.ypos + 1][currentBox.xpos] == '?') {
            // If its a bomb, don't unmask it, just leave
            if(!adjacentBombLoopHelper(0, +1, currentBox, bomblocations)) {
                gridlocation nextBox;
                nextBox.ypos = currentBox.ypos + 1;
                nextBox.xpos = currentBox.xpos;
                // Set the tile to what it should be
                char bombs = getAdjacentBombNumber(nextBox, bomblocations);
                if(bombs == '0') {
                    linesInformation[nextBox.ypos][nextBox.xpos] = ' ';
                }
                else {
                    linesInformation[nextBox.ypos][nextBox.xpos] = bombs;
                }
                if(linesInformation[nextBox.ypos][nextBox.xpos] == ' ') {
                    uncoverAdjacentSafeSpaces(linesInformation, nextBox, bomblocations);
                }
            }
        } 
    }
}