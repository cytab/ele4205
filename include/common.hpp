/**

	@file Définitions nécessaires pour l'application en mode client / serveur. 
	
	\mainpage
	Projet du cours ELE4205 consistant à réaliser une application de
	capture de vidéos.

	Ordre des transfers entre le client et le serveur :

	SERVEUR -> CLIENT  : State

	CLIENT  -> SERVEUR : Résolution

	SERVEUR -> CLIENT  : Image (optionnel)

	CLIENT -> SERVEUR  : Partition

	Pour toutes les informations concernant la compilation et l'exécution
	de l'application, consultez le projet
	https://bitbucket.org/rgourdeau/ele4205_projet_aut2021_25/src/master/.

	L'application est capable de lire des fichiers tels que :

	\image html Autumn_leaves-ocr.png

**/

#include<stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <chrono>
#include <thread>
#include <map>
#include <sys/types.h>
#include <regex>

#include "opencv2/highgui/highgui.hpp"
#include <opencv2/opencv.hpp>

// Paramètres de transferts de données
/** @brief Port pour le transfert d'images. */
#define PORT_NUMBER		4099
/** @brief Port pour le transfert de musique. */
#define MUSIC_PORT_NUMBER	4100
/** @brief Adresse IP de la Odroid-C2. */
#define SERVER_ADDRESS	"192.168.7.2"
/** @brief Fanion indiquant que la transmission est réussie. */
#define ELE4205_OK	0b00000001
/** @brief Fanion indiquant que la transmission est un échec. */
#define ELE4205_QUIT	0b00000010
/** @brief Masque indiquant que la résulution 1 est sélectionnée. */
#define ELE4205_RES01	0b00000100
/** @brief Masque indiquant que la résulution 2 est sélectionnée. */
#define ELE4205_RES02	0b00001000
/** @brief Masque indiquant que la résulution 3 est sélectionnée. */
#define ELE4205_RES03	0b00010000
/** @brief Masque indiquant que la résulution 4 est sélectionnée. */
#define ELE4205_RES04	0b00100000
/** @brief Indice de la résulution initiale. */
#define INITIAL_RES_INDEX 2

/** @brief Masque indiquant que le capteur détecte de la lumière. */
#define STATE_READY 0b00000001
/** @brief Masque indiquant que le bouton est appuyé. */
#define STATE_IDOWN 0b00000010
/** @brief Masque indiquant que le bouton n'est pas appuyé. */
#define STATE_PUSHB 0b00000100

// Paramètres de l'interface graphique du client.
/** @brief Identifiant de la touche ÉCHAPP */
#define ESC 27
/** @brief Temps d'attente en millisecondes quand ECHAPP est appuyé. */
#define ESC_WAIT_TIME_MICRO_S	30
/** @brief Nom de la fenêtre de visualisation des images. */
#define FRAME_WINDOW_NAME "Capture vidéo"
/** @brief Nom de la fenêtre de sélection de la résolution. */
#define MENU_WINDOW_NAME "Sélectionner la résolution"
/** @brief Largeur de la fenêtre de sélection. */
#define MENU_W 1000
/** @brief Hauteur de la fenêtre de sélection. */
#define MENU_H 450
/** @brief Hauteur du texte. */
#define TEXT_HEIGHT	32
/** @brief Largeur du texte. */
#define TEXT_WIDTH	200
/** @brief Position verticale du premier bouton. */
#define FIRST_BUTTON_Y	100
/** @brief Position horizontale du premier bouton. */
#define BUTTON_X	100
/** @brief Espace entre les boutons. */
#define GUI_BUTTON_PADDING 10

/** @brief Hauteur de la fenêtre vide. */
#define BLANK_FRAME_H		200
/** @brief Largeur de la fenêtre vide. */
#define BLANK_FRAME_W		400
/** @brief Nom de la fenêtre vide. */
#define BLANK_FRAME_NAME	"Pas d'image."

// Paramètres pour l'utilisation des broches sur la Odroid-C2.
/** @brief Identifiant de la broche de lecture du bouton. */
#define GPIO_ID "228"
/** @brief Direction de la lecture. */
#define GPIO_DIR "in"
/** @brief Nom du fichier du convertisseur analogique / numérique. */
#define ADC_FILENAME "/sys/class/saradc/ch0"
/** @brief Mode de lecture des fichiers. */
#define READ_FILE_MODE "r"
/** @brief Mode d'écriture des fichiers. */
#define WRITE_FILE_MODE "w"
/** @brief Nom du fichier de la valeur du GPIO. */
#define GPIO_FILENAME "/sys/class/gpio/gpio228/value"
/** @brief Nom du fichier d'exportation du GPIO. */
#define EXPORT_FILE "/sys/class/gpio/export"
/** @brief Nom du fichier de direction du GPIO. */
#define GPIO_DIR_DIRECTORY "/sys/class/gpio/gpio228/direction"
/** @brief Seuil de détection de lumière du ADC. */
#define ADC_THRESHOLD	1000
/** @brief État de repos du bouton. */
#define BUTTON_UP	1

/** @brief Chemin d'accès pour la fréquence du PWM */
#define FREQUENCY_PATH	"/sys/devices/pwm-ctrl.42/freq0"
/** @brief Chemin d'accès pour l'activation du PWM. */
#define ENABLE_PATH	"/sys/devices/pwm-ctrl.42/enable0"
/** @brief Chemin d'accès pour le rapport cyclique du PWM. */
#define DUTY_PATH	"/sys/devices/pwm-ctrl.42/duty0"

// Fonction utilitaire de débogage.
#ifdef Debug
/** @brief Fonction de journalisation. */
#define log_info(x)	std::cout << x << std::endl;
#else
/** @brief Fonction de journalisation. */
#define log_info(x)
#endif

/**
 * Resolution Structure d'enregistrement d'une résolution.
 */
struct Resolution {
	int w;
	int h;
};

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
int SocketConnect(int hSocket, int port=PORT_NUMBER);

/**
 * Lier le socket serveur au socket client.
 *
 * \param hsocket Descripteur de fichier du socket serveur.
 * \param p Port du client.
 * \return Code d'erreur (0 pour succès).
 */
int BindCreatedSocket(int hSocket, int p);

