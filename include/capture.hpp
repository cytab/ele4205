/** capture.hpp
	\brief Fichier d'inclusion pour les fonctions de captures de vidéos.
	*/

#ifndef CAPTURE_HPP
#define CAPTURE_HPP

#include <iostream>
#include <string>
#include <sstream>
#include <time.h>
#include <stdlib.h>
#include <vector>
#include <exception>
#include <system_error>
#include <opencv2/opencv.hpp>

/**
	\typedef Resolution Structure d'enregistrement d'une résolution.
	*/
typedef struct Resolution {
	int w;
	int h;
};

/**
	\var supportedResolutions Liste des résolutions supportées.
	*/
const Resolution SUPPORTED_RESOLUTIONS[] = {
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
	\struct FrameSetting Structure d'enregistrement de paramètres
		associés à une résolution.
	\var res Résolution
	\var fps Nombre d'images par seconde
	*/
typedef struct FrameSetting {
	Resolution res;
	double fps;
};

/**
	\fn getFrameFPS Obtenir la durée d'une image (en : frame) pour une
		résolution donnée.
	\return Durée de la prise d'image en secondes ou -1 si la résolution n'est
		pas supportée.
	*/
double getFrameFPS(Resolution res);

/**
	\fn bonecVtiming
	\brief Calculer le nombre d'images par seconde pour toutes les
		résolutions supportées.
	*/
void boneCVtiming();

/**
	\fn captureVideo(std::string cameraID,
		float duration,
		std::string outputFileName)
	\brief Capture une séquence vidéo
	\param duration Durée de la capture (en secondes)
	\param res Résolution à utiliser
	\param outputFileName Nom du fichier de sortie
	**/
void captureVideo(float duration,
	Resolution res,
	std::string outputFileName);

#endif
