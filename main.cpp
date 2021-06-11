#include <SFML/Graphics.hpp>
#include <random>
#include <iostream>
#include "minesweeper.h"

#define WIDTH 800
#define HEIGHT 600

#define CELL_SIZE 40
#define ROWS 15
#define COLS 20
#define MINE_COUNT 20

using namespace std;
using namespace sf;

int main()
{

  RenderWindow window(VideoMode(WIDTH, HEIGHT), "Minesweeper");
  Font font;

  bool firstTime = true;

  Image icon;
  icon.loadFromFile("res/mine.png");
  Vector2u iconSize = icon.getSize();
  window.setIcon(iconSize.x, iconSize.y, icon.getPixelsPtr());

  if (!font.loadFromFile("res/mine-sweeper.ttf"))
  {
    std::cout << "Failed to load font!";
    exit(1);
  }

  Text gameOverText;

  gameOverText.setFont(font);
  gameOverText.setFillColor(Color::Blue);
  gameOverText.setString("Game Over");
  gameOverText.setCharacterSize(50);

  FloatRect textRect = gameOverText.getLocalBounds();
  gameOverText.setOrigin(textRect.width / 2, textRect.height / 2);
  gameOverText.setPosition(Vector2f(WIDTH / 2.0f, HEIGHT / 2.0f));

  Minesweeper minesweeper(ROWS, COLS, CELL_SIZE, MINE_COUNT, font);

  while (window.isOpen())
  {
    Event event;
    while (window.pollEvent(event))
    {
      if (event.type == Event::Closed)
        window.close();

      if (event.type == Event::MouseButtonPressed && !minesweeper.isGameOver())
      {
        Vector2i mousePos = Mouse::getPosition(window);
        int x = mousePos.x / CELL_SIZE;
        int y = mousePos.y / CELL_SIZE;

        if (firstTime)
        {
          minesweeper.addMines(minesweeper.getIndex(x, y));
          firstTime = false;
        }

        CellState cellState = minesweeper.getCellState(x, y);
        int cell = minesweeper.getCell(x, y);

        if (event.mouseButton.button == Mouse::Left)
          minesweeper.mouseLeftPressed(x, y, cell, cellState);

        if (event.mouseButton.button == Mouse::Right)
          minesweeper.mouseRightPressed(x, y, cell, cellState);
      }

      if (event.type == Event::Resized)
      {
        FloatRect visibleArea(0, 0, event.size.width, event.size.height);
        window.setView(View(visibleArea));
      }
    }

    window.clear(Color(169, 166, 171, 255));

    minesweeper.draw(window);

    if (minesweeper.isGameOver())
      window.draw(gameOverText);

    window.display();
  }

  return 0;
}