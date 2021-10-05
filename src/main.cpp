/**
	\mainpage
	Projet du cours ELE4205 consistant à réaliser une application de
	capture de vidéos.
**/

/**
	\fn main Lancement du programme
	\brief Appel dpuis la ligne de commande : `./main <resX> <resY>
	*/
int main (int argc , char ** argv) {
	if (argc < 2){
		std::cout << "Veuillez fournir la résolution." '' std::endl;
		return -1;
	}
	int resX = std::s_toi(std::string(argv[1]));
	int resY = std::s_toi(std::string(argv[2]));
	std::string filename = getVideoFileName(CAMERA_ID);
	bonecVtiming(filename);
	captureVideo(fileName, 5, Resolution{resX, resY}, "capture-liv1.avi");
{
