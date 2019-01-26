#include <sstream>
#include "MainMenu.h"
#include "Def.h"
#include "GameState.h"

#include <iostream>
#include <cctype>

namespace Marius
{
	MainMenuState::MainMenuState(GameDataRef data) : _data(data)
	{

	}

	void MainMenuState::Init()
	{
		this->_data->assets.LoadTexture("Background", MAIN_MENU_BACKGROUND_FILEPATH);
		this->_data->assets.LoadTexture("Play Button", MAIN_MENU_PLAY_BUTTON);
		this->_data->assets.LoadTexture("Game Title", MAIN_MENU_TITLE_FILEPATH);
		this->_data->assets.LoadTexture("Loading Button", LOADING_BUTTON);
		this->_data->assets.LoadTexture("Name Box", NAME_BOX);
		this->_data->assets.LoadFont("Chat Font", CHAT_FONT_FILEPATH);

		this->_background.setTexture(this->_data->assets.GetTexture("Background"));
		this->_playButton.setTexture(this->_data->assets.GetTexture("Play Button"));
		this->_title.setTexture(this->_data->assets.GetTexture("Game Title"));
		this->_loadingButton.setTexture(this->_data->assets.GetTexture("Loading Button"));
		this->_nameBox.setTexture(this->_data->assets.GetTexture("Name Box"));

		this->_playButton.setPosition((this->_data->window.getSize().x / 2) - (this->_playButton.getGlobalBounds().width / 2), (this->_data->window.getSize().y / 2) - (this->_playButton.getGlobalBounds().height / 2));
		this->_title.setPosition((this->_data->window.getSize().x / 2) - (this->_title.getGlobalBounds().width / 2), this->_title.getGlobalBounds().height * 0.1);
		this->_loadingButton.setPosition((this->_data->window.getSize().x / 2) - (this->_loadingButton.getGlobalBounds().width / 2), (this->_data->window.getSize().y / 2) - (this->_loadingButton.getGlobalBounds().height / 2));
		this->_nameBox.setPosition((this->_data->window.getSize().x / 2) - (this->_nameBox.getGlobalBounds().width / 2), (this->_data->window.getSize().y / 2) + (this->_loadingButton.getGlobalBounds().height * 2));

		this->_playerReady = false;

		this->_insertText.setFont(this->_font);
		this->_insertText.setCharacterSize(15);
		this->_insertText.setFillColor(sf::Color::Green);
		this->_insertText.setString("Please Inser Your Name");
		this->_rawText = "";

		this->_font = this->_data->assets.GetFont("Chat Font");
		this->_insertText.setPosition((this->_data->window.getSize().x / 2) - (this->_insertText.getGlobalBounds().width / 2), (this->_data->window.getSize().y / 2) + (this->_loadingButton.getGlobalBounds().height * 2) + 7);
	}

	void MainMenuState::HandleInput()
	{
		sf::Event event;

		while (this->_data->window.pollEvent(event))
		{
			if (sf::Event::Closed == event.type)
			{
				this->_data->window.close();
			}
			if (this->_playerReady == false)
			{
				if (this->_data->input.IsSpriteClicked(this->_playButton, sf::Mouse::Left, this->_data->window))
				{
					this->_playerReady = true;

					std::string dataSent = "start"; //code for available players
					std::string dataRecv;

					if (_data->sendingSocket.NewChatData())
					{
						//check to see if new data is received from the server
						dataRecv = _data->sendingSocket.ReceiveDate(dataRecv);
						
						std::cout << "debug1 :" << dataRecv << std::endl;
						//if a player is available ask if he is ready
						if (dataSent == dataRecv)
						{
							std::string dataSentReady = "ready";
							_data->sendingSocket.SendData(dataSentReady);
							this->_data->machine.AddState(StateRef(new GameState(_data)), true);
						}
					}
					else
					{
						//send data to the server to look for available players
						_data->sendingSocket.SendData(dataSent);
					}
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
					_data->sendingSocket.setPlayername(_rawText);
					this->_rawText.clear();
					this->_insertText.setString("");
				}
			}
		}
	}

	void MainMenuState::Update(float dt)
	{
		if (this->_playerReady)
		{
			//check to see if new data is received from the server
			if (_data->sendingSocket.NewChatData())
			{
				std::string dataReceive;
				dataReceive = _data->sendingSocket.ReceiveDate(dataReceive);
				if (dataReceive == "ready")
				{
					this->_data->machine.setPlayerTurn();
					std::cout << "My turn" << std::endl;
					this->_data->machine.AddState(StateRef(new GameState(_data)), true);
				}
			}
		}
	}

	void MainMenuState::Draw(float dt)
	{
		this->_data->window.clear(sf::Color::Red);

		this->_data->window.draw(this->_background);

		if (this->_playerReady){
			this->_data->window.draw(this->_loadingButton);
		}
		else {
			this->_data->window.draw(this->_playButton);
		}
		this->_data->window.draw(this->_title);

		this->_data->window.draw(this->_nameBox);

		this->_data->window.draw(this->_insertText);

		this->_data->window.display();
	}
}