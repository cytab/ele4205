/**
	\mainpage
	Projet du cours ELE4205 consistant à réaliser une application de
	capture de vidéos.
**/

#include <capture.hpp>

/**
	\fn main Lancement du programme
	\brief Appel dpuis la ligne de commande : `./main <resX> <resY>
	*/
int main (int argc, char ** argv) {
	if (argc < 2){
		std::cout << "Veuillez fournir la résolution (w, h)." << std::endl;
		return -1;
	}
	int resX = std::stoi(std::string(argv[1]));
	int resY = std::stoi(std::string(argv[2]));
	std::string filename = getVideoFileName("046d:0825");
	boneCVtiming(filename);
	captureVideo(filename, 5.0, Resolution{resX, resY}, "capture-liv1.avi");
}
