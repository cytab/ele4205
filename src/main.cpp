/**
	\mainpage
	Projet du cours ELE4205 consistant à réaliser une application de
	capture de vidéos.
**/

#include <capture.hpp>

#define CAPTURE_DURATION_5 5.0
#define N_INPUT_ARGUMENTS  2

/**
	\var OUTPUT_FILE_NAME Nom du fichier de sortie.
	*/
const std::string OUTPUT_FILE_NAME("capture-liv1.avi");

/**
	\fn main Lancement du programme
	\brief Appel dpuis la ligne de commande : `./main <resX> <resY>
	*/
int main (int argc, char ** argv) {
	if (argc < N_INPUT_ARGUMENTS){
		std::cout << "Veuillez fournir la résolution." << std::endl;
		std::cout << "Usage : ./capture <w> <h>" << std::endl;
		return -1;
	}
	int resW = std::stoi(std::string(argv[1]));
	int resH = std::stoi(std::string(argv[2]));
	boneCVtiming();
	captureVideo(CAPTURE_DURATION_5, Resolution{resW, resH}, OUTPUT_FILE_NAME);
}
