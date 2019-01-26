#include "GameState.h"
#include "Def.h"
#include "GameOverState.h"
#include "SendingSocket.h"

#include <iostream>
#include <string>
#include <sstream>
#include <cctype>

namespace Marius
{
	GameState::GameState(GameDataRef data) : _data(data)
	{

	}

	void GameState::Init()
	{
		gameState = STATE_PLAYING;
		bool temp = false;
		if (this->_data->machine.getPlayerTurn(temp))
		{
			turn = PLAYER_PIECE;
			std::cout << "player turn" << std::endl;
		}

		this->_data->assets.LoadTexture("Chat Box", CHAT_BOX);
		this->_data->assets.LoadTexture("Game Background", GAME_BACKGROUND_FILEPATH);
		this->_data->assets.LoadTexture("Grid", GRID_SPRITE_FILEPATH);
		this->_data->assets.LoadTexture("X Piece", X_PIECE_FILEPATH);
		this->_data->assets.LoadTexture("O Piece", O_PIECE_FILEPATH);
		this->_data->assets.LoadTexture("O WinPiece", O_WIN_PIECE_SPRITE);
		this->_data->assets.LoadTexture("X WinPiece", X_WIN_PIECE_SPRITE);
		this->_data->assets.LoadFont("Chat Font", CHAT_FONT_FILEPATH);
		this->_data->assets.LoadTexture("Close Button", CLOSE_BUTTON);

		this->_chatText.emplace_back(sf::Text());
		this->_chatText.emplace_back(sf::Text());
		this->_chatText.emplace_back(sf::Text());
		this->_chatText.emplace_back(sf::Text());
		this->_chatText.emplace_back(sf::Text());
		this->_chatText.emplace_back(sf::Text());
		this->_chatText.emplace_back(sf::Text());
		this->_chatText.emplace_back(sf::Text());

		for (int x = 0; x < this->_chatText.size(); x += 1)
		{
			this->_chatText[x].setFont(this->_font);
			this->_chatText[x].setCharacterSize(20);
			this->_chatText[x].setFillColor(sf::Color::Yellow);
			this->_chatText[x].setString("");
		}

		this->_lineFocus = _chatText.size() - 1;

		this->_insertText.setFont(this->_font);
		this->_insertText.setCharacterSize(15);
		this->_insertText.setFillColor(sf::Color::Yellow);
		this->_insertText.setString("");
		this->_rawText = "";

		this->_background.setTexture(this->_data->assets.GetTexture("Game Background"));
		this->_chatBox.setTexture(this->_data->assets.GetTexture("Chat Box"));
		this->_grid.setTexture(this->_data->assets.GetTexture("Grid"));
		this->_font = this->_data->assets.GetFont("Chat Font");
		this->_closeButton.setTexture(this->_data->assets.GetTexture("Close Button"));

		this->_chatBox.setPosition((this->_data->window.getSize().x / 2) - (this->_chatBox.getGlobalBounds().width / 2), this->_chatBox.getGlobalBounds().height * 0.1);
		this->_grid.setPosition((this->_data->window.getSize().x / 2) - (this->_grid.getGlobalBounds().width / 2), this->_data->window.getSize().y - (this->_grid.getGlobalBounds().height * 1.1));
		this->_closeButton.setPosition(this->_data->window.getSize().x - this->_closeButton.getGlobalBounds().width, 0);
		
		this->_insertText.setPosition((this->_data->window.getSize().x / 2) - (this->_chatBox.getGlobalBounds().width / 2), this->_chatBox.getGlobalBounds().height * 0.97);
		

		for (int x = 0; x < 3; x++)
		{
			for (int y = 0; y < 3; y++)
			{
				gridArray[x][y] = EMPTY_PIECE;
			}
		}
		InitGridPieces();
	}

	void GameState::HandleInput()
	{
		sf::Event event;

		while (this->_data->window.pollEvent(event))
		{
			if (sf::Event::Closed == event.type)
			{
				this->_data->window.close();
			}
			if (sf::Event::MouseWheelMoved == event.type)
			{
				if (event.mouseWheel.delta > 0 && this->_lineFocus > 6)
				{
					this->_lineFocus--;
				}
				else if (event.mouseWheel.delta < 0 && this->_lineFocus < this->_chatText.size() - 1)
				{
					this->_lineFocus++;
				}

			}
			
			if (sf::Event::TextEntered == event.type)
			{
				if (event.text.unicode < 128 && std::isprint(event.text.unicode))
				{
					this->_rawText += static_cast<char>(event.text.unicode);
					this->_insertText.setString(this->_rawText);
				}
				if (event.text.unicode == 8)
				{
					this->_rawText.pop_back();
					this->_insertText.setString(this->_rawText);
				}
			}
			if (sf::Event::KeyPressed == event.type)
			{
				if (event.key.code == sf::Keyboard::Enter)
				{
					std::string temp = _data->sendingSocket.getPlayerName() + " : " + this->_insertText.getString();
					_data->sendingSocket.SendData(temp);

					sf::Text sfText;
					sfText.setFont(this->_font);
					sfText.setCharacterSize(20);
					sfText.setFillColor(sf::Color::Yellow);
					sfText.setString(temp);
					this->_chatText.emplace_back(sfText);
					this->_lineFocus = this->_chatText.size() - 1;

					this->_rawText.clear();
					this->_insertText.setString("");
				}
			}
			
			if (this->_data->input.IsSpriteClicked(this->_closeButton, sf::Mouse::Left, this->_data->window))
			{
				// Switch To GameOver State
				this->_data->machine.AddState(StateRef(new GameOverState(_data)), false);
			}
			if (this->_data->input.IsSpriteClicked(this->_grid, sf::Mouse::Left, this->_data->window))
			{

				if (STATE_PLAYING == gameState)
				{
					this->CheckAndPlacePiece();
				}
			}
		}
	}

