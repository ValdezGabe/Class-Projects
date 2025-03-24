// Name: Gabriel Valdez

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

#define BUFF_SIZE 2049
#define MAX_ARGS 512

// Process Structure 
// Linked list
struct Process {
    pid_t val;
    struct Process* next;
};

// Shell Strcuture
struct Shell {
	char** args;
	int fg_mode;
	int recent;
	int current;
	struct Process* bg_pids;
	struct Process* bg_head;
	int bg_count;
};

// Global shell
struct Shell* smallsh = NULL;



/******************************************************************************
 * Name: free_shell
 * Description:
 * 	Frees all dynamically allocated memory within a Shell structure
 * 	including its arguments array and the structure itself.
 * Parameters:
 * 	- s: Pointer to shell structure
******************************************************************************/
void free_shell(struct Shell* s){
	// Mem Check
	if(!s){
		return;
	}

	// Iterate args while there is memory allocated
	if (s->args != NULL) {
        int i = 0;
		while(s->args[i] != NULL) {
			// Free data
			free(s->args[i]);
			s->args[i] = NULL;
			i++;
		}
		// Free the args array itself
        free(s->args);
		s->args = NULL;
    }

	// Free background process list
    struct Process* temp = s->bg_pids;
    while (temp != NULL) {
        struct Process* prev = temp;
        temp = temp->next;
        free(prev);
    }

	// Free the Shell structure itself
    free(s);
	smallsh = NULL;
}


/******************************************************************************
 * Name: build_pids
 * Description:
 * 	Build's pid list
 * Parameters:
 * 	- s: Pointer to shell structure
******************************************************************************/
void build_pids(struct Shell* s){
	// Error handling
	if(s == NULL){
		return;
	}
	s->bg_pids = NULL;
	s->bg_count = 0;
}


/******************************************************************************
 * Name: build_shell
 * Description:
 * 	Initializes default shell struct
 * Parameters:
 * 	- s: Pointer to shell structure
******************************************************************************/
void build_shell(struct Shell* s) {
	// Error handling
	if (s == NULL){
		return;
	}

	s->bg_pids = NULL;
	s->fg_mode = 0;
	s->recent = 0;
	s->current = 0;
	s->args = NULL;

	build_pids(s);
}


/******************************************************************************
 * Name: terminate_processes
 * Description:
 * 	Terminates all background processes 
 * 	Frees the linked list of processes
 * Parameters:
 * 	- s: Pointer to shell structure
******************************************************************************/
void terminate_processes(struct Shell* s) {
	struct Process* temp = s->bg_pids;
    struct Process* prev = NULL;

    while (temp != NULL) {
		// Kill and wait for process to exit
        kill(temp->val, SIGTERM);  
        waitpid(temp->val, NULL, 0); 

        // Move to the next process before freeing current one
        prev = temp;
        temp = temp->next;
        free(prev);
    }

    // Reset the linked list
    s->bg_pids = NULL;
    s->bg_count = 0;
}


/******************************************************************************
 * Name: sh_exit
 * Description:
 * 	Terminals all background process 
 * 	Frees allocated memory 
 * 	Exits shell
 * Parameters:
 * 	- s: Pointer to shell structure
 *  - input: Pointer to user input
******************************************************************************/
int sh_exit(struct Shell* s, char* input) {
	if (s) {
        terminate_processes(s);
        free_shell(s);
    }
    
    if (input) {
        free(input);
    }

	exit(0);
}


/******************************************************************************
 * Name: sh_cd
 * Description:
 * 	Changes the current working directory
 * 	If there is no argument, then it defaults to home directory
 * Parameters:
 * 	- args: Array of strings 
******************************************************************************/
int sh_cd(char** args){
	// Check if there are arguements
	// Else change to dir
	if (args[1] == NULL) {
        chdir(getenv("HOME"));
    } else if (chdir(args[1]) != 0) {
		fprintf(stdout, "CD error\n");
		fflush(stdout);
	}
	return 0;
}


