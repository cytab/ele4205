/**
 *
 * Définitions nécessaires pour transférer des images par protocole
 * TCP/IP.
 *
 * **/

#include<stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <chrono>
#include <thread>
#include <sys/types.h>

#include "opencv2/highgui/highgui.hpp"
#include <opencv2/opencv.hpp>

#define PORT_NUMBER 4099
#define SERVER_ADDRESS "192.168.7.2"
#define ELE4205_OK    0b00000001
#define ELE4205_QUIT  0b00000010
#define ELE4205_RES01 0b00000100
#define ELE4205_RES02 0b00001000
#define ELE4205_RES03 0b00010000
#define ELE4205_RES04 0b00100000
#define INITIAL_RES_INDEX 2

#define ESC 27
#define FRAME_WINDOW_NAME "Capture vidéo"
#define MENU_WINDOW_NAME "Sélectionner la résolution"
#define MENU_W 500
#define MENU_H 500
#define TEXT_HEIGHT 32
#define TEXT_WIDTH 200
#define FIRST_BUTTON_Y 100
#define BUTTON_X 100

#ifdef Debug
#define log_info(x)	std::cout << x << std::endl;
#else
#define log_info(x)
#endif

// Signatures des fonctions client.
/**
 * Créer un connecteur réseau (socket) pour interagir avec le serveur.
 * \return Descripteur de fichier du socket.
 * **/
short SocketCreate(void)
{
    return socket(AF_INET, SOCK_STREAM, 0);
}

/**
 * Configurer le socket client et le connecter au serveur. 
 * \param hSocket Descripteur de fichier du socket client.
 * \return Code d'erreur de l'opération (0 pour succès, autres pour erreur).
 */
int SocketConnect(int hSocket);

// Signatures des fonctions serveur.
/**
 * Envoyer l'entête d'une image OpenCV au client.
 *
 * \param frame Image OpenCV.
 * \param sock Descripteur de fichier du socket serveur.
 * \return Code d'erreur (0 pour succès).
 */
int sendEntete(cv::Mat frame, int sock);

/**
 * Envoyer une image complète vers le client.
 *
 * \param capture Object VideoCapture pour prendre des images.
 * \param frame Image OpenCV.
 * \param sock Descripteur de fichier du socket serveur.
 * \param bytes Codes d'erreur.
 * \param flag Fanions de configuration de la fonction.
 * \return Code d'erreur (0 pour succès).
 */
int sendImage(cv::VideoCapture capture,cv::Mat frame, int sock, int flag);

/**
 * Lier le socket serveur au socket client.
 *
 * \param hsocket Descripteur de fichier du socket serveur.
 * \param p Port du client.
 * \return Code d'erreur (0 pour succès).
 */
int BindCreatedSocket(int hSocket, int p);

