void server(){
	//setup a socket structure
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = inet_addr("192.168.0.250");
	server.sin_port = htons(4242);

	//bind the socket
	bind(sockfd, (struct sockaddr*)&server, sizeof(server));
	connect(sockfd, (struct sockaddr*)&server, sizeof(server));

	char *message = "What do you get when you multiply six by nine?";
	write(sockfd, &message, sizeof(message));
	printf("Message: '%s' sent.\n", message);

	char *recieved;
	read(sockfd, &recieved, sizeof(recieved));
	printf("Message: '%s' recieved.\n", recieved);

	//cleanup
	close(sockfd);
	return 0;
}
