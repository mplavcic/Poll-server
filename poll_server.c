#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <poll.h>


#define PORT "9034"  


void *get_in_addr(struct sockaddr *sa) {
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}
	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int get_listener_socket(void) {
	
	struct addrinfo hints, *res, *p;
    	int status;
    	int yes = 1;
    	int listener;

    	memset(&hints, 0, sizeof(hints));
    	hints.ai_family = AF_UNSPEC;
    	hints.ai_socktype = SOCK_STREAM;
    	hints.ai_flags = AI_PASSIVE;

    	if ((status = getaddrinfo(NULL, PORT, &hints, &res)) != 0) {
		fprintf(stderr, "pollserver: %s\n", gai_strerror(status));
        	exit(1);
    	}
	
	for (p = res; p != NULL; p = p->ai_next) {
		listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
		if (listener < 0) { 
            	continue;
		}
        
        setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

        if (bind(listener, p->ai_addr, p->ai_addrlen) < 0) {
		close(listener);
		continue;
        }

        break;
    }

    freeaddrinfo(res); 

    if (p == NULL) {
        return -1;
    }

    if (listen(listener, 10) == -1) {
        return -1;
    }

    return listener;

}

void add_to_pfds(struct pollfd *pfds[], int newfd, int *fd_count, int *fd_size) {
	
	if (fd_count == fd_size) {
		*fd_size *= 2;
		*pfds = realloc(*pfds, *fd_size * sizeof(**pfds));
		}
	
	(*pfds)[*fd_count].fd = newfd;
	(*pfds)[*fd_count].events = POLLIN;

	(*fd_count)++;
}


int main(void) {

	int listener, newfd;
	struct sockaddr_storage remoteaddr;
	socklen_t addrlen;
	char remoteIP[IP6_ADDRSTRLEN]
	char buf[256];
	
	int fd_size = 5;
	int fd_count = 0;
	struct pollfd *pfds = malloc(fd_size * sizeof(struct pollfd));

	listener = get_listener_socket();
	
	if (listener == -1) {
		fprintf(stderr, "error getting listener socket\n");
		exit(1);
	}

	pfds[0].fd = listener;
	pfds[0].events = POLLIN;

	fd_count = 1;

	for (;;) {
		int poll_count = poll(pfds, fd_count, -1);
		if (poll_count == -1) {
			perror("poll");
			exit(1);
		}
		for (i = 0; i < fd_count; i++) {
			if (pfds[i].revents & POLLIN) {
				if (pfds[i].fd == listener) {
					addrlen = sizeof(remoteaddr);
					newfd = accept(listener, (struct sockaddr *) &remoteaddr, &addrlen);
					if (newfd == -1) {
						perror("accept");
					} else {
						add_to_pfds(pfds, newfd, *fd_count, *fd_size)
							printf("pollserver: new connection from %d on socket %d",
									inet_ntop(remoteaddr.ss_family,
									get_in_addr((struct sockaddr *) &remoteaddr), remoteIP, IP6_ADDRSTRLEN)
								,newfd);
					}
					
				} else {
						
					int nbytes = recv(pfds[i].fd, *buf, sizeof(buf), 0);
					int sender_fd = pfds[i].fd;

					if (nbytes <= 0)	

				}
			}
		} 
	}
	
	return 0;
}




















