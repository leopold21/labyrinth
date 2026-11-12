# Le jeu du Labyrinthe

## Présentation

### Menu principal et fonctionnalités générales
Le programme propose un jeu de labyrinthe offrant quatre actions principales à l'utilisateur :

- Créer un nouveau labyrinthe.

- Visualiser un labyrinthe existant.

- Jouer sur un labyrinthe déjà créé.

- Quitter le jeu.

### Création et sauvegarde d'un labyrinthe
Pour générer un labyrinthe, l'utilisateur suit les étapes suivantes :

- Il choisit les dimensions du labyrinthe (en respectant les contraintes imposées)
- Selectionne le niveau de dificulté:
    - Facile : il n'existe qu'un seul chemin possible entre le départ et l'arrivée.
    - Difficile : plusieurs chemins mènent à la sortie.
- L'utilisateur saisit son nom (enregistré dans le fichier pour identifier le créateur) ainsi que le nom donné au labyrinthe. Le fichier est ensuite sauvegardé dans le dossier "The models labyrinths".

### Visualisation et chargement
L'utilisateur peut parcourir la liste des labyrinthes disponibles afin de les afficher à l'écran.

#### Objectif 
Rejoindre la sortie en un minimum de coups, tout en maximisant son score grâce aux bonus et en évitant les malus et les monstres (fantômes et ofres). Pour franchir la sortie, le joueur doit impérativement récupérer la clé (un indicateur à l'écran précise si le joueur la possède ou non).

### Légende et éléments du jeu
o : Le joueur.

\- : La sortie (l'arrivée).

K : La clé qui permet de sortir du labyrinthe.

X : Les bonus (augmentent le score).

Y : Les malus (diminuent le score).

F : Les fantômes (monstres mobiles capables de traverser les murs, qui font perdre des points au contact).

O : Les ogres (monstres mobiles qui restent à proximiter du trésor, qui font perdre des points au contact).

Informations utiles au cours de la partie : Le scores des bonus, malus et monstres, le nombre de coups et si vous possédez la clé ou non sont affichés en permanence.

#### Calcul du score et classement

Score final : À la fin de la partie, le score total est calculé en additionnant les points récoltés en jeu (bonus, malus et fantômes) et un nombre lié au nombre de coups (moins le joueur effectue de déplacements, plus il gagne de points).

Top 10 : Si le score final intègre le top 10 du labyrinthe, le joueur saisit son nom pour l'enregistrer dans le classement, associé au labyrinthe. Le classement est ensuite affiché pour que le joeur puisse voir son vrai classement. De plus, il est sauvegardé dans le répertoire "Ranking".

## Lancement du jeu
- Ouvrer  un terminal
- Placez-vous à la racine du projet (répétopire "labyrinth") 
- Compiler le programme: "gcc -Wall *.c -o main"
- Lancer l'exécutable:  "./main" 
- A vous de jouer!

## Membre du projet
- BEUVE Léopold 