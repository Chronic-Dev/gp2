#define MAC_OS_X_VERSION_MIN_REQUIRED MAC_OS_X_VERSION_10_5

#include "commands.h"

void disable_watchdog ( );
void init_tcp ( );
void init_usb ( );

int parse_command(char *command, int wfd) {
	int i;
	char *cmd = NULL;
	char **args = NULL;
	char **tmpargs;
	int argc = 0;
	char *tmp;
	
	tmp = strtok(command, " ");
	while (tmp != NULL) {
		printf("%s\n", tmp);
		if (cmd == NULL) {
			cmd = malloc(sizeof(char) * strlen(tmp));
			strcpy(cmd, tmp);
		} else {
			argc++;
			tmpargs = malloc(sizeof(char *) * argc);
			tmpargs[(argc - 1)] = malloc(sizeof(char) * strlen(tmp));
			strcpy(tmpargs[(argc - 1)], tmp);
			for (i = 0; i < (argc - 1); i++) {
				if (args[i] != NULL) {
					tmpargs[i] = malloc(sizeof(char) * strlen(args[i]));
					strcpy(tmpargs[i], args[i]);
					free(args[i]);
				}
			}
			if (args != NULL)
				free(args);
			args = tmpargs;
		}
		tmp = strtok(NULL, " ");
	}

	if (!strcmp(cmd, "exit")) {
		return 1;
	} else if (!strcmp(cmd, "put")) {
		return cmd_put(wfd, argc, args);
	} else if (!strcmp(cmd, "get")) {
		return cmd_get(wfd, argc, args);
	} else if (!strcmp(cmd, "mod")) {
		return cmd_mod(wfd, argc, args);
	} else {
		//Loop through loaded modules
		for (i = 0; i < gModc; i++) {
			isUsableCommand_t _isUsableCommand = (isUsableCommand_t)dlsym(gMods[i], "isUsableCommand");
			if (_isUsableCommand(cmd)) {
				processCommand_t _processCommand = (processCommand_t)dlsym(gMods[i], "processCommand");
				return _processCommand(wfd, cmd, argc, args);
			}
		}
		//command not found
		printf("Command '%s' not found\n", cmd);
	}
	return 0;
}

int socket_listen(void) {
	struct sockaddr_in local_addr, remote_addr;
	fd_set master, read_fds;
	int listener, fdmax, yes = 1, i;
	struct timeval tv;
	int port = 7;
	int do_listen = 1;
	int ret;

	FD_ZERO(&master);
	FD_ZERO(&read_fds);

	listener = socket(AF_INET, SOCK_STREAM, 0);
	setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

	memset(&local_addr, 0, sizeof(struct sockaddr_in));
	local_addr.sin_family = AF_INET;
	local_addr.sin_port = htons(port);
	local_addr.sin_addr.s_addr = INADDR_ANY;

	i = bind(listener, (struct sockaddr *)&local_addr, sizeof(struct sockaddr));
	if (i) {
		printf("ERROR: bind() returned %d: %s\n", i, strerror(errno));
		return i;
	}

	i = listen(listener, 8);
	if (i) {
		printf("ERROR: listen() returned %d: %s\n", i, strerror(errno));
		return i;
	}

	FD_SET(listener, &master);
	fdmax = listener;

	printf("daemon now listening on TCP:%d.\n", port);

	while (do_listen) {
		read_fds = master;
		tv.tv_sec = 2;
		tv.tv_usec = 0;

		if (select(fdmax+1, &read_fds, NULL, NULL, &tv)>0) {
			for(i=0; i<=fdmax; i++) {
				if (FD_ISSET(i, &read_fds)) {
					if (i == listener) {
						int newfd;
						int addrlen = sizeof(remote_addr);
						int parsecmds = 1;
						if ((newfd = accept(listener, (struct sockaddr *)&remote_addr, (socklen_t *)&addrlen)) == -1) {
							continue;
						}
						setsockopt(newfd, SOL_SOCKET, TCP_NODELAY, &yes, sizeof(int));
						setsockopt(newfd, SOL_SOCKET, SO_NOSIGPIPE, &yes, sizeof(int));
						//ret = send_data(newfd);
						char buf[300];
						char c;
						int i;
						while (parsecmds) {
							i = 0;
							do {
								c = getc(newfd);
								buf[i] = c;
								i++;
							} while (i < 50 && c != '\n');
							buf[(i - 1)] = '\0';
							ret = parse_command(buf, newfd);
							if (ret != 0)
								parsecmds = 0; //Exit
						}
						close(newfd);
						if (!ret)
							do_listen = 0;
					}
				} /* if FD_ISSET ... */
			} /* for(i=0; i<=fdmax; i++) */
		} /* if (select(fdmax+1, ... */
	} /* for(;;) */

	printf("rebooting device in 10 seconds.\n");
	sleep(10);
	return 0;
}

int main(int argc, char* argv[]) {
	printf("payload compiled " __DATE__ " " __TIME__ "\n");
	gMods = NULL;
	gModc = 0;
	disable_watchdog();
	printf("watchdog disabled.\n");

	init_tcp();
	init_usb();
	printf("usbmux initialized\n");

	return socket_listen();
}

