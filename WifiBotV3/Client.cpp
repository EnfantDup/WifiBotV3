#include "stdafx.h"

using namespace std;

Client::Client()
{
}

bool Client::connexion()
{
	WSADATA initialisation; // Variable d'initialisation
	int erreur;
	bool valide = true; //Connexion valide ?

	//Initialisation
	erreur = WSAStartup(MAKEWORD(2,2),&initialisation);

	if (erreur!=0)
	{
		cerr << "Probleme d'initialisation de winsock, erreur n° " << erreur << " " << WSAGetLastError() << endl;
		valide = false;
	}

	//socket TCP
	SOCKET sock;

	//Creation de la socket
	sock = socket(PF_INET, SOCK_STREAM, 0);

	if (sock == INVALID_SOCKET)
	{
		cerr << "Probleme de creation de socket :" << WSAGetLastError() << endl;
		valide = false;
	}

	//adresse du serveur
	SOCKADDR_IN serv; 

	//Proprietes
	serv.sin_family=AF_INET;
	serv.sin_addr.s_addr=inet_addr("127.0.0.1"); 
	serv.sin_port=htons(15020); 

	//Et enfin connexion
	erreur = connect(sock,(struct sockaddr*)&serv,sizeof(serv));

	if (erreur!=0)
	{
		cerr << "Probleme de connexion, erreur n°" << erreur << " " << WSAGetLastError() << endl;
		valide = false;
	}

	return valide;
}