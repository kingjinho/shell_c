#include <stdio.h>
#include <string.h>
#include <_stdlib.h>
#include <unistd.h>
#include <sys/wait.h>


int lsh_cd(char **args);
int lsh_help(char **args);
int lsh_exit(char **args);

char *builtin_str[] = {
    "cd", "help", "exit"
};

int (*builtin_func[])(char **) = {
    &lsh_cd, &lsh_help, &lsh_exit
};

int lsh_num_builtins() {
    return sizeof(builtin_str) / sizeof(char *);
}

int lsh_cd(char **args) {
    if (args[1] == NULL) {
        fprintf(stderr, "lsh: expected argument to \"cd\"\n");
    } else if (chdir(args[1]) != 0) {
        perror("lsh");
    }
    return 1;
}

int lsh_help(char **args) {
    printf("KingJinho's LSH\n");
    printf("Type program names and arguments, and hit enter.\n");
    printf("the following are built in:\n");

    for (int i = 0; i < lsh_num_builtins(); i++) {
        printf("    %s\n", builtin_str[i]);
    }

    printf("Use the man command for information on other programs.\n");
    return 1;
}

int lsh_exit(char **args) {
    return 0;
}

//read line
#define LSH_RL_BUFSIZE 1024
char *lsh_read_line(void) {
    int buf_size = LSH_RL_BUFSIZE;
    int position = 0;
    char *buffer = malloc(sizeof(char) * buf_size);

    if (!buffer) {
        fprintf(stderr, "lsh: allocation error\n");
        exit(EXIT_FAILURE);
    }

    while (1) {
        //read a character
        //store as an int because EOF is an integer
        const int c = getchar();

        //if we hit eof, replace it with a null character and return
        if (c == EOF || c == '\n') {
            buffer[position] = '\0'; //mark as the end of a string
            return buffer;
        }

        buffer[position] = c;

        position++;

        //reallocate if exceeded the buffer
        if (position >= buf_size) {
            buf_size += LSH_RL_BUFSIZE;
            buffer = realloc(buffer, buf_size);
            if (!buffer) {
                fprintf(stderr, "lsh: allocation error\n");
                exit(EXIT_FAILURE);
            }
        }
    }
}

#define LSH_TOK_BUFSIZE 64
#define LSH_TOK_DELIM " \t\r\n\a"

char **lsh_split_line(char *line) {
    int buf_size = LSH_TOK_BUFSIZE;
    int position = 0;
    char **tokens = malloc(buf_size * sizeof(char *));
    char *token;

    if (!tokens) {
        fprintf(stderr, "lsh: allocation error\n");
        exit(EXIT_FAILURE);
    }

    token = strtok(line, LSH_TOK_DELIM);
    while (token != NULL) {
        tokens[position] = token;
        position++;

        if (position >= buf_size) {
            buf_size = (int) realloc(tokens, buf_size * sizeof(char *));

            if (!tokens) {
                fprintf(stderr, "lsh: allocation error\n");
                exit(EXIT_FAILURE);
            }
        }
        token = strtok(NULL, LSH_TOK_DELIM);
    }
    tokens[position] = NULL;
    return tokens;
}

// exec, wait
int lsh_launch(char **args) {
    pid_t pid, wpid;
    int status;

    // fork process
    // two processes running concurrently
    pid = fork();
    if (pid == 0) {
        //child process
        if (execvp(args[0], args) == -1) {
            perror("lsh");
        }
        exit(EXIT_FAILURE);
    }

    //fork() error check
    if (pid < 0) {
        //error forking
        perror("lsh");
    } else {
        //parent process
        do {
            //wait for the process's state to change
            wpid = waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }

    return 1;
}

// start the process
int lsh_execute(char **args) {
    if (args[0] == NULL) {
        //empty command
        return 1;
    }

    for (int i = 0;i < lsh_num_builtins(); i++) {
        if (strcmp(args[0], builtin_str[i]) == 0) {
            return (*builtin_func[i])(args);
        }
    }

    return lsh_launch(args);
}


//interpret: Read, Parse and Execute
static void lsh_loop() {
    int status;

    do {
        printf("> ");
        char *line = lsh_read_line();
        char **args = lsh_split_line(line);
        status = lsh_execute(args);

        free(line);
        free(args);
    } while (status);
}

int main(int argc, char **argv) {
    //init - read config

    //interpret - read commands and executes
    lsh_loop();
    //terminate - shutdown commands, frees up memory, terminates

    return EXIT_SUCCESS;
}
