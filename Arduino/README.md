Ce programme fait partie du programme Arudino, qui comprend des fonctions telles que le changement de mode, 
la réception des instructions du PC, l'envoi de l'état du robot et le suivi.

1. Avant utilisation, vous devez modifier le compte et le mot de passe WIFI, ainsi que l'adresse IP du serveur MQTT.

2. L'environnement de développement dans lequel ce programme s'exécute est Arduino Uno.

3. Méthode d'essai indépendante: 
   1) Tout d'abord, téléchargez le moustique sur https://mosquitto.org/.
   2) Exécuter dans le répertoire installé: mosquitto -v pour ouvrir le serveur.
   3) Ensuite on exécute: mosquitto_pub -t "robot" -m "1" pour contrôler le robot, allez au tableau 1.
   4) On peut également passer en mode Bluetooth en envoyant l'instruction mosquitto_pub -t "robot" -m "B".
   
4. Nous pouvons combiner le programme MQTT côté PC avec le programme d'Arduino. 
   Cela signifie que le robot enverra automatiquement l'état au PC, puis attendra que le PC envoie des instructions de contrôle.

