#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <pthread.h>


#define BLKSIZE 1024

int readline(int fd, char *buf, int nbytes);
int copyfile(int fromfd, int tofd);
char* IPToURL(char* url);



char* IPToURL(char* url) {
    char* IPbuf;
    struct hostent* host_entry;

    if ((host_entry = gethostbyname(url)) == NULL) {
        perror("Could not resolve address");
        exit(1);
    }

    IPbuf = inet_ntoa(*((struct in_addr*) host_entry->h_addr_list[0]));

    return IPbuf;
}


int monitor() {
   int bytesread;
   pid_t childpid;
   int fd, fd1, fd2;

    if ((fd1 = open("Docs/aspirations.odt", O_RDONLY)) == -1) {
        fprintf(stderr, "Failed to open file:%s\n", strerror(errno));
        return 1;
    }

    if ((fd2 = open("Docs/goals.odt", O_RDONLY)) == -1) {
        fprintf(stderr, "Failed to open file:%s\n", strerror(errno));
        return 1;
    }

    if ((childpid = fork()) == -1) {
        perror("Failed to create child process");
        return 1;
    }

    if (childpid > 0) 
        fd = fd1;
    else 
        fd = fd2;

    bytesread = copyfile(fd, STDOUT_FILENO);
    fprintf(stderr, "Bytes read %d\n", bytesread);
    return 0;
}



void printaccess(const char *path) {
    struct stat statbuf;

    if (stat(path, &statbuf) == -1)
        perror("Failed to get file status");
    else
        printf("%s last accessed at %s", path, ctime(&statbuf.st_atime));
}

int isdirectory(const char *path) {
    struct stat statbuf;

    if (stat(path, &statbuf) == -1)
        return 0;
    else 
        return S_ISDIR(statbuf.st_mode);
}



int copyfile(int fromfd, int tofd) {
    char *bp;
    char buf[BLKSIZE];
    int bytesread, byteswritten;
    int totalbytes = 0;

    for ( ; ; ) {
        while ((bytesread = read(fromfd, buf, BLKSIZE)) ==  -1 
            && (errno == EINTR))    ;
            
        if (bytesread <= 0)
            break;

        bp = buf;
        while (bytesread > 0) {
            while ((byteswritten = write(tofd, bp, bytesread)) == -1
                    && (errno == EINTR))   ;

            if (byteswritten <= 0)
                break;

            totalbytes += byteswritten;
            bytesread -= byteswritten;
            bp += byteswritten;
        }

        if (byteswritten == -1)
            break;
    }

    return totalbytes;
}

int readline(int fd, char *buf, int nbytes) {
    int numread = 0;
    int returnval;

    while (numread < nbytes - 1) {
        returnval = read(fd, buf + numread, 1);

        if ((returnval == -1) && (errno == EINTR)) 
            continue;
        if ((returnval == 0) && (numread == 0))
            return 0;
        if (returnval == 0)
            break;
        if (returnval == -1)
            return -1;

        numread++;
        if (buf[numread-1] == '\n') {
            buf[numread] = '\0';
            return numread;
        }
    }

    errno = EINVAL;
    return -1;
}

// void dummy(void) {
//     pid_t childpid;
//     int i, n;

//     childpid = fork();

//     if (childpid == -1) {
//         perror("Failed to fork");
//     }

//     if (childpid == 0) {
//         if (setsid() == -1)
//             perror("Child failed to become a session leader");
//         else {
//             fprintf(stderr, "I am a session leader\n");
//             execl("/bin/ls", "ls", "-1", NULL);
//         }

//         return 1;
//     }

//     if (childpid != wait(NULL)) {
//         perror("Parent failed to wait due to signal or error");
//         return 1;
//     }
// }



pid_t r_wait(int *stat_loc) {
    int retval;

    while((retval = wait(stat_loc) && (errno == EINTR))) ;

    return retval;
}

void show_return_status(void) {
    pid_t childpid;
    int status;

    childpid = r_wait(&status);

    if (childpid == -1) 
        perror("Failed to wait for child");
    else if (WIFEXITED(status) && !WEXITSTATUS(status))
        printf("Child %ld terminated normally\n", (long)childpid);
}
