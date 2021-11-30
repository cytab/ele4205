# Projet de session, ELE4205

Auteurs : Cyrille Jamabel TABE (1863167) et Vincent THERRIEN (1956352)

Date : Automne 2021

Polytechnique Montréal, département de génie électrique
Travail réalisé dans le cadre du cours ELE4205.

## Description

Ce répertoire contient le projet de session réalisé dans le cadre du cours
ELE4205 - Systèmes d'exploitation et interfaces matérielles.

L'application logicielle développée est exécutée sur un ordinateur bureau
(*desktop*) qui communique avec une Odroid-C2, un ordinateur monocarte
(*single-board computer*), afin de lire des partitions musicales et de
les faire jouer par la Odroid-C2. Les rôles de chaque partie sont détaillés
ci-dessous :

- L'**Ordinateur bureau** est le client de l'application. Il
  reçoit deux types de données du serveur : (1) des images et (2) l'état
  d'un bouton qui indique si l'image reçue doit être décodée en partition.
  Si c'est le cas, l'image captée est utilisée pour élaborer une série de
  caractères qui soont transférés au serveur. Le client transmet deux types
  de données au serveur : (1) des partitions et (2) des résolutions à utiliser
  pour la capture des images. Quatre résolutions peuvent être sélectionnées
  avec une interface graphique en gardant le bouton de la souris enfoncé sur
  la résolution souhaitée.
- La **Odroid-C2** est un ordinateur monocarte qui agit comme le serveur de
  l'application. Lorsqu'un capteur photosensible détecte de la lumière, la
  caméra branchée à la Odroid-C2 capte des images et les retransmet vers le
  client. Lorsque le bouton est appuyé, le client reçoit l'instruction de
  décoder l'image et de renvoyer sa transcription. En recevant la
  transcription, la Odroid-C2 fait jouer les notes sur bipeur (*buzzer*).

Les sous-sections suivantes composent le répertoire :

- `doc` : Documentation du projet. Uniquement les fichiers sources sont
  versionnés. Pour générer la documentation, consultez la partie sur la
  [documentation](compilation-de-la-documentation).
- `include` : Fichiers d'inclusion (en-têtes du projet).
- `src` : Fichiers sources.

Le projet s'appuie sur les bibliothèques suivantes :

- **OpenCV** pour la capture et le traitement d'images
- **Tesseract** et **Leptonica** pour la reconnaissance de caractères

## Compilation du projet

### Obtention du SDK

Obtenez d'abord la chaîne de compilation (SDK) du projet :
```bash
# Obtenir Yocto et Open-embedded pour la Odroid-C2
bash
cd /export/tmp/4205_nn/ # Répertoire du projet
git clone -b krogoth git://git.yoctoproject.org/poky.git
cd poky
git clone -b krogoth git://git.openembedded.org/meta-openembedded
git clone -b master https://github.com/akuster/meta-odroid.gi
cd meta-odroi
git checkout 89685506742fa9d9c1860f3eebae5850e6235bdf
cd ..
# Ajouter les couches du système
bitbake-layers add-layer /export/tmp/4205_nn/poky/meta-odroid/
bitbake-layers add-layer /export/tmp/4205_nn/poky/meta-openembedded/meta-oe/
bitbake-layers add-layer /export/tmp/4205_nn/poky/ele4205-labo1/
```

Dans le fichier `conf/local.conf`, effectuez les modifications suivantes pour
sélectionner l'architecture cible et les bibliothèques nécessaires :
```
MACHINE = "odroid-c2"
...
IMAGE_INSTALL_append = " \
  v4l-utils \
  python-modules \
  opencv \
  python-opencv \
"
```

On peut ensuite compiler l'image :
```
umask a+rx u+rwx
nice bitbake core-image-minimal
```

Pour faire la compilation avec le SDK, on exécute :
```
cd /export/tmp/4205_nn/poky/
source oe-init-build-env build-oc2
umask a+rx u+rwx
nice bitbake -c populate_sdk core-image-base
sh ./tmp/deploy/sdk/poky-glibc-x86_64-core-image-base-aarch64-toolchain-2.1.3.sh
. /export/tmp/4205_nn/opt/poky/environment-setup-aarch64-poky-linux
source /export/tmp/4205_nn/opt/poky/environment-setup-aarch64-poky-linux
```

### Compilation de l'application

Pour générer l'application, il faut d'abord configurer le projet :
```bash
git clone  https://<Nom utilisateur>@bitbucket.org/rgourdeau/ele4205_projet_aut2021_25.git
cd ELE4205_PROJET_AUT2021_25
mkdir build
cd build
```

On compile les exécutables du serveur avec le SDK :

```bash
bash
source /export/tmp/4205_nn/opt/poky/environment-setup-aarch64-poky-linux
# Compiler les exécutables du serveur et les transférer vers le serveur.
cmake -DBUILD_SERVER=ON ..
make
# Réinitialiser la connection SSH.
sudo /users/Cours/ele4205/commun/scripts/ifconfig-enp0s-up
# Transférer les exécutables.
scp server root@192.168.7.2:/home/root
scp musicPlayer root@192.168.7.2:/home/root
```

On compile l'exécutable du client sans le SDK. Dans un autre terminal,
exécuter :

```bash
# Effacer les données dans le cache avant de compiler l'exécutable du client.
rm -r ./*
# Compiler l'exécutable du client.
cmake -DBUILD_CLIENT=ON ..
make
```

Pour inclure des informations de débogage, remplacez la commande
`cmake <options> ..` par `cmake <options> -DCMAKE_BUILD_TYPE="Debug" ..`.

## Exécution de l'application

Du côté du serveur, il faut d'abord réaliser le circuit électrique suivant,
donné par l'énoncé du projet :
![Montage](doc/montage.PNG)

Le bipeur doit aussi être branché aux broches 33 et 39.

L'on doit ensuite démarrer les services de la modulation par largeur
d'impulsion (PWM) avec les commandes suivantes :
```
ssh root@192.168.7.2
modprobe pwm-meson
modprobe pwm-ctrl
```

La caméra doit être connecté à la Odroid-C2 avec le cable USB.

On peut ensuite démarrer le serveur :
```
ssh root@192.168.7.2
./server
```

Du côté de l'ordinateur bureau, on peut ensuite lancer le client :
```
./client
```

L'application est censée fonctionner. Pour la tester, vous pouvez prendre
une photo de l'image suivante et vérifier si un son est produit :
![Image de test](/doc/Autumn_leaves-ocr.png)

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

### Livrable 3

26 octobre : fin de livrable 3

Le livrable 3 utilise une interface graphique.Cliquez sur les boutons de la
fenêtre du client pour changer la résolution.

### Livrable 4

9 novembre : fin du librable 4

Le livrable intègre à présent un circuit qui permet (1) d'enregistrer des
images en appuyant sur un bouton et (2) de bloquer la transmission d'images
lorsqu'un détecte photosensible capte une absence de lumière.

### Livrable 5

16 novembre : fin du livrable 5

Le livrable 5 intègre une reconnaissance des caractères dans l'image reçue par
le client. Les caractères reconnus sont affichés dans le terminal de
l'application cliente.

### Livrable 6

30 novembre : fin du livrable 6

Le livrable 6 permet de jouer les partitions avec le bipeur et simplifie le
processus de compilation.