	void GameState::Update(float dt)
	{
		if (_data->sendingSocket.NewData())
		{
			if (STATE_PLAYING == gameState)
			{
			this->CheckAndPlacePieceE();
			}
		}
		if (_data->sendingSocket.NewChatData())
		{
			std::string str;
			str = _data->sendingSocket.ReceiveDate(str);
			std::cout << ">>:" << str << std::endl;
		
			sf::Text sfText;
			
			sfText.setFont(this->_font);
			sfText.setCharacterSize(20);
			sfText.setFillColor(sf::Color::Yellow);
			sfText.setString(str);
			this->_chatText.emplace_back(sfText);
			this->_lineFocus = this->_chatText.size() - 1;
		}
	}

	void GameState::Draw(float dt)
	{
		this->_data->window.clear(sf::Color::Red);

		this->_data->window.draw(this->_background);
		this->_data->window.draw(this->_chatBox); 
		this->_data->window.draw(this->_grid);
		
		for (int x = 0; x < 3; x++)
		{
			for (int y = 0; y < 3; y++)
			{
				this->_data->window.draw(this->_gridPieces[x][y]);
			}
		}
		if (!this->_chatText.empty())
		{
			for (int x = 0; x < 7; x += 1)
			{
				this->_chatText[_lineFocus - (6 - x)].setPosition((this->_data->window.getSize().x / 2) - (this->_chatBox.getGlobalBounds().width / 2) +10, this->_chatBox.getPosition().y + (22 * x));
				this->_data->window.draw(this->_chatText[_lineFocus - (6 - x)]);
			}
		}
		this->_data->window.draw(this->_insertText);
		this->_data->window.draw(this->_closeButton);
		this->_data->window.display();
	}

	void GameState::InitGridPieces()
	{
		sf::Vector2u tempSpriteSize = this->_data->assets.GetTexture("X Piece").getSize();

		for (int x = 0; x < 3; x++)
		{
			for (int y = 0; y < 3; y++)
			{
				gridArray[x][y] = EMPTY_PIECE;
				_gridPieces[x][y].setTexture(this->_data->assets.GetTexture("X Piece"));
				_gridPieces[x][y].setPosition(_grid.getPosition().x + (tempSpriteSize.x * x) - 7, _grid.getPosition().y + (tempSpriteSize.y * y) - 7);
				_gridPieces[x][y].setColor(sf::Color(255, 255, 255, 0));
			}
		}
	}

	void GameState::CheckAndPlacePiece()
	{
		sf::Vector2i touchPoint = this->_data->input.GetMousePosition(this->_data->window);
		sf::FloatRect gridSize = _grid.getGlobalBounds();
		sf::Vector2f gapOutsideOfGrid = sf::Vector2f((SCREEN_WIDTH - gridSize.width) / 2, this->_data->window.getSize().y - (this->_grid.getGlobalBounds().height * 1.1));

		sf::Vector2f gridLocalTouchPos = sf::Vector2f(touchPoint.x - gapOutsideOfGrid.x, touchPoint.y - gapOutsideOfGrid.y);

		sf::Vector2f gridSectionSize = sf::Vector2f(gridSize.width / 3, gridSize.height / 3);

		int column, row;

		// Check which column the user has clicked
		if (gridLocalTouchPos.x < gridSectionSize.x) // First Column
		{
			column = 1;
		}
		else if (gridLocalTouchPos.x < gridSectionSize.x * 2) // Second Column
		{
			column = 2;
		}
		else if (gridLocalTouchPos.x < gridSize.width) // Third Column
		{
			column = 3;
		}

		// Check which row the user has clicked
		if (gridLocalTouchPos.y < gridSectionSize.y) // First Row
		{
			row = 1;
		}
		else if (gridLocalTouchPos.y < gridSectionSize.y * 2) // Second Row
		{
			row = 2;
		}
		else if (gridLocalTouchPos.y < gridSize.height) // Third Row
		{
			row = 3;
		}

		if (gridArray[column - 1][row - 1] == EMPTY_PIECE)
		{
			if (PLAYER_PIECE == turn)
			{
				gridArray[column - 1][row - 1] = turn;

				//send coordonates
				std::ostringstream ss;
				ss << "/0" << column << row;
				std::string data = ss.str();
				_data->sendingSocket.SendData(data);


				_gridPieces[column - 1][row - 1].setTexture(this->_data->assets.GetTexture("X Piece"));

				_gridPieces[column - 1][row - 1].setColor(sf::Color(0, 0, 255, 255));

				this->CheckHasPlayerWon(PLAYER_PIECE);

				turn = ENEMY_PIECE;
			}
		}

	}

