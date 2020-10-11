#pragma once
#include "SFML/Graphics.hpp"
#include <iostream>

enum class TileType
{
	Empty,
	EmptySearching,
	EmptySearched,
	Wall,
	Start,
	End
};

class CTile:
	public sf::Drawable
{
public:
	sf::Sprite* sprite;
	sf::Texture texture;
	TileType type;

	CTile* previous = nullptr;
	std::vector<CTile*> next;

	CTile(TileType _type, sf::Vector2f _pos);

	void SetType(TileType _type);

	virtual void draw(sf::RenderTarget& target, sf::RenderStates states)const;
};

