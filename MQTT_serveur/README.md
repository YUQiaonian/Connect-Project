Cette partie du programme est un programme côté PC, utilisé pour créer une base de données, 
recevoir l'état du robot via MQTT et envoyer des instructions de contrôle pour contrôler le robot.

1. Change the ADDRESS macro definition in mymqtt.h to your own IP address.
2. Run the com.sh file to compile the app and server programs
3. Open a terminal and enter mosqutto -v to run the mosquitto server
4. Run the server program, connect to the MQTT server, and create a database.
5. The client runs the app, enters the table number, menu number, and enters 0 to confirm the order.
