#include "SendingSocket.h"

namespace Marius
{
	SendingSocket::SendingSocket():recvThread(&SendingSocket::ReceiveThredData, this)
	{

	}
	SendingSocket::~SendingSocket() 
	{

	}
	void SendingSocket::init()
	{
		std::string ipAddress = "127.0.0.1";			// IP Address of the server
		int port = 54000;								// Listening port # on the server

		// Initialize WinSock
		WSAData data;
		WORD ver = MAKEWORD(2, 2);
		this->_wsResult = WSAStartup(ver, &data);
		if (this->_wsResult != 0)
		{
			std::cerr << "Can't start Winsock, Err #" << this->_wsResult << std::endl;
			return;
		}

		// Create socket
		this->_sock = socket(AF_INET, SOCK_STREAM, 0);
		if (this->_sock == INVALID_SOCKET)
		{
			std::cerr << "Can't create socket, Err #" << WSAGetLastError() << std::endl;
			WSACleanup();
			return;
		}

		// Bind the ip address and port to a socket
		sockaddr_in hint;
		hint.sin_family = AF_INET;
		hint.sin_port = htons(port);
		inet_pton(AF_INET, ipAddress.c_str(), &hint.sin_addr);

		// Connect to server
		this->_connResult = connect(this->_sock, (sockaddr*)&hint, sizeof(hint));
		if (this->_connResult == SOCKET_ERROR)
		{
			std::cerr << "Can't connect to server, Err #" << WSAGetLastError() << std::endl;
			closesocket(this->_sock);
			WSACleanup();
			return;
		}
		else
		{
			std::cout << "<Conected to server>" << std::endl;
			this->_connOpen = true;
		}
		this->_newDataRecv = true;
		recvThread.launch();
	}
	void SendingSocket::SendData(std::string dataForSend)
	{
		if (this->_connOpen == true)
		{
			// Send the text
			int sendResult = send(this->_sock, dataForSend.c_str(), dataForSend.size() + 1, 0);
			if (sendResult == SOCKET_ERROR)
			{
				std::cout << "Error to send mesage!" << std::endl;
			}
		}
		
	}
	bool SendingSocket::NewData()
	{
		std::string temp = this->_dataRecv;
		std::string temp2 = temp.substr(0, 2);
		if (this->_newDataRecv &&(temp2 == "/0"))//new system data
		{
			std::cout << "new data recv" << std::endl;
			this->_newSystemDataRecv = true;
			return true;
		}
		return false;
	}
	bool SendingSocket::NewChatData()
	{
		std::string temp = this->_dataRecv;
		std::string temp2 = temp.substr(0, 2);
		if (this->_newDataRecv && (temp2 != "/0"))//new chat data
		{
			this->_newChatDataRecv = true;
			return true;
		}
		return false;
	}
	std::string SendingSocket::ReceiveDate(std::string dataReceived)
	{
		std::string temp = "No Data";
		//if received new data do:
		std::cout << "<Server> :" << this->_dataRecv << std::endl;
		if (this->_newSystemDataRecv)
		{
			temp = this->_dataRecv;
			this->_newSystemDataRecv = false;
			this->_newDataRecv = false;
			return temp;
		}
		else if (this->_newChatDataRecv)
		{
			temp = this->_dataRecv;
			this->_newChatDataRecv = false;
			this->_newDataRecv = false;
			return temp;
		}
		return temp;
		
	}
	void  SendingSocket::ReceiveThredData()
	{
		while (this->_connOpen == true)
		{
			sf::Lock lockDate(threadLock);
			// Wait for response
			ZeroMemory(this->_buffer, 4096);
			int bytesReceived = recv(this->_sock, this->_buffer, 4096, 0);
			if (bytesReceived > 0)
			{
				this->_dataRecv = std::string(this->_buffer, 0, bytesReceived);
				//std::cout << "Server>" << this->_dataRecv << std::endl;
				this->_newDataRecv = true;
			}
		}
	}

	void SendingSocket::closeConnection()
	{
		// Gracefully close down everything
		recvThread.terminate();
		closesocket(this->_sock);
		WSACleanup();
	}

	void SendingSocket::setPlayername(std::string name)
	{
		this->_playerName = name;
	}
	std::string SendingSocket::getPlayerName()
	{
		return this->_playerName;
	}
}
