/** capture.hpp
	\brief Fichier d'inclusion pour les fonctions de captures de vidéos.
	*/

#include <iostream>
#include <string>
#include <time.h>
#include <opencv2/opencv.hpp>

/**
	\typedef Resolution Structure d'enregistrement d'une résolution.
	*/
typedef struct Resolution {
	unsigned int w,
	unsigned int h
}

/**
	\var supportedResolutions Liste des résolutions supportées.
	*/
const Resolution supportedResolutions[] = {
	{176, 144},
	{160, 120},
	{320, 176},
	{320, 240},
	{352, 288},
	{432, 240},
	{800, 600},
	{864, 480},
	{960, 544},
	{960, 720},
	{1184, 656},
	{1280, 720},
	{1280, 960}
};

/**
	\fn captureVideo(std::string cameraID,
		float duration,
		std::string outputFileName)
	\brief Capture une séquence vidéo
	\param cameraID Identifiant de la caméra
	\param duration Durée de la capture (en secondes)
	\param res Résolution à utiliser
	\param outputFileName Nom du fichier de sortie
	**/
void captureVideo(std::string cameraID,
	float duration,
	Resolution res,
	std::string outputFileName);
