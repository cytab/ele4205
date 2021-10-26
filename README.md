# Projet de session, ELE4205

Auteurs : Cyrille Jamabel TABE et Vincent THERRIEN (1956352)

Date : Automne 2021

Polytechnique Montréal, département de génie électrique

## Description

Ce répertoire contient le projet de session réalisé dans le cadre du cours
ELE4205 - Systèmes d'exploitation et interfaces matérielles. Les sous-sections
suivantes composent le répertoire :

- `conf` : Fichiers de configuration de la compilation.
- `doc` : Documentation du projet. Uniquement les fichiers sources sont
  versionnés.
- `include` : Fichiers d'inclusion.
- `src` : Fichiers sources.

## Compilation du projet

Obtenez d'abord la chaîne de compilation (SDK) du projet. L'exemple
ci-dessous montre une installation à l'intérieur du répertoire du projet :
```bash
mkdir build # Le répertoire `build` est ignoré par git par défaut.
cd build
cmake ..
make
```

Pour inclure des informations de débogage, remplacez la command `cmake ..`
par `cmake -DCMAKE_BUILD_TYPE="Debug" ..`

## Compilation de la documentation

Pour compiler la documentation, exécutez, dans le répertoire
`ele4205_projet_aut2021_25/doc`, la commande : `$ doxygen Doxyfile`. Le
logiciel `doxygen` est nécessaire pour cette opération; s'il ne l'est pas,
référez-vous à la page du projet en question :
`https://www.doxygen.nl/index.html`

## Progrès

### Livrable 1

19 octobre : fin du livrable 1.

### Livrable 2

26 octobre : fin du livrable 2

Pour le serveur (server.cpp), on utilise la compilation croisée avec le SDK du
projet. Le fichier exécutable est ensuite transmis sur la Odroid à l'aide de la
commande `scp root@192.168.7.2:/home/root`.

Pour l'application client (client.cpp), on compile en natif directement sur
Centos.

