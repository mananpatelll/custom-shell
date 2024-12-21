#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <error.h>
#include <fcntl.h>

// Define constants

#define MAX_COMMAND_LEN 1024 // lenght of max command
#define MAX_ARGS 64          // Maximum arguments for command

void parse_input(char *input, char **args)
{
    char *token = strtok(input, " \n"); // splits input by space and remove new line
    int index = 0;                      // to keep track of index
    while (token != NULL && index < MAX_ARGS - 1)
    {                                // Runs while loop until token is not null or index size is less than max args
        args[index++] = token;       // Add token into array
        token = strtok(NULL, " \n"); // Continue tokenizing input
    }
    args[index] = NULL; // Last index as null
}

int handle_redirection(char **args)
{
    int i = 0;
    while (args[i] != NULL)
    {
        if (strcmp(args[i], ">") == 0)
        { // Output redirection
            if (args[i + 1] == NULL)
            { // Missing file
                fprintf(stderr, "syntax error: expected file after >\n");
                return -1;
            }
            int fd = open(args[i + 1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (fd == -1)
            {
                perror("open failed");
                return -1;
            }
            dup2(fd, STDOUT_FILENO); // Redirect stdout to file
            close(fd);
            args[i] = NULL; // Remove redirection from args
            return 0;
        }
        if (strcmp(args[i], "<") == 0)
        { // Input redirection
            if (args[i + 1] == NULL)
            { // Missing file
                fprintf(stderr, "syntax error: expected file after <\n");
                return -1;
            }
            int fd = open(args[i + 1], O_RDONLY);
            if (fd == -1)
            {
                perror("open failed");
                return -1;
            }
            dup2(fd, STDIN_FILENO); // Redirect stdin from file
            close(fd);
            args[i] = NULL; // Remove redirection from args
            return 0;
        }
        i++;
    }
    return 0; // No redirection
}

int contains_pipes(char *input, char **command1, char **command2)
{
    // Function to check is command contains pipe
    char *pipe_position = strchr(input, '|'); // Find pipe
    if (pipe_position == NULL)
    {
        return 0; // No pipe found
    }
    *pipe_position = '\0';
    parse_input(input, command1);             // Parse first command
    parse_input(pipe_position + 1, command2); // Parse second command
    return 1;
}

void execute_piped_command(char **command1, char **command2)
{
    int pipefd[2];
    if (pipe(pipefd) == -1)
    {
        perror("pipe failed");
        return;
    }

    pid_t pid1 = fork();
    if (pid1 < 0)
    {
        perror("fork failed");
        return;
    }

    if (pid1 == 0)
    {                                   // First child for command1
        close(pipefd[0]);               // Close unused read end
        dup2(pipefd[1], STDOUT_FILENO); // Redirect stdout to pipe write end
        close(pipefd[1]);               // Close write end after redirecting
        if (execvp(command1[0], command1) == -1)
        {
            perror("command execution failed");
        }
        exit(1);
    }

    pid_t pid2 = fork();
    if (pid2 < 0)
    {
        perror("fork failed");
        return;
    }

    if (pid2 == 0)
    {                                  // Second child for command2
        close(pipefd[1]);              // Close unused write end
        dup2(pipefd[0], STDIN_FILENO); // Redirect stdin to pipe read end
        close(pipefd[0]);              // Close read end after redirecting
        if (execvp(command2[0], command2) == -1)
        {
            perror("command execution failed");
        }
        exit(1);
    }

    // Parent process
    close(pipefd[0]); // Close both ends of the pipe
    close(pipefd[1]);
    waitpid(pid1, NULL, 0); // Wait for both children
    waitpid(pid2, NULL, 0);
}

void execute_command(char **args)
{
    pid_t pid = fork(); // creates sub process
    if (pid < 0)
    {
        perror("fork failed"); // give error if fork failed
        return;
    }
    if (pid == 0)
    {
        if (handle_redirection(args) == -1)
        { // Handle redirection
            exit(1);
        } // fork returns 0 in child process
        if (execvp(args[0], args) == -1)
        {
            perror("command execution failed"); // prints error if execvp failed
        }
        exit(1);
    }
    else
    {
        waitpid(pid, NULL, 0); // Waits for child process to complete
    }
}

int main()
{
    char input[MAX_COMMAND_LEN];
    char *args[MAX_ARGS];

    char *command1[MAX_ARGS];
    char *command2[MAX_ARGS];

    printf("*******************************************\n");
    printf("*          Welcome to Manan's Shell       *\n");
    printf("*       Type 'exit' to leave the shell     *\n");
    printf("*******************************************\n");

    while (1)
    {
        char cwd[1024];
        getcwd(cwd, sizeof(cwd));
        printf("mananshell:%s> ", cwd);

        fflush(stdout);
        if (fgets(input, sizeof(input), stdin) == NULL) // read input
        {
            break;
        }
        if (contains_pipes(input, command1, command2)) // Check for pipe
        {
            execute_piped_command(command1, command2);
            continue;
        }

        parse_input(input, args); // Pase input into arguments
        if (args[0] == NULL)
        { // Ignore empty inputs
            continue;
        }
        if (strcmp(args[0], "exit") == 0)
        { // Exit shell is exit is entered
            break;
        }
        if (strcmp(args[0], "cd") == 0) // Handles cd commands
        {
            if (args[1] == NULL) // No directory provided
            {
                fprintf(stderr, "cd: missing arguement\n");
            }
            else
            {
                if (chdir(args[1]) != 0) // Change directory
                {
                    perror("cd"); // If failes print error
                }
            }
            continue;
        }
        execute_command(args); // Execute command
    }
    printf("Thank you for using Manan's Shell! Goodbye! 👋\n");
    return 0;
}
