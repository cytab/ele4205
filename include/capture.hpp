/** capture.hpp
	\brief Fichier d'inclusion pour les fonctions de captures de vidéos.
	*/

#include <iostream>
#include <string>
#include <time.h>
#include <stdlib.h>
#include <map>
#include <opencv2/opencv.hpp>
#include <regex>

/**
	\var CAMERA_ID Identifiant de la caméra.
	*/
const std::string CAMERA_ID = "046d:0825"

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
	\var timeFrame  map contenant les variable : key(resX, resY) value(fps)
	*/
std::map <Resolution,double> timeFrame = {};

/**
	\fn getVideoFileName
	\brief Lister les périphériques USB.
	*/
std::string getLsUSB();

/**
	\fn getVideoFileName
	\brief Obtenir le chemin d'accès de la caméra à partir de son ID.
	*/
std::string getVideoFileName(std::string cameraID);


/**
	\fn bonecVtiming
	\param fileName fichier de périphérique vidéo
	\brief Calcule le frame par seconde de chaque résolution
	*/
void bonecVtiming(std::string fileName)

/**
	\fn captureVideo(std::string cameraID,
		float duration,
		std::string outputFileName)
	\brief Capture une séquence vidéo
	\param fileName Fichier de la caméra
	\param duration Durée de la capture (en secondes)
	\param res Résolution à utiliser
	\param outputFileName Nom du fichier de sortie
	**/
void captureVideo(std::string fileName,
	float duration,
	Resolution res,
	std::string outputFileName);

