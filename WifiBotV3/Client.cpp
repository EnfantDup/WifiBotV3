#include "stdafx.h"

using namespace std;

Client::Client()
{
	robot = new Robot();
}

bool Client::connexion()
{
	
	int erreur;
	bool valide = true; //Connexion valide ?

	//Initialisation
	erreur = WSAStartup(MAKEWORD(2,2),&init);

	if (erreur!=0)
	{
		cerr << "Probleme d'initialisation de winsock, erreur n° " << erreur << " " << WSAGetLastError() << endl;
		valide = false;
	}

	//Creation de la socket
	sock = socket(PF_INET, SOCK_STREAM, 0);

	if (sock == INVALID_SOCKET)
	{
		cerr << "Probleme de creation de socket :" << WSAGetLastError() << endl;
		valide = false;
	}

	//Proprietes
	addr_serv.sin_family=AF_INET;
	addr_serv.sin_addr.s_addr=inet_addr("127.0.0.1"); 
	addr_serv.sin_port=htons(15020); 

	//Et enfin connexion
	erreur = connect(sock,(struct sockaddr*)&addr_serv,sizeof(addr_serv));

	if (erreur!=0)
	{
		cerr << "Probleme de connexion, erreur n°" << erreur << " " << WSAGetLastError() << endl;
		valide = false;
	}

	return valide;
}

void Client::deconnexion()
{
	closesocket(sock);
    WSACleanup();
}

bool Client::sendData()
{
	//Buffer de test
	/*
	dataToSend[0] = (char)(255);
	dataToSend[1] = (char)(0x07);
	dataToSend[2] = (char)(120);
	dataToSend[3] = (char)(0);
	dataToSend[4] = (char)(120);
	dataToSend[5] = (char)(0);
	dataToSend[6] = (char)(80);*/

	char direction = 80; //En avant

	robot->proceedSpeed();

	if(robot->getLeftSpeed() < 0)
		direction -= 16; //Roues gauche en arrière

	if(robot->getRightSpeed() < 0)
		direction -= 64; //Roues droites en arrière

	dataToSend[0] = (char)(255); //255
	dataToSend[1] = (char)(0x07); //Taille
	dataToSend[2] = (char)(abs(robot->getLeftSpeed())); //Vitesse à gauche
	dataToSend[3] = (char)(0);
	dataToSend[4] = (char)(abs(robot->getRightSpeed())); //Vitesse à droite
	dataToSend[5] = (char)(0);
	dataToSend[6] = (char)(direction);

	//CRC
	short int crc = Crc16(dataToSend, 7);
	dataToSend[7] = (char)(crc);
	dataToSend[8] = (char)(crc >> 8);

	int erreur = send(sock, dataToSend, 9, 0);

	if (erreur == SOCKET_ERROR)
		return false;
	else
		return true;
}

short int Client::Crc16(char *Adresse_tab , unsigned char Taille_max)
{
		unsigned int Crc = 0xFFFF;
		unsigned int Polynome = 0xA001;
		unsigned int CptOctet = 0;
		unsigned int CptBit = 0;
		unsigned int Parity= 0;

		for ( CptOctet= 1 ; CptOctet < Taille_max ; CptOctet++)
		{
			Crc ^= *( Adresse_tab + CptOctet);
			for ( CptBit = 0; CptBit <= 7 ; CptBit++)
			{
				Parity= Crc;
				Crc >>= 1;
				if (Parity%2 == true) 
					Crc ^= Polynome;
			}
		}
		return(Crc);
}

Robot* Client::getRobot()
{
	return robot;
}
