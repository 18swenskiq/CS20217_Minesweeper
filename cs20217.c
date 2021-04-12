#include <cs20217.h>
#include <stdlib.h>
#include <minesweeper.c>

static char name[] = "Minesweeper (cs20217)";

void cs20217_game()
{
  minesweeper();
}

char *cs20217_name()
{
  return name;
}