/******************************************************************************
 * Name: redirect_file
 * Description:
 * 	 Handles input and output file redirection for the shel
 * Parameters:
 * 	- fileName: Name of the redirected file
 *  - flag: An int representing in (0) and out (1)
******************************************************************************/
int redirect_file(char* fileName, int flag) {
	// Dont work
    if (fileName == NULL) {
		return 0;  
	}

	// Default file descriptor
    int fd = -1;

	// flag Redirection
    if (flag == 0) {  
        fd = open(fileName, O_RDONLY);
		// Failed
        if (fd == -1) {
            fprintf(stdout, "%s: No such file or directory\n", fileName);
			fflush(stdout);
            return 0;  
        }
        dup2(fd, STDIN_FILENO);
    }  else if (flag == 1) {  
		// Output Redirection
        fd = open(fileName, O_WRONLY | O_CREAT | O_TRUNC, 0644);
		// Failed
        if (fd == -1) {
            fprintf(stdout, "%s: No such file or directory\n", fileName);
			fflush(stdout);
            return 0;  
        }
        dup2(fd, STDOUT_FILENO);
    }

    close(fd);
    return 1;  
}



/******************************************************************************
 * Name: sh_execute
 * Description:
 * 	 Executes commands in the shell
 * 		- Built ins
 * 		- Process
 * Parameters:
 * 	- s: Pointer to shell structure
 *  - input: Pointer to user input
******************************************************************************/
int sh_execute(struct Shell* s, char* input) {
	// No command
    if (s->args[0] == NULL) {
        return 0; 
    }

	// Built ins
    if (strcmp(s->args[0], "exit") == 0) {
        sh_exit(s, input);
    } else if (strcmp(s->args[0], "cd") == 0) {
        return sh_cd(s->args);
    } else if (strcmp(s->args[0], "status") == 0) {
        return sh_status();
    }

	// Flag for background execs
    int bg_flag = 0, i = 0;

	// Ini Redirection Variables
    char* infile = NULL;
    char* outfile = NULL;

    // Parse argsuments for < > and &
    while (s->args[i] != NULL) {
        if (strcmp(s->args[i], "<") == 0 && s->args[i + 1] != NULL) {
            infile = s->args[i + 1];  
			// Remove operator
            s->args[i] = NULL;  
        }
        else if (strcmp(s->args[i], ">") == 0 && s->args[i + 1] != NULL) {
            outfile = s->args[i + 1];  
			// Remove operator
            s->args[i] = NULL;  
        }
        else if (strcmp(s->args[i], "&") == 0) {
            bg_flag = 1;
			// Remove operator
            s->args[i] = NULL;  
        }
        i++;
    }

    // Prevent same file for input and output
    if (infile && outfile && strcmp(infile, outfile) == 0) {
        fprintf(stdout, "Same file error\n");
		fflush(stdout);
        return 1;
    }

	// Fork a new process for execs
    pid_t fork_result = fork();
    if (fork_result == -1) {
        fprintf(stdout, "Fork error\n");
		fflush(stdout);
        return 1;
    }

	// Child process
    if (fork_result == 0) {  
		// Reset SIGINT to default
        if (!bg_flag) {
			// Default sigs 
            struct sigaction def_action = {0};
			// If SIGINT, terminate like usual
            def_action.sa_handler = SIG_DFL;
			// New sig
            sigaction(SIGINT, &def_action, NULL);
        }

        // Input rd
        if (infile && !redirect_file(infile, 0)) {
            exit(1);
        }

        // Output
        if (outfile && !redirect_file(outfile, 1)) {
            exit(1);
        }

		// Repalce the current process with command
        execvp(s->args[0], s->args);
        fprintf(stdout, "%s: command not found\n", s->args[0]);
		fflush(stdout);

        exit(1);
		// Parent Process
    }  else {  
        if (!bg_flag) {
			// Wait for child to finish and get status
            int stat;
            waitpid(fork_result, &stat, 0);
			// Store exit stat
            s->recent = WEXITSTATUS(stat);
        } else {
            printf("Background process %d started\n", fork_result);
            fflush(stdout);

			// Allocate memory for processes
            struct Process* p = (struct Process*)malloc(sizeof(struct Process));
            // Store process variables
			p->val = fork_result;
            p->next = s->bg_pids;
			// Insert at head
            s->bg_pids = p;
            s->bg_count++;
        }
    }
    return 0;
}


