/* capture.hpp
 *
 */

#include <string>
#include <opencv2/opencv.hpp>

typedef struct Resolution {
	unsigned int w,
	unsigned int h
}

Resolution supportedResolutions[] = {
	Resolution(176, 144),
	Resolution(160, 120),
	Resolution(320, 176),
	Resolution(320, 240),
	Resolution(352, 288),
	Resolution(432, 240),
	Resolution(800, 600),
	Resolution(864, 480),
	Resolution(960, 544),
	Resolution(960, 720),
	Resolution(1184, 656),
	Resolution(1280, 720),
	Resolution(1280, 960)
}

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
