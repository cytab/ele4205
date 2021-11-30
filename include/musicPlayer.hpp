#include "common.hpp"

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
struct Note {
	int frequency;
	float duration;
};

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

