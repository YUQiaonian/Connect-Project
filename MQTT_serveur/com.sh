gcc -g -Wall -o server server.c mymqtt.c mysqlite.c -lsqlite3 -L/home/jiang/MQTT/paho.mqtt.c-master/build/src -lpaho-mqtt3c -I/home/jiang/MQTT/paho.mqtt.c-master/src

gcc -g -Wall -o app app.c mysqlite.c -lsqlite3 -lsqlite3
