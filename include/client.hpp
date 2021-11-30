#include "common.hpp"

#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>

/**
 * Numéro séquentiel des images à enregistrer.
 */
extern int imageID;

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
 * Enregistrer une image sous format PNG selon le nom fourni en argument.
 * Ensuite, reconnaître les caractères de l'images et les afficher dans
 * le terminal. Le code est inspiré d'un exemple trouvé sur
 * https://tesseract-ocr.github.io/tessdoc/APIExample.html
 *
 * \param imgName Nom de l'image à sauvergarder sur le disque.
 * \param frame Image à enregistrer
 */
void readAndSendMusic(int imgName, cv::Mat* frame);

