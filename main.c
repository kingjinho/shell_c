#include <stdio.h>
#include <string.h>
#include <_stdlib.h>

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

    if (!tokens) {
        fprintf(stderr, "lsh: allocation error\n");
        exit(EXIT_FAILURE);
    }

    char *token = strtok(line, LSH_TOK_DELIM);
    while (token != NULL) {
        tokens[position] = token;
        position++;

        if (position >= buf_size) {
            buf_size = (int) realloc(tokens, buf_size * sizeof(char*));

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

static int lsh_execute(char **args) {
    return 0;
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