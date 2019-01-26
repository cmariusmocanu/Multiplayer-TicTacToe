#pragma once

#include <iostream>
#include <string>
#include <sstream>
#include <WS2tcpip.h>
#include "SFML\Network.hpp"
#pragma comment(lib, "ws2_32.lib")

namespace Marius
{
	class SendingSocket
	{
	public:
		SendingSocket();
		~SendingSocket();

		void init();
		void SendData(std::string dataForSend);
		std::string ReceiveDate(std::string dataReceived);
		void ReceiveThredData();
		bool NewData();
		bool NewChatData();
		void closeConnection();

		void setPlayername(std::string name);
		std::string getPlayerName();



	private:
		SOCKET _sock;
		int _wsResult;
		int _connResult;
		bool _connOpen;
		char _buffer[4096];
		sf::Thread recvThread;
		sf::Mutex threadLock;

		std::string _dataRecv;
		std::string _dataChatRecv;
		bool _newDataRecv;
		bool _newChatDataRecv;
		bool _newSystemDataRecv;

		std::string _playerName;
	};
}