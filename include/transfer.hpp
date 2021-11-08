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

#include "capture.hpp"

#define PORT_NUMBER	4099
#define SERVER_ADDRESS	"192.168.7.2"
#define ELE4205_OK	0b00000001
#define ELE4205_QUIT	0b00000010
#define ELE4205_RES01	0b00000100
#define ELE4205_RES02	0b00001000
#define ELE4205_RES03	0b00010000
#define ELE4205_RES04	0b00100000
#define INITIAL_RES_INDEX 2

#define STATE_READY 0b00000001
#define STATE_IDOWN 0b00000010
#define STATE_PUSHB 0b00000100

#define ESC 27
#define FRAME_WINDOW_NAME "Capture vidéo"
#define MENU_WINDOW_NAME "Sélectionner la résolution"
#define MENU_W 500
#define MENU_H 500
#define TEXT_HEIGHT	32
#define TEXT_WIDTH	200
#define FIRST_BUTTON_Y	100
#define BUTTON_X	100

#define GPIO_ID "228"
#define GPIO_DIR "in"
#define ADC_FILENAME "/sys/class/saradc/ch0"
#define READ_FILE_MODE "r"
#define WRITE_FILE_MODE "w"
#define GPIO_FILENAME "/sys/class/gpio/gpio228/value"
#define EXPORT_FILE "/sys/class/gpio/export"
#define GPIO_DIR_DIRECTORY "/sys/class/gpio/gpio228/direction"

#ifdef Debug
#define log_info(x)	std::cout << x << std::endl;
#else
#define log_info(x)
#endif

/*
Ordre des transfers :
SERVEUR -> CLIENT  : State
CLIENT  -> SERVEUR : Resolution
SERVEUR -> CLIENT  : Image (optionnel)
*/

/**
 * Résolutions supportées.
 */
const Resolution CAMERA_RESOLUTIONS[] = {
	{160, 120},
	{320, 240},
	{800, 600},
	{1280, 720}
};

/**
 * Obtenir le masque de bits pour les résolutions à partir d'un indice.
 *
 * \param index Indice de 0 à 3
 * \return Masque de bits
 */
uint32_t getResMask(uint32_t index){
	uint32_t mask = ELE4205_RES01;
	mask = mask << index;
	return mask;
}

/**
 * Obtenir l'indice d'une résolution à partir d'un masque de bits.
 *
 * \param mask Masque de bits
 * \return Indice de 0 à 3
 */
uint32_t getResIndex(uint32_t mask){
	for (uint32_t i = 0; i < 4; i++){
		if ((mask & (ELE4205_RES01 << i)) != 0){
			return i;
		}
	}
	return -1;
}

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

// Fonctions de l'interface graphique.
/**
 * Réagir aux évènements de la souris.
 * \param event Identifiant de l'évènement.
 * \param x Position du curseur (vertical).
 * \param y Position du curseur (horizontal).
 * \param flags Fanions de configuration.
 * \param userdata Pointeur vers une zone mémoire (inutilisée).
 */
void mouseCallBack(int event, int x, int y, int flags, void* userdata);

/**
 * Initialiser la fenêtre de l'interface graphique.
 * \param menuImage Référence vers une matrice à afficher.
 */
void initializeMenu(cv::Mat &menuImage);

/**
 * Lis la valeur du adc du répertoire /sys/class/saradc
 * \return Valeur entre 0 et 1023 (plus lumineurx -> moins lumineux)
 */
int readAdc() ; 

/**
 * Lis la valeur du adc du répertoire /sys/class/gpio/gpio228/value 
 * (value est un fichier et non un programme)
 * \return Valeur entre 0 et 1 
 */
int readButton() ; 

/**
 * Configurer le GPIO 
 */
void setEnvGpio() ; 
