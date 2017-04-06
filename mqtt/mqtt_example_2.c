#include <signal.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <mosquitto.h>

#define mqtt_host "41.160.245.219"// "srv-uc01.kva.co.za" //"172.18.100.61"
#define mqtt_port 30001//1883

static int run = 1;
const char *topic = NULL;

void handle_signal(int s)
{
	run = 0;
}

void connect_callback(void *obj, int result)
{
	printf("connect callback, rc=%d\n", result);
}

void message_callback(void *obj, const struct mosquitto_message *message)
{
	bool match = 0;
	printf("got message '%.*s' for topic '%s'\n", message->payloadlen, (char*) message->payload, message->topic);

	//mosquitto_topic_matches_sub("myTopic", message->topic, &match);
	//if (match) {
	//	printf("got message for ADC topic\n");
	//}

}
int main(int argc, char *argv[])
{
	uint8_t reconnect = true;
	char clientid[24];
	struct mosquitto *mosq;
	int rc = 0;
	int *info;
	bool clean_session = true;
        bool retain = true;
int qos =2; //0,1,2
int counter =0;
//uint32_t payloadlen = 4;
//const uint8_t *payload;
//char payload_c[4] = "1234";
char msg[10];
char *greet;
greet = "hello";

topic = "myTopic";

	signal(SIGINT, handle_signal);
	signal(SIGTERM, handle_signal);

	mosquitto_lib_init();

	memset(clientid, 0, 24);
	snprintf(clientid, 23, "mysql_log_%d", getpid());
	mosq = mosquitto_new(clientid,info);

	if(mosq)
	{
		mosquitto_connect_callback_set(mosq, connect_callback);
		mosquitto_message_callback_set(mosq, message_callback);

	    	rc = mosquitto_connect(mosq, mqtt_host, mqtt_port, 60,clean_session);

		mosquitto_subscribe(mosq, NULL, topic, qos);

				int snd = mosquitto_publish(mosq, NULL, topic, strlen(greet), greet, qos, retain);
    				if(snd != 0) printf("mqtt_send error=%i\n", snd);

		while(run)
		{
			rc = mosquitto_loop(mosq, -1);
			if(run && rc)
			{
				printf("connection error!\n");
				sleep(10);
				mosquitto_reconnect(mosq);
			}
			counter++;
			sprintf(msg, "%d", counter);
			if((counter % 10) == 0)
			{
				//sprintf(msg, "%d", counter);
				int snd = mosquitto_publish(mosq, NULL, topic, strlen(msg), msg, qos, retain);
    				if(snd != 0) printf("mqtt_send error=%i\n", snd);
			}
		}
		mosquitto_destroy(mosq);
	}

	mosquitto_lib_cleanup();

	return rc;
}
