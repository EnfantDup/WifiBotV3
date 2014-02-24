#include "stdafx.h"
#include <winsock.h>
#include <iostream>
#include <cmath>
#include "Robot.h"


class Client
{
	public:
		Client();
		bool connexion();
		void deconnexion();
		bool sendData();
		short int Crc16(char *data, unsigned char Taille_max);

	private:
		WSADATA init; // Variable d'initialisation
		SOCKET sock; //Socket TCP
		SOCKADDR_IN addr_serv; //Adresse du wifibot
		char dataToSend[9];
		Robot robot;
};