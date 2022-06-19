#include <iostream>

std::chrono::steady_clock::time_point start;
int RUNNING = 0;
int FINISHED = 1;

int getGameStatus(TileMap *tmap, int playerX, int playerY)
{
  if (playerX < 0 || playerX >= tmap->getWidth() || playerY < 0 || playerY >= tmap->getHeight())
  {
    return FINISHED;
  }

  if (tmap->getTile(playerX, playerY) == 1)
  {
    return FINISHED;
  }

  return RUNNING;
}

int getElapsedTimeInSeconds()
{
  return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start).count() / 1000;
}

void startGame()
{
  start = std::chrono::steady_clock::now();
}