#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <wiringPi.h>

int main(int argc, char *argv[])
{
	int s_socket, c_socket;
	struct sockaddr_in s_addr, c_addr;
	int len;

	if(argc != 2)
	{
		fprintf(stderr, "[Usage] %s port_number\n", argv[0]);
		exit(1);
	}

	s_socket = socket(PF_INET, SOCK_STREAM, 0);

	memset(&s_addr, 0, sizeof(s_addr));
	s_addr.sin_family = AF_INET;
	s_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	s_addr.sin_port = htons(atoi(argv[1]));

	if(bind(s_socket, (struct sockaddr *) &s_addr, sizeof(s_addr)) == -1)
	{
		printf("Can not Bind\n");
		return -1;
	}

	if(listen(s_socket, 5) == -1)
	{
		printf("listen Fail\n");
		return -1;
	}

	len = sizeof(c_addr);
	c_socket = accept(s_socket, (struct sockaddr *) &c_addr, &len);

	while(1)
	{
		int 	n;
		char buffer[BUFSIZ] ;
	
		while(1)
		{	
			if((n=read(0, buffer, BUFSIZ)) > 0)
			{
				write(c_socket, buffer, n);
				
				if((strncmp(buffer, "exit", 4)) == 0)
				{
					break;
				}
			}
		}
	}

	close(c_socket);
	close(s_socket);
}