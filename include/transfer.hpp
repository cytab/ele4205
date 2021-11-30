/**
 *
 * @file Définitions nécessaires pour l'application en mode client / serveur.
 *
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
#ifdef BUILD_CLIENT
#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>
#endif

#include "capture.hpp"

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
#define MENU_W 800
/** @brief Hauteur de la fenêtre de sélection. */
#define MENU_H 500
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
 * Résolutions supportées.
 */
const Resolution CAMERA_RESOLUTIONS[] = {
	{160, 120},
	{320, 240},
	{800, 600},
	{1280, 720}
};

/**
 * Fréquences en fonction du nom des notes.
 */
const std::map<std::string, int> NOTE_FREQUENCIES = {
	{"A",  440},
	{"A#", 466},
	{"B",  494},
	{"C",  523},
	{"C#", 554},
	{"D",  587},
	{"D#", 622},
	{"E",  659},
	{"F",  698},
	{"F#", 734},
	{"G",  784},
	{"G#", 831},
	{"a",  880},
	{"a#", 932},
	{"b",  988},
	{"c",  1046},
	{"c#", 1109},
	{"d",  1175},
	{"d#", 1245},
	{"e",  1319},
	{"f",  1397},
	{"f#", 1480},
	{"g",  1568},
	{"g#", 1661},
	{"R", 0}
};

/**
 * Durée en fonction du chiffre suivant la note. La durée est données
 * en nombres de temps, c'est-à-dire en nombre de noires.
 */
const std::map<int, float> NOTE_DURATIONS = {
	{1, 4.0},
	{2, 2.0},
	{4, 1.0},
	{8, 0.5}
};

/**
 * Structure de stockage d'une note.
 */
typedef struct Note {
	int frequency;
	float duration;
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
 * Numéro séquentiel des images à enregistrer.
 */
extern int imageID;

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
int readAdc(); 

/**
 * Lis la valeur du adc du répertoire /sys/class/gpio/gpio228/value 
 * (value est un fichier et non un programme)
 * \return Valeur entre 0 et 1 
 */
int readButton(); 

/**
 * Configurer le GPIO 
 */
void setEnvGpio(); 

/**
 * Enregistrer une image sous format PNG selon le nom fourni en argument.
 * Ensuite, reconnaître les caractères de l'images et les afficher dans
 * le terminal. Le code est inspiré d'un exemple trouvé sur
 * https://tesseract-ocr.github.io/tessdoc/APIExample.html
 *
 * \param imgName Nom de l'image à sauvergarder sur le disque.
 * \param frame Image à enregistrer
 */
void readAndSendMusic(int imgName, cv::Mat* frame);

/**
 * Récupère le tempo se trouvant en début de partition.
 * \param sheetMusic Fichier texte contenant la partition de musique
 */
int getTempo(std::string* sheetMusic);

/**
 * Décrypte  un code (exp : R8) en retrouvant sa fréquence et sa durée.
 *
 * \param code contenant un code encryptant la note.
 * \return retourne Note
 */
Note getNote(std::string code);

/**
 * Parcourt la partition tout en s'assurant que les codes de lettres 
 * satisfassent la conditions A-Za-z# et appelle getNote.
 * \param sheetMusic Fichier texte contenant la partition de musique.
 * \param notes référence à la structure Notes quiest remplit pendant 
 *     la lecture de la partitiom
 * 
 */
void getNotes(std::string* sheetMusic, std::vector<Note>& notes);

/**
 * Écris dans le PATH du buzzer afin de produire le son d'une note .
 * 
 * \param note Structure contenant la fréquence et la durée d'une note .
 * \param beat variable obtenu à partir du tempo 60.0 / (float) tempo
 */
void playNote(Note& note, float beat);

/**
 * 
 * Jouer une pièce.
 * \param sheetMusic Pointeur vers le texte contenant la partition musicale.
 */
void playMusic(std::string* sheetMusic);
