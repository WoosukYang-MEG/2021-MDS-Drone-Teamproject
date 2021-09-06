#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <string.h>
#include <pthread.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <wiringPi.h>

#define DEVICE_MAX_NUM 4

#define LED 0
#define SERVO 1
#define BUZZER 2
#define ULTRA 3

const int melody[] = { 262, 294, 330, 349, 392, 440, 494, 523 };
const int servo_pin = 1;
const int led_pin = 2;
const int buzzer_pin = 25;
const int ultra_trig_pin = 23;
const int ultra_echo_pin = 24;

int ultra_print_flag = 0;

void printids(const char *s){

	pid_t pid;
	pthread_t tid;
	pid = getpid();
	tid = pthread_self();
	
	printf("%s pid %u tid %u (0x%x)\n", s, 
	(unsigned int)pid, (unsigned int) tid, (unsigned int) tid);
}

void *thr_led(void *arg)
{
	printids("led  thread : ");
	while(1)
	{
		digitalWrite(led_pin,HIGH);
		delay(100);
		digitalWrite(led_pin,LOW);
		delay(100);
	}
	return ((void *)0);
}

void *thr_servo(void *arg)
{
	int cnt;

	printids("servo thread : ");
	while(1){
		for(cnt=0; cnt<3; cnt++){
			pwmWrite(servo_pin, 600);
			delay(500);
			pwmWrite(servo_pin, 1500);
			delay(500);
		}
	}

	//pwmWrite(servo_pin,1);

	return ((void*)0);
}

void *thr_buzzer(void *arg)
{
	int i;

	printids("buzzer thread ; ");
	while(1)
	{
	 	for(i=0; i<8; i++)
	 	{
			pwmSetRange(1000000/melody[i]);
			pwmWrite(buzzer_pin, 1000000/melody[i]/2);
			delay(1000);
	 	}
	}

	return ((void *)0);
}

void *thr_ultrasonic(void *arg)
{
    int s_time, e_time;
    float distance;

	printids("ultra sonic thread ; ");
    while(1)
    {
	if(ultra_print_flag == 1)
	{
	    printf("upf: %d\n", ultra_print_flag);
	    digitalWrite(ultra_trig_pin, LOW);
	    delay(500);
    
	    digitalWrite(ultra_trig_pin, HIGH);
	    delayMicroseconds(10);
    
	    digitalWrite(ultra_trig_pin, LOW);

	    while(digitalRead(ultra_echo_pin) == 0);
	    s_time = micros();

	    while(digitalRead(ultra_echo_pin) == 1);
	    e_time = micros();

	    distance = (e_time - s_time) / 29. / 2.;

	    printf("D: %.2f cm\n", distance);
	}
    }

    return ((void*)0); 
}



int main(int argc, char *argv[])
{
	pthread_t ntid[5];
	void* (*fn[])(void *) ={thr_led, thr_servo, thr_buzzer, thr_ultrasonic};

	int act_cnt[DEVICE_MAX_NUM] = {0, };
	int c_socket;
	struct sockaddr_in c_addr;
	int	n;
	char rcvBuffer[BUFSIZ];
    int i;

	if(argc != 3)
	{
		fprintf(stderr, "[Usage] %s ip_address port_number\n", argv[0]);
		exit(1);
	}
	
	c_socket = socket(PF_INET, SOCK_STREAM, 0);
	
	memset(&c_addr, 0, sizeof(c_addr));
	c_addr.sin_family = AF_INET;
	c_addr.sin_addr.s_addr = inet_addr(argv[1]);
	c_addr.sin_port = htons(atoi(argv[2]));
	
	if(connect(c_socket, (struct sockaddr *) &c_addr, sizeof(c_addr))==-1)
	{
		printf("Can not connect\n");
		close(c_socket);
		return -1;
	}

	wiringPiSetup();

	while(1)
	{
		if((n = read(c_socket, rcvBuffer, BUFSIZ)) < 0)
		{
		        break;
		}
		else
		{
			rcvBuffer[n] = '\0';
			printf("Command From Command Center: %s\n", rcvBuffer);
			if((strncmp(rcvBuffer, "led", 3)) == 0)
			{
	        printf("led action on\n");
				pinMode(led_pin, OUTPUT);

				if(act_cnt[LED] == 0)
				{
					pthread_create(&ntid[LED], NULL, fn[LED], NULL);
				}
				act_cnt[LED]++;
			}
			else if((strncmp(rcvBuffer, "servo", 5)) == 0)
			{
	        printf("servo action on\n");
				pinMode(servo_pin, PWM_OUTPUT);
				pwmSetClock(19);
				pwmSetMode(PWM_MODE_MS);
				pwmSetRange(20000);
				pwmWrite(servo_pin, 600);
				delay(1000);
				if(act_cnt[SERVO] == 0)
				{
					pthread_create(&ntid[SERVO], NULL, fn[SERVO], NULL);
				}
				act_cnt[SERVO]++;
			}
			else if((strncmp(rcvBuffer, "buzzer", 6)) == 0)
			{
	        printf("buzzer action on\n");
				pinMode(buzzer_pin, PWM_OUTPUT);
				pwmSetClock(19);
				pwmSetMode(PWM_MODE_MS);
				if(act_cnt[BUZZER] == 0)
				{
					pthread_create(&ntid[BUZZER], NULL, fn[BUZZER], NULL);
				}
				act_cnt[BUZZER]++;
			}
			else if((strncmp(rcvBuffer, "ultra", 5)) == 0)
			{
	        printf("ultrasonic action on\n");
	        pinMode(ultra_trig_pin, OUTPUT);
	        pinMode(ultra_echo_pin, INPUT);
	        ultra_print_flag = 1;
	        
				if(act_cnt[ULTRA] == 0)
				{
					pthread_create(&ntid[ULTRA], NULL, fn[ULTRA], NULL);
				}
				act_cnt[ULTRA]++;
			}
			else if((strncmp(rcvBuffer, "s-led", 5)) == 0)
			{
	        printf("stop led action\n");
				digitalWrite(led_pin,LOW);
				delay(100);
				pinMode(led_pin, INPUT);
			}
			else if((strncmp(rcvBuffer, "s-servo", 7)) == 0)
			{
	        printf("stop servo action\n");
				pwmWrite(servo_pin, 600);
				delay(500);
				pinMode(servo_pin, INPUT);
			}
			else if((strncmp(rcvBuffer, "s-buzzer", 8)) == 0)
			{
	        printf("stop buzzer action\n");
				pinMode(buzzer_pin, INPUT);
			}
			else if((strncmp(rcvBuffer, "s-ultra", 7)) == 0)
			{
	        printf("stop ultrasonic action\n");
	        ultra_print_flag = 0;
			}
		}
	}

    for(i=0; i<DEVICE_MAX_NUM; i++)
    {
		pthread_join(ntid[i], NULL);
    }
	close(c_socket);
	exit(0);
}
