
#include <iostream>
#include <vector>
#include <thread>

using namespace std;

struct Coord
{
  int x;
  int y;
};

Coord getRandomCoord(TileMap *tmap)
{
  int x = rand() % tmap->getWidth();
  int y = rand() % tmap->getHeight();
  return {x, y};
}

bool hasFloor(TileMap *tmap)
{

  for (int i = 0; i < tmap->getWidth(); i++)
  {
    for (int j = 0; j < tmap->getHeight(); j++)
    {
      if (tmap->getTile(i, j) == 0)
      {
        return true;
      }
    }
  }

  return false;
}

vector<Coord> getNeighbors(Coord coord)
{
  Coord right = {coord.x + 1, coord.y};
  Coord left = {coord.x - 1, coord.y};
  Coord up = {coord.x, coord.y + 1};
  Coord down = {coord.x, coord.y - 1};

  return {right, left, up, down};
}

bool isValidCoord(Coord coord, TileMap *tmap)
{
  return coord.x >= 0 && coord.x < tmap->getWidth() && coord.y >= 0 && coord.y < tmap->getHeight();
}

void spreadLava(vector<Coord> *coordsParam, TileMap *tmap)
{
  vector<Coord> &coords = *coordsParam;
  vector<Coord> newCoords;

  /*
   * Main spread loop
   * Get all the lava coords and randomly define if the neighbor floor will
   * become lava or not.
   **/
  for (int i = 0; i < coords.size(); i++)
  {
    vector<Coord> neighbors = getNeighbors(coords[i]);

    for (int j = 0; j < neighbors.size(); j++)
    {
      if (isValidCoord(neighbors[j], tmap))
      {

        int r = rand() % 2;
        int isFloor = tmap->getTile(neighbors[j].x, neighbors[j].y) == 0;
        if (r == 1 && isFloor)
        {
          tmap->setTile(neighbors[j].x, neighbors[j].y, 1);
          newCoords.push_back(neighbors[j]);
        }
      }
    }
  }

  /*
   * Add the new coords to the coords array
   **/
  for (int i = 0; i < newCoords.size(); i++)
  {
    coords.push_back(newCoords[i]);
  }

  /*
   * If still has floors, spread lava again recursively
   **/
  if (hasFloor(tmap))
  {
    std::this_thread::sleep_for(
        std::chrono::milliseconds(1000));
    spreadLava(&coords, tmap);
  }
}

/*
 * This function is responsible for spreading the lava
 * It will start on 3 random coordinates and spread the lava
 * until there is no more lava to spread
 **/
void startSpreadingLava(TileMap *tmap)
{
  Coord c1 = {0, 0};
  tmap->setTile(c1.x, c1.y, 1);

  Coord c2 = {0, tmap->getHeight() - 1};
  tmap->setTile(c2.x, c2.y, 1);

  Coord c3 = {tmap->getWidth() - 1, 0};
  tmap->setTile(c3.x, c3.y, 1);

  Coord c4 = {tmap->getWidth() - 1, tmap->getHeight() - 1};
  tmap->setTile(c4.x, c4.y, 1);

  Coord c5 = getRandomCoord(tmap);
  tmap->setTile(c5.x, c5.y, 1);

  vector<Coord> coords;

  coords.push_back(c1);
  coords.push_back(c2);
  coords.push_back(c3);
  coords.push_back(c4);
  coords.push_back(c5);

  spreadLava(&coords, tmap);
}