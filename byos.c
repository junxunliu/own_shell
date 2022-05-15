#include "byos.h"
// Find out what other #include's you need! (E.g., see man pages.)
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/wait.h>
#include <signal.h>
#include <unistd.h>

int interp(const struct cmd *c)
{
    int fd_out = STDOUT_FILENO;
    int saved_stdout;
    saved_stdout = dup(1); // STDOUT_FILENO

    int ret = 0;

    if (c -> redir_stdout) {
       if ((fd_out = open(c -> redir_stdout, O_WRONLY | O_TRUNC | O_CREAT, 0666)) == -1) {
           perror("open");
           return 1;
       }
    }

    // echo
    if (c -> type == ECHO) {
        char *arg = c -> data.echo.arg;
        int total = strlen(arg);
        int bytes;
        while (total > 0) {
            if (total > 1024) {
                bytes = 1024;
            }
            else {
                bytes = total;
            }
            if (write(fd_out, arg, bytes) != bytes) {
                perror("write");
                return 1;
            }
            total = total - bytes;
            arg = arg + bytes;
        }
    }

    // forx
    else if (c -> type == FORX) {
        pid_t pid = fork();

        // child
        if (pid == 0) {
            dup2(fd_out, STDOUT_FILENO);
            if (execvp(c -> data.forx.pathname, c -> data.forx.argv) == -1) {
                fprintf(stderr, "fail running exec()");
                exit(127); // Asked by assignment 
            }
        }

        // parent
        else {
            int status;
            wait(&status);
            if (WIFEXITED(status)) {
                ret =  WEXITSTATUS(status);
            }
            else {
                ret = 128 + WTERMSIG(status);
            }
        }
    }
    
    // ls
    else {
        ret = 0;
        dup2(fd_out, STDOUT_FILENO);
        for (int i = 0; i < c -> data.list.n; i++) {
            ret = interp(c -> data.list.cmds + i);
            if (ret == 128 + SIGINT) {
                break;
            }
        }
    }

    if (fd_out != STDOUT_FILENO) {
        if(close(fd_out) == -1) {
            perror("close");
            return 1;
        }
    }

    // restore stdout
    /* pilcrows. C restore stdout to Terminal. Stack Overflow. 
    from https://stackoverflow.com/questions/11042218/c-restore-stdout-to-terminal */
    dup2(saved_stdout, 1);
    close(saved_stdout);

    return ret;
}