/******************************************************************************
 * Name: expandShVar
 * Description:
 * 	 Expands occurences of $$
 * 	 Replaces dollas with pid 
 * Parameters:
 * 	- s: Pointer to shell structure
 *  - input: Pointer to user input
******************************************************************************/
char* expandShVar(char* line) {
	// Error handling
    if (line == NULL) {
        return NULL;
    }

	// Get length of input
    size_t len = strlen(line);
    char pidStr[16]; 
	// Get pid and store in pid buffer
    snprintf(pidStr, sizeof(pidStr), "%d", getpid());  

	// Count occurrences of dolla signs
    size_t count = 0;
	// Make sure i is positive
	size_t i;
    for (i = 0; i < len - 1; i++) {
		// Check $ followed by $
        if (line[i] == '$' && line[i + 1] == '$') {
            count++;
        }
    }

	// Original length + extra space to replace
	// Get length of array
	size_t pidLen = strlen(pidStr);
	// -2 since dolla dolla
	// Calculate required size
    size_t newSize = len + (count * (pidLen - 2)) + 1; 
	// Allocate enough memory to expanded 
    char* result = (char*)calloc(newSize, sizeof(char));
    if (!result) {
        fprintf(stdout, "Allocation error\n");
		fflush(stdout);

        return NULL;
    }

    char* r_ptr = line;
    char* w_ptr = result;

    while (*r_ptr) {
		// If we find $ followed by $, replace w pid
        if (*r_ptr == '$' && *(r_ptr + 1) == '$') {
            strcpy(w_ptr, pidStr);
            w_ptr += strlen(pidStr);
            r_ptr += 2;
        } else {
            *w_ptr++ = *r_ptr++;
        }
    }
	// Terminate with null term
    *w_ptr = '\0';

    return result;
}


/******************************************************************************
 * Name: read_line
 * Description:
 * 	Reads a line of input and expands 
 * Parameters:
 * 	- None
******************************************************************************/
char* read_line(){
    char* line = (char*)calloc(BUFF_SIZE, sizeof(char));
    if (line == NULL) {
        fprintf(stdout, "Allocation error \n");
		fflush(stdout);
        return NULL;
    }

	// EOF
    if (fgets(line, BUFF_SIZE, stdin) == NULL) {
        free(line);
        return NULL; 
    }

    // Remove newline
    line[strcspn(line, "\n")] = '\0';

    // Ignore comments
    if (line[0] == '#') {
        free(line);
        return NULL;  
    }

    char* exp = expandShVar(line);
    free(line);
    return exp;
}




/******************************************************************************
 * Name: split_line
 * Description:
 * 	Parses the input into an array of tokens/args/commands 
 * Parameters:
 * 	- line: Pointer to input string
******************************************************************************/
char** split_line(char* line) {
	if(line == NULL){
		return NULL;
	}

	char** tokens = (char**)calloc(MAX_ARGS, sizeof(char*));
	char* rest = NULL;
	char* token;
	int pos = 0;
	
	if(!tokens){
		fprintf(stdout, "Allocation error \n");
		fflush(stdout);

		return NULL;
	}

	token = strtok_r(line, " ", &rest);
	while(token != NULL){
		tokens[pos] = token;
		pos++;

		if(pos >= BUFF_SIZE){
			fprintf(stdout, "Exceeds Maxium Arguments\n" );
			fflush(stdout);

			free(tokens);
			return NULL;
		}
		token = strtok_r(NULL, " ", &rest);
	}
	tokens[pos] = NULL;
	return tokens;
}


/******************************************************************************
 * Name: sh_status
 * Description:
 * 	Prints the exit status of the alast exec'd process
 * Parameters:
 * 	- status: An int representing the exit status
******************************************************************************/
int sh_status(int status){
    if (status < 2) {
        fprintf(stdout, "exit value %d\n", status);
    } else {
        fprintf(stdout, "terminated by signal %d\n", status);
    }
	fflush(stdout);
    return 0;
}


/******************************************************************************
 * Name: SIGINT_handler
 * Description:
 * 	Handles ^C signal 
 * Parameters:
 * 	- signal_number: Integer that represents the signal that triggered the signal
******************************************************************************/
void SIGINT_handler(int signal_number){
	// Error handling
	if(smallsh == NULL){
		return;
	}

	// Ignore SIGINT
	if (smallsh->current != 0) {
		// Kill the foreground process
        kill(smallsh->current, SIGINT); 
        write(STDOUT_FILENO, "\n", 1);
    }

    write(STDOUT_FILENO, ": ", 2);
    fflush(stdout);
}