	void GameState::CheckAndPlacePieceE()
	{
		
		if (ENEMY_PIECE == turn)
		{
			if (_data->sendingSocket.NewData())
			{
				int column, row;
				std::string temp;
				temp = _data->sendingSocket.ReceiveDate(temp);

				if (temp == "/011")
				{
					column = 1;
					row = 1;
				}
				else if (temp == "/012")
				{
					column = 1;
					row = 2;
				}
				else if (temp == "/013")
				{
					column = 1;
					row = 3;
				}
				else if (temp == "/021")
				{
					column = 2;
					row = 1;
				}
				else if (temp == "/022")
				{
					column = 2;
					row = 2;
				}
				else if (temp == "/023")
				{
					column = 2;
					row = 3;
				}
				else if (temp == "/031")
				{
					column = 3;
					row = 1;
				}
				else if (temp == "/032")
				{
					column = 3;
					row = 2;
				}
				else if (temp == "/033")
				{
					column = 3;
					row = 3;
				}

				gridArray[column - 1][row - 1] = ENEMY_PIECE;

				_gridPieces[column - 1][row - 1].setTexture(this->_data->assets.GetTexture("O Piece"));

				_gridPieces[column - 1][row - 1].setColor(sf::Color(255, 0, 0, 255));

				this->CheckHasPlayerWon(ENEMY_PIECE);

				turn = PLAYER_PIECE;
			}
		}
	}

	void GameState::CheckHasPlayerWon(int player)
	{
		Check3PiecesForMatch(0, 0, 1, 0, 2, 0, player);
		Check3PiecesForMatch(0, 1, 1, 1, 2, 1, player);
		Check3PiecesForMatch(0, 2, 1, 2, 2, 2, player);
		Check3PiecesForMatch(0, 0, 0, 1, 0, 2, player);
		Check3PiecesForMatch(1, 0, 1, 1, 1, 2, player);
		Check3PiecesForMatch(2, 0, 2, 1, 2, 2, player);
		Check3PiecesForMatch(0, 0, 1, 1, 2, 2, player);
		Check3PiecesForMatch(0, 2, 1, 1, 2, 0, player);

		int emptyNum = 9;

		for (int x = 0; x < 3; x++)
		{
			for (int y = 0; y < 3; y++)
			{
				if (EMPTY_PIECE != gridArray[x][y])
				{
					emptyNum--;
				}
			}
		}

		// check if the game is a draw
		if (0 == emptyNum && (STATE_WON != gameState) && (STATE_LOSE != gameState))
		{
			gameState = STATE_DRAW;
		}

		// check if the game is over
		if (STATE_DRAW == gameState || STATE_LOSE == gameState || STATE_WON == gameState)
		{
			// show game over
			std::cout << "game end" << std::endl;
		}

		std::cout << gameState << std::endl;
	}

	void GameState::Check3PiecesForMatch(int x1, int y1, int x2, int y2, int x3, int y3, int pieceToCheck)
	{
		
		if (pieceToCheck == gridArray[x1][y1] && pieceToCheck == gridArray[x2][y2] && pieceToCheck == gridArray[x3][y3])
		{
			std::string winningPieceStr;

			if (O_PIECE == pieceToCheck)
			{
				winningPieceStr = "O WinPiece";
			}
			else
			{
				winningPieceStr = "X WinPiece";
			}

			_gridPieces[x1][y1].setTexture(this->_data->assets.GetTexture(winningPieceStr));
			_gridPieces[x2][y2].setTexture(this->_data->assets.GetTexture(winningPieceStr));
			_gridPieces[x3][y3].setTexture(this->_data->assets.GetTexture(winningPieceStr));

			_gridPieces[x1][y1].setColor(sf::Color(255, 255, 255, 255));
			_gridPieces[x2][y2].setColor(sf::Color(255, 255, 255, 255));
			_gridPieces[x3][y3].setColor(sf::Color(255, 255, 255, 255));


			if (PLAYER_PIECE == pieceToCheck)
			{
				gameState = STATE_WON;
			}
			else
			{
				gameState = STATE_LOSE;
			}
		}
	}
}