#pragma once

#include <SFML/Graphics.hpp>
#include "State.h"
#include "GameLoop.h"
#include "SendingSocket.h"

namespace Marius
{
	class MainMenuState : public State
	{
	public:
		MainMenuState(GameDataRef data);

		void Init();

		void HandleInput();
		void Update(float dt);
		void Draw(float dt);

	private:
		GameDataRef _data;

		sf::Sprite _background;

		sf::Sprite _playButton;

		sf::Sprite _loadingButton;

		sf::Sprite _title;

		sf::Sprite _nameBox;

		bool _playerReady;

		sf::Text _insertText;
		std::string _rawText;
		sf::Font _font;
	};
}