/******************************************************************************
 * Name: SIGTSTP_handler
 * Description:
 * 	Handles ^Z signal 
 * Parameters:
 * 	- signal_number: Integer that represents the signal that triggered the signal
******************************************************************************/
void SIGTSTP_handler(int signal_number){
	// Error handling
	if(smallsh == NULL){
		return;
	}

	//Toggle foreground mode
	smallsh->fg_mode = !smallsh->fg_mode;

	if (smallsh->fg_mode){
		char* msg = "\nEntering foreground-only mode (& is now ignored)\n: ";
        write(STDOUT_FILENO, msg, strlen(msg));
	} else {
		char* msg = "\nExiting foreground-only mode\n: ";
        write(STDOUT_FILENO, msg, strlen(msg));
	}
	write(STDOUT_FILENO, ": ", 2);
	fflush(stdout);

}


/******************************************************************************
 * Name: zombie_handler
 * Description:
 * 	Handles SIGCHLD signal
 * 	Cleans up zombie background processes
 * Parameters:
 * 	- signal_number: Integer that represents the signal that triggered the signal
******************************************************************************/
void zombie_handler(int signal_number) {
    int child_pid, child_status;
    struct Process** prev = &smallsh->bg_pids;
    struct Process* curr = smallsh->bg_pids;

    while ((child_pid = waitpid(-1, &child_status, WNOHANG)) > 0) {
        if (WIFEXITED(child_status)) {
            printf("Background process %d is done: exit value %d\n", child_pid, WEXITSTATUS(child_status));
        } else if (WIFSIGNALED(child_status)) {
            printf("Background process %d is done: terminated by signal %d\n", child_pid, WTERMSIG(child_status));
        }
        fflush(stdout);

        // Remove from linked list
        while (curr != NULL) {
            if (curr->val == child_pid) {
                *prev = curr->next;
                free(curr);
                smallsh->bg_count--;
                break;
            }
            prev = &curr->next;
            curr = curr->next;
        }
    }
}


/******************************************************************************
 * Name: build_handlers
 * Description:
 * 	Configures handler
 * Parameters:
 * 	- signal_number: Integer that represents the signal that triggered the signal
 *  - handler: Pointer to singal handler function
******************************************************************************/
int build_handlers(int signal_number, void (*handler)(int)){
	struct sigaction action;
    
    // Set signal handler function
    action.sa_handler = handler;

	// Block all signals while handling
    sigfillset(&action.sa_mask); 

	// Restart system calls
    action.sa_flags = SA_RESTART; 

	if (sigaction(signal_number, &action, NULL) == -1) {
		fprintf(stdout, "Signal action failed");
		fflush(stdout);

		return -1;
    }
	return 0;
}


/******************************************************************************
 * Name: loop
 * Description:
 * 	Runs the main interactive loop of the shell
 * Parameters:
 * 	- s: Pointer to shell structure
******************************************************************************/
void loop(struct Shell* s){
	char* line;

	do {
		printf(": ");
		fflush(stdout);

		line = read_line();
		if (line == NULL) {
            continue;  // Skip this iteration if read_line failed
        }

        s->args = split_line(line);
        if (s->args == NULL) {
            free(line);
            continue;  // Skip execution if splitting fails
        }

		s->current = sh_execute(s, line);

		free(line);
		free(s->args);

		line = NULL;
		s->args = NULL;
	} while (s->current == 0);
}

int main() {
	// Good luck, and have fun! There's a lot to learn from this assignment
	// :)
	// Memory Allocation
	struct Shell* s = (struct Shell*)malloc(sizeof(struct Shell));
	if (s == NULL) {
		fprintf(stdout, "Allocation Error\n");
		fflush(stdout);

		exit(1);
	}

	// Initialize Shell
	build_shell(s);
	smallsh = s;

	// Buiild signal handlers
	if (build_handlers(SIGINT, SIGINT_handler) == -1 || 
		build_handlers(SIGTSTP, SIGTSTP_handler) == -1 || 
		build_handlers(SIGCHLD, zombie_handler) == -1) {
			free_shell(s);
			exit(1);
	}

	// Run shell loop
	loop(s);

	// Free memory
	free_shell(s);
	return 0;
}
