#pragma once

#include <SFML/Graphics.hpp>
#include "State.h"
#include "GameLoop.h"
#include "SendingSocket.h"
#include <vector>

namespace Marius
{
	class GameState : public State
	{
	public:
		GameState(GameDataRef data);

		void Init();

		void HandleInput();
		void Update(float dt);
		void Draw(float dt);

	private:
		void InitGridPieces();

		void CheckAndPlacePiece();
		void CheckAndPlacePieceE();

		void CheckHasPlayerWon(int turn);
		void Check3PiecesForMatch(int x1, int y1, int x2, int y2, int x3, int y3, int pieceToCheck);

		GameDataRef _data;

		sf::Sprite _background;

		sf::Sprite _grid;

		sf::Sprite _gridPieces[3][3];

		sf::Sprite _chatBox;

		sf::Sprite _closeButton;

		int gridArray[3][3];

		int turn;
		int gameState;

		std::vector<sf::Text> _chatText;
		sf::Text _insertText;
		std::string _rawText;
		sf::Font _font;
		int _lineFocus;
	};
}