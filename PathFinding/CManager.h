#pragma once
#include "SFML/Graphics.hpp"
#include "CTile.h"
#include "CButton.h"
#include <vector>

class CManager
{
public:

	sf::RenderWindow* window = nullptr;

	sf::Font font;


	std::vector<sf::Drawable*> ToDrawList;
	std::vector<CButton*> Buttons;


	CTile* start = nullptr;
	CTile* end = nullptr;

	std::vector <sf::VertexArray*> lines;


	sf::Clock clock;
	float step = (1.0f / 60.0f);
	int currentStep = 0;

	bool startFinding = false;
	bool foundRoute = false;
	bool drawnRoute = false;

	bool frozenClick = false;

	CTile* tiles[40][40];
};

