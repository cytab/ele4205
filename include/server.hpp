#include "common.hpp"

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

