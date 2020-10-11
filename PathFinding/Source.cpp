#include <vector>
#include <string>
#include <iostream>
#include <time.h>
#include <math.h>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include <Windows.h>

#include "CManager.h"
#include "CTile.h"



void CreateButton(void(*function)(), std::string _string, int _fontSize, sf::Color _tColour, sf::Text::Style _style, float _x, float _y, sf::Color _bgColour, float _padding);

int FixedUpdate();
void CheckButtonsPressed();
void Draw();

CManager manager;

void StartStopSearch() {
	manager.startFinding = !manager.startFinding;
	manager.Buttons[0]->text->setString(manager.startFinding ? "Stop" : "Start");
	manager.Buttons[0]->rect->setFillColor(manager.startFinding ? sf::Color::Red : sf::Color::Color(0, 150, 0));
	//manager.step = (manager.startFinding ? (1.0f / 5.0f) : (1.0f / 60.0f));
}

void ClearSearch() {
	for (int x = 0; x < 40; x++) {
		for (int y = 0; y < 40; y++) {
			if (manager.tiles[x][y] == nullptr) {
				CTile* myTile = new CTile(TileType::Empty, { (float)x * 20,(float)y * 20 });
				manager.tiles[x][y] = myTile;
			}
			else {
				if (manager.tiles[x][y]->type == TileType::Wall) continue;
				manager.tiles[x][y]->SetType(TileType::Empty);
			}
		}
	}

	manager.start = manager.tiles[10][20];
	manager.end = manager.tiles[30][20];

	manager.start->SetType(TileType::Start);
	manager.end->SetType(TileType::End);

	for (sf::VertexArray* _vert : manager.lines)
	{
		delete _vert;
	}
	manager.lines.clear();

	manager.foundRoute = false;
	manager.drawnRoute = false;
}

void ClearWalls() {
	for (int x = 0; x < 40; x++) {
		for (int y = 0; y < 40; y++) {
			if (manager.tiles[x][y]->type == TileType::Wall) manager.tiles[x][y]->SetType(TileType::Empty);
		}
	}
}

int main() {
	sf::RenderWindow window(sf::VideoMode(800, 800), "A* Pathfinding - By Keane Carotenuto");
	manager.window = &window;

	if (!manager.font.loadFromFile("Fonts/Roboto.ttf")) std::cout << "Failed to load Roboto\n";

	CreateButton(&StartStopSearch, "Start", 25, sf::Color::White, sf::Text::Style::Regular, 0, 0, sf::Color::Color(0,150,0), 5);
	CreateButton(&ClearSearch, "Clear Search", 25, sf::Color::White, sf::Text::Style::Regular, 0, 40, sf::Color::Red, 5);
	CreateButton(&ClearWalls, "Clear Walls", 25, sf::Color::White, sf::Text::Style::Regular, 0, 80, sf::Color::Red, 5);
	
	ClearSearch();

	float stepTime = 0;
	bool drawn = false;
	

	while (window.isOpen() == true)
	{
		stepTime += manager.clock.getElapsedTime().asSeconds();
		manager.clock.restart();

		while (stepTime >= manager.step)
		{
			//Main Loop of Game
			if (FixedUpdate() == 0) return 0;

			stepTime -= manager.step;
			drawn = false;
		}

		//Draws After Updates
		if (drawn)
		{
			//sf::sleep(sf::seconds(0.01f));
		}
		else
		{
			Draw();

			drawn = true;
		}

		CheckButtonsPressed();


		sf::Event newEvent;

		//Quit
		while (window.pollEvent(newEvent))
		{
			if (newEvent.type == sf::Event::Closed)
			{
				window.close();
			}
		}
	}

	return 0;
}

