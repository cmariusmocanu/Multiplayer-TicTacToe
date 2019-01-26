#include <iostream>
#include <WS2tcpip.h>
#include <string>
#include <sstream>

#pragma comment (lib, "ws2_32.lib")

using namespace std;

void main()
{
	// Initialze winsock
	WSADATA wsData;
	WORD ver = MAKEWORD(2, 2);

	int wsSock = WSAStartup(ver, &wsData);
	if (wsSock != 0)
	{
		cerr << "Error Initializing winsock!" << endl;
		return;
	}

	// Make a socket
	SOCKET listening = socket(AF_INET, SOCK_STREAM, 0);
	if (listening == INVALID_SOCKET)
	{
		cerr << "Can't create a new socket!" << endl;
		WSACleanup();
		return;
	}

	// Bind the ip address and port to a socket
	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(54000);
	hint.sin_addr.S_un.S_addr = INADDR_ANY;

	bind(listening, (sockaddr*)&hint, sizeof(hint));

	// Tell Winsock the socket is for listening 
	listen(listening, SOMAXCONN);

	// Create the master file descriptor set and set it to zero
	fd_set master;
	FD_ZERO(&master);

	// Add the listening socket!
	FD_SET(listening, &master);
	cout << "<Server Started> " << endl;

	bool running = true;

	while (running)
	{
		// MAKE A COPY OF THE MASTER LIST TO PASS INTO select() !!!
		fd_set copy = master;

		// See who's talking to us
		int socketCount = select(0, &copy, nullptr, nullptr, nullptr);

		// Loop through all the current connections
		for (int i = 0; i < socketCount; i++)
		{
			SOCKET sock = copy.fd_array[i];

			// check to see if is it an inbound communication
			if (sock == listening)
			{
				// Accept a new connection
				SOCKET client = accept(listening, nullptr, nullptr);

				// Add the new connection to the list of connected clients
				FD_SET(client, &master);

				// Send a welcome message to the connected client
				string welcomeMsg = "<Welcome to the TicTacToe Server!>\r\n";
				send(client, welcomeMsg.c_str(), welcomeMsg.size() + 1, 0);
				// send to the console that a new connection was established
				cout << "<New communication opened> " << endl;
			}
			else // It's an inbound message
			{
				char buf[4096];
				ZeroMemory(buf, 4096);

				// Receive message
				int bytesIn = recv(sock, buf, 4096, 0);
				if (bytesIn <= 0)
				{
					// Drop the client
					closesocket(sock);
					FD_CLR(sock, &master);
				}
				else
				{
					// Send the received data to the other client
					for (int i = 0; i < master.fd_count; i++)
					{
						SOCKET outSock = master.fd_array[i];
						if (outSock != listening && outSock != sock)
						{
							ostringstream ss;
							ss << buf;
							string strOut = ss.str();
							
							send(outSock, strOut.c_str(), strOut.size() + 1, 0);
						}
					}
					// print the received data to the console
					cout << "SOCKET #" << sock << ": " << string(buf, 0, bytesIn) << endl;
				}
			}
		}
	}

	// Remove the listening socket from the master file descriptor
	FD_CLR(listening, &master);
	closesocket(listening);

	// Message to let users know that the server is closing.
	string msg = "Server is closing \r\n";

	while (master.fd_count > 0)
	{
		// Get the socket number
		SOCKET sock = master.fd_array[0];

		// Send the shutting down message
		send(sock, msg.c_str(), msg.size() + 1, 0);

		// Remove it from the master file list and close the socket
		FD_CLR(sock, &master);
		closesocket(sock);
	}

	// Cleanup winsock
	WSACleanup();

	system("pause");
}