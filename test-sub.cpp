/*
 * test-sub.c
 * Part of the mosquito-test demonstration application
 * Consumes messages from an MQTT topic indefinitely
 * Copyright (c)2016 Kevin Boone. Distributed under the terms of the 
 *  GPL v3.0 
 */
#include <iostream>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mosquitto.h>
#include <mysql.h>
#include <my_global.h>
#include <string>

// Server connection parameters
#define MQTT_HOSTNAME "localhost" 
#define MQTT_PORT 1883
#define MQTT_USERNAME "admin"
#define MQTT_PASSWORD "admin"
#define MQTT_TOPIC "test"

#define SERVER "localhost"
#define USERNAME "root"
#define PASSWORD "ncc1p8"
#define DATABASE "mysqlmqtt"
using namespace std;
/*
 * my_message_callback. 
 * Called whenever a new message arrives
 */
void my_message_callback(struct mosquitto *mosq, void *obj, const struct mosquitto_message *message)
{
// Note: nothing in the Mosquitto docs or examples suggests that we
//  must free this message structure after processing it.
	string query, pay;
	pay = (char *)message->payload;
	printf ("Got message: %s\n", pay.c_str());
	
	MYSQL *connect;
    connect = mysql_init(NULL);

    if (connect){
        cout << "Connection Established Successfully......." << endl;
    }

    connect = mysql_real_connect(connect, SERVER, USERNAME, PASSWORD, DATABASE, 0,NULL,0);

    if (connect){
        cout << "Connection Established Successfully......." << endl;
        //cout << "Your Name is " << empFirstName << endl;
    }

    query = "INSERT INTO Emp (empName, suhu, tinggi) VALUES('"+pay+"', '20', '20')";

    cout << query << endl;

    if (mysql_query(connect, query.c_str())){
        cout << "Success.... \n" << endl;
    }   

    mysql_close (connect);

}

/*
 * Start here
 */
int main (int argc, char **argv)
{
	struct mosquitto *mosq = NULL;


	// Initialize the Mosquitto library
	mosquitto_lib_init();

	// Create a new Mosquito runtime instance with a random client ID,
	//  and no application-specific callback data.  
	mosq = mosquitto_new (NULL, true, NULL);
	if (!mosq)
	{
		fprintf (stderr, "Can't init Mosquitto library\n");
		exit (-1);
	}

	// Set up username and password
	mosquitto_username_pw_set (mosq, MQTT_USERNAME, MQTT_PASSWORD);

	// Establish a connection to the MQTT server. Do not use a keep-alive ping
	int ret = mosquitto_connect (mosq, MQTT_HOSTNAME, MQTT_PORT, 0);
	if (ret)
	{
		fprintf (stderr, "Can't connect to Mosquitto server\n");
		exit (-1);
	}

	// Subscribe to the specified topic. Multiple topics can be
	//  subscribed, but only one is used in this simple example.
	//  Note that we don't specify what to do with the received
	//  messages at this point
	ret = mosquitto_subscribe(mosq, NULL, MQTT_TOPIC, 0);
	if (ret)
	{
		fprintf (stderr, "Can't publish to Mosquitto server\n");
		exit (-1);
	}

	// Specify the function to call when a new message is received
	mosquitto_message_callback_set (mosq, my_message_callback);

	// Wait for new messages
	mosquitto_loop_forever (mosq, -1, 1);

	// Tody up. In this simple example, this point is never reached. We can
	//  force the mosquitto_loop_forever call to exit by disconnecting
	//  the session in the message-handling callback if required.
	mosquitto_destroy (mosq);
	mosquitto_lib_cleanup();

	return 0;
}