int FixedUpdate()
{
	manager.currentStep ++;
	manager.ToDrawList.clear();

	//Breadth First Search
	if (!manager.foundRoute && manager.startFinding && manager.currentStep % 5 == 0) {
		std::vector<CTile*> toUpdate;

		for (int x = 0; x < 40; x++) {
			for (int y = 0; y < 40; y++) {

				if (manager.tiles[x][y]->type == TileType::Start || manager.tiles[x][y]->type == TileType::EmptySearched) {

					int tempx = 0;
					int tempy = 0;
					for (int i = 0; i < 4; i++) {
						//Sleep(100);

						switch (i)
						{

						case 0:
							tempx = x + 1;
							tempy = y;
							break;

						case 1:
							tempx = x;
							tempy = y + 1;
							break;

						case 2:
							tempx = x - 1;
							tempy = y;
							break;

						case 3:
							tempx = x;
							tempy = y - 1;
							break;
						}

						if (tempx < 0 || tempx > 39 || tempy < 0 || tempy > 39 || manager.tiles[tempx][tempy]->type == TileType::EmptySearching) continue;

						if (manager.tiles[tempx][tempy]->type == TileType::Empty || manager.tiles[tempx][tempy]->type == TileType::End) {
							manager.tiles[x][y]->next.push_back(manager.tiles[tempx][tempy]);

							manager.tiles[tempx][tempy]->previous = manager.tiles[x][y];

							sf::VertexArray* lines = new sf::VertexArray(sf::LineStrip, 2);
							lines->operator[](0).position = sf::Vector2f(manager.tiles[x][y]->sprite->getPosition().x + 10, manager.tiles[x][y]->sprite->getPosition().y + 10);
							lines->operator[](0).color = sf::Color::Red;
							lines->operator[](1).position = sf::Vector2f(manager.tiles[tempx][tempy]->sprite->getPosition().x + 10, manager.tiles[tempx][tempy]->sprite->getPosition().y + 10);
							lines->operator[](1).color = sf::Color::Red;

							manager.lines.push_back(lines);

							if (manager.tiles[tempx][tempy]->type == TileType::End) {
								manager.foundRoute = true;
								goto update;
							}
							else {
								toUpdate.push_back(manager.tiles[tempx][tempy]);
								manager.tiles[tempx][tempy]->SetType(TileType::EmptySearching);
							}
						}
					}
				}
			}
		}

		update:

		for (CTile* _tile : toUpdate) {
			_tile->SetType(TileType::EmptySearched);
		}
	}
	

	

	if (sf::Mouse::isButtonPressed(sf::Mouse::Left) || sf::Mouse::isButtonPressed(sf::Mouse::Right))
	{
		for (int x = 0; x < 40; x++) {
			for (int y = 0; y < 40; y++) {
				if (manager.tiles[x][y]->type == TileType::Start || manager.tiles[x][y]->type == TileType::End) continue;

				sf::Vector2f mousePos = (sf::Vector2f)sf::Mouse::getPosition(*manager.window);

				if (manager.tiles[x][y]->sprite->getGlobalBounds().contains(mousePos)) {
					manager.tiles[x][y]->SetType(
						sf::Mouse::isButtonPressed(sf::Mouse::Left) &&
						manager.tiles[x][y]->type == TileType::Empty ?
						TileType::Wall : 
						(sf::Mouse::isButtonPressed(sf::Mouse::Right) &&
						manager.tiles[x][y]->type == TileType::Wall ? TileType::Empty : manager.tiles[x][y]->type));
				}
			}
		}
	}


	for (int x = 0; x < 40; x++) {
		for (int y = 0; y < 40; y++) {
			manager.ToDrawList.push_back(manager.tiles[x][y]);
		}
	}

	if (manager.foundRoute && !manager.drawnRoute) {
		manager.drawnRoute = true;

		CTile* currentTile = manager.end;

		while (currentTile->previous != nullptr) {

			sf::VertexArray* lines = new sf::VertexArray(sf::LineStrip, 2);
			lines->operator[](0).position = sf::Vector2f(currentTile->sprite->getPosition().x + 10, currentTile->sprite->getPosition().y + 10);
			lines->operator[](0).color = sf::Color::Green;
			lines->operator[](1).position = sf::Vector2f(currentTile->previous->sprite->getPosition().x + 10, currentTile->previous->sprite->getPosition().y + 10);
			lines->operator[](1).color = sf::Color::Green;

			manager.lines.push_back(lines);

			currentTile = currentTile->previous;
		}


	}

	for (sf::VertexArray* _line : manager.lines) {
		manager.ToDrawList.push_back(_line);
	}

	return 1;
}

void CheckButtonsPressed()
{
	if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
		
		if (!manager.frozenClick) {
			std::cout << "Click\n";
			for (CButton* _button : manager.Buttons)
			{

				//If click, do func
				if (_button->rect->getGlobalBounds().contains((sf::Vector2f)sf::Mouse::getPosition(*manager.window))) {
					std::cout << "ClickED\n";
					if (_button->function != nullptr) _button->function();
				}
			}
		}
		manager.frozenClick = true;
	}
	else {
		manager.frozenClick = false;
	}
}

void CreateButton(void(*function)(), std::string _string, int _fontSize, sf::Color _tColour, sf::Text::Style _style, float _x, float _y, sf::Color _bgColour, float _padding)
{
	sf::Text* tempText = new sf::Text;
	tempText->setString(_string);
	tempText->setCharacterSize(_fontSize);
	tempText->setFillColor(_tColour);
	tempText->setStyle(_style);
	tempText->setPosition(_x, _y);
	tempText->setFont(manager.font);

	sf::RectangleShape* buttonRect = new sf::RectangleShape;
	buttonRect->setPosition(tempText->getGlobalBounds().left - _padding, tempText->getGlobalBounds().top - _padding);
	buttonRect->setSize(sf::Vector2f(tempText->getGlobalBounds().width + (2 * _padding), tempText->getGlobalBounds().height + (2 * _padding)));
	buttonRect->setFillColor(_bgColour);

	CButton* button = new CButton(buttonRect, tempText, function);
	manager.Buttons.push_back(button);
}

void Draw()
{
	manager.window->clear();

	for (sf::Drawable* item : manager.ToDrawList) {
		manager.window->draw(*item);
	}

	for (CButton* button : manager.Buttons) {
		manager.window->draw(*button->rect);
		button->text->setFont(manager.font);
		manager.window->draw(*button->text);
	}

	manager.window->display();
}
