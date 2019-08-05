#include <iostream>
#include <string>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_LINE	100

int main(int argc, char* argv[])
{
	struct sockaddr_in serveraddr;
	short serverport = 9981;

	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	serveraddr.sin_port = htons(serverport);

	if(0 > connect(sockfd, (sockaddr*)&serveraddr, sizeof(struct sockaddr_in)))
	{
		perror("connect fail!");
		exit(EXIT_FAILURE);
	}

	std::string data = "12345678";
	char line[MAX_LINE];

	for(int i = 0; i < 10; i++)
	{		
		std::string senddata(data);
		//senddata.append(std::to_string(i).c_str());
		senddata.append("111");
		int len = write(sockfd, data.c_str(), data.size());
		//int len = send(sockfd, data.c_str(), data.size(), 0);
		if(len == data.size())
		{
			std::cout << "write finish" << std::endl;
			//shutdown( sockfd, 2 );
			std::cout << "bye bye" << std::endl;

			
			bzero(line, MAX_LINE);
			read(sockfd, line, MAX_LINE);
			std::cout << "read from server [" << line << "]"<<std::endl;
			sleep(1);

		}
	}



	return 0;
}
