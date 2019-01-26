#pragma once

#include "SFML\Graphics.hpp"

namespace Marius
{
	class InputManager
	{
	public:
		InputManager() {}
		~InputManager() {}

		bool IsSpriteClicked(sf::Sprite object, sf::Mouse::Button button, sf::RenderWindow &window);
		//bool IsEnterPressed(sf::String text ,sf::Keyboard::Key key);

		sf::Vector2i GetMousePosition(sf::RenderWindow &window);
	};
}