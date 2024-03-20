#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <iostream>
#include <stdio.h>
#include <fcntl.h>
#include <dirent.h>
#include <string>
#include <vector>
#include <sstream>
#include <functional>
#include <cstring>

using namespace std;


#define LOGGING  0
#define Suc "Success\n"
#define hysh_TOK_DELIM ' '
#define BUFFERSIZE 1024
#define COPYMORE 644

/**
 * Functions for various shell commands in a custom shell.
 *
 * @param args The arguments passed to the shell command.
 *
 * @returns The result of executing the shell command.
 */
int hysh_cd(vector<string> &args);
int hysh_ls(vector<string> &args);
int hysh_help(vector<string> &args);
int hysh_exit(vector<string> &args);
int hysh_copy(vector<string> &args);
int hysh_clear(vector<string> &args);
int hysh_touch(vector<string> &args);
int hysh_vhist(vector<string> &args);
int hysh_chist(vector<string> &args);

/**
 * A vector that stores a history of commands previously executed.
 */
vector<string> history ; 

/**
 * List of built-in commands in a shell program.
 */
vector<string> builtin_str = {
    "cd",
    "ls",
    "help",
    "exit",
    "cp",
    "clear",
    "touch",
    "vhist",
    "chist",
};

/**
 * List of built-in functions for the custom shell.
 *
 * @param builtin_func A vector of function pointers to the built-in functions.
 *
 * @returns None
 */
vector<function<int(vector<string>&)>> builtin_func = {
    &hysh_cd,
    &hysh_ls,
    &hysh_help,
    &hysh_exit,
    &hysh_copy,
    &hysh_clear,
    &hysh_touch,
    &hysh_vhist,
    &hysh_chist,
};

/**
 * Clears the console screen.
 *
 * @param args A vector of strings (not used in this function).
 *
 * @return 1 indicating success and back to loop.
 */
int hysh_clear(vector<string> &args){
    system("clear");
    return 1;
}


/**
 * Prints an error message to stderr and exits back to loop
 *
 * @param s1 The error message to be displayed.
 *
 * @returns None
 */
void errExit(const char *s1)
{
    fprintf(stderr, "Error: %s ", s1);
    exit(1);
}

/**
 * Copies the contents of a source file to a destination file.
 *
 * @param src The path of the source file.
 * @param dst The path of the destination file.
 *
 * @returns None
 */
void copyFile( char* src , char* dst)
{
    int in_fd, out_fd, n_chars;
    char buf[BUFFERSIZE];

     /* open files */
    if( (in_fd=open(src, O_RDONLY)) == -1 )
    {
        errExit("Cannot open src file");
    }

    if( (out_fd=creat(dst, COPYMORE)) == -1 )
    {
        errExit("Cannot create dest file");
    }
    /* Copying the files */
     while( (n_chars = read(in_fd, buf, BUFFERSIZE)) > 0 )
    {
        if( write(out_fd, buf, n_chars) != n_chars )
        {
            errExit("Write error to dest file");
        }


        if( n_chars == -1 )
        {
            errExit("Read error from src file");
        }
    }
  
  
    /* close files */
    if( close(in_fd) == -1 || close(out_fd) == -1 )
    {
      errExit("Error closing files");
    }
}

/**
 * Copies a file from a source location to a destination location.
 *
 * @param args A vector containing the source file path and destination file path.
 *
 * @returns 1 if the file is successfully copied, otherwise an error message is displayed.
 */
int hysh_copy(vector<string> &args)
{   
    if( args.size() < 3)
    {
        cout << "Usage : "<< args[0] << " <source_file> <destination_file>" <<endl;
        return 1;
    }

    char *src , *dst;
    src = args[1].data();
    dst = args[2].data();
    if( src[0] != '/' && dst[0] != '/' )//cp2 file1.txt file2.txt
    {
        copyFile(src, dst);
    }
    else if( src[0] != '/' && dst[0] == '/' )//cp2 file1.txt /dir 
    {
        int i;
        for(i=1; i<=strlen(dst); i++)
        {
            dst[(i-1)] = dst[i];
        }
        strcat(dst, "/");
        strcat(dst, src);
        copyFile(src, dst);
        
    } else {
        cerr << "Usage : cp source destination\n";
    }

    return 1;
}



/**
 * Lists all files and directories in the current directory.
 *
 * @param args A vector of strings (not used in the function).
 *
 * @returns 1 upon successful completion.
 */
int hysh_ls(vector<string> &args)
{   
    DIR *dr;
    struct dirent *en;
    dr = opendir(".");
     if (dr) {
      while ((en = readdir(dr)) != NULL) {
         cout<<" \n"<<en->d_name; //print all directory name
      }
      cout << "\n" ;
      closedir(dr); //close all directory
   }
    return 1;
}

/**
 * Change the current working directory to the specified path.
 *
 * @param args A vector of strings containing the command arguments.
 *
 * @returns -1 if the number of arguments is 1, 1 if successful, or an error code if the directory does not exist.
 */
int hysh_cd(vector<string> &args)
{   
    if(args.size() == 1) return -1;
    cout << "Entering path " << args[1] << endl;
    const char* dir = args[1].c_str();
    if(dir == nullptr) {
        chdir(getenv("HOME"));
        return 1;
    }
    else {
        if(chdir(dir) == -1)
        {
            cout << dir << " : No such directory \n";
            return 1;
        }
    }
    return 1;
}

/**
 * Displays help information for the custom shell.
 *
 * @param args A vector of strings containing arguments passed to the shell.
 *
 * @returns 1 to indicate successful execution.
 */
int hysh_help(vector<string> &args)
{
    printf("Hitesh's SHELL \n");
    printf("Type program names and arguments, and hit enter.\n");
    printf("The following are built in:\n");
    for (int i = 0; i < builtin_str.size(); i++) {
        cout << builtin_str[i] << endl;
    }
    return 1;
}

/**
 * Exits the program.
 *
 * @param args A vector of strings containing arguments.
 *
 * @return 0 upon successful exit.
 */
int hysh_exit(vector<string> &args)
{
    #if LOGGING
        cout << "exit implementation" << endl;
    #endif
    return 0;
}

/**
 * Creates an empty file with the given filename.
 *
 * @param args A vector of strings containing the filename.
 *
 * @return Returns 1 if the file creation is successful, -1 if no filename is provided.
 */
int hysh_touch(vector<string> &args){
    
    if(args.size() == 1) {
        perror("please provide filename \n");
        return -1;
    }
    const char* fname = args[1].c_str();
    FILE *fd = fopen(fname,"w");
    fclose(fd);
    return 1;
}

/**
 * Clears the history of commands.
 *
 * @param args A vector of strings containing command arguments.
 *
 * @return 1 indicating successful clearing of history.
 */
int hysh_chist(vector<string> &args)
{   
    history.clear();
    cout << "history cleared \n"; 
    return 1;
}

/**
 * Displays the previous commands stored in the history vector.
 *
 * @param args A vector of strings containing the arguments.
 *
 * @returns 1 if the operation is successful.
 */
int hysh_vhist(vector<string> &args)
{
    cout << "Previous Commands are : \n";
    int n = history.size();
    for(int i = 0 ; i < n-1 ; i++) cout << "cmd " << i+1 << " : " << history[i] << endl;
    return 1;
}


/**
 * Launches a new process with the given arguments.
 *
 * @param args A vector of strings representing the command and its arguments.
 *
 * @returns 1 upon successful execution of the process.
 */
int hysh_launch(vector<string> &args)
{
    pid_t pid , wpid;
    int status;
    vector<char*> cArgs;
    cArgs.reserve(args.size() + 1);
    for (const auto& arg : args) {
        cArgs.push_back(const_cast<char*>(arg.c_str()));
    }
    cArgs.push_back(nullptr);

    pid = fork();
    if(pid == 0){
        //Child process
        if(execvp(cArgs[0],cArgs.data()) == -1)
        {
            perror("hysh");
        }
        exit(EXIT_FAILURE);
    } else if (pid < 0){
        perror("hysh");
    } else {
        do {
            wpid = waitpid(pid,&status,WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }

    return 1;
}


/**
 * Executes a command based on the arguments provided.
 *
 * @param args A vector of strings containing the command and its arguments.
 *
 * @returns An integer indicating the success or failure of the command execution.
 */
int hysh_execute(vector<string> &args)
{   
    string a = args[0];
    if(args.empty()) return 1;
    
    #if LOGGING 
        cout << "args[0] : "<< args[0] << endl;
    #endif
    
    for(int i = 0 ; i < builtin_str.size() ; i++)
    {   
        if(args.size() >= 0 && args[0] == builtin_str[i])
        {   
            #if LOGGING
                cout << "executing function " << builtin_str[i] << endl;
            #endif
            return builtin_func[i](args);
        }
    }

    return hysh_launch(args);
}

/**
 * Splits a string into tokens based on a delimiter and returns a vector of tokens.
 *
 * @param line The input string to be split.
 *
 * @returns A vector of strings containing the tokens extracted from the input string.
 */
vector<string> hysh_split_line(string &line)
{
    vector<string> args;
    string arg;
    istringstream s(line);
    while(getline(s,arg,hysh_TOK_DELIM)){
        args.push_back(arg);
    }
    return args;
}

/**
 * Reads a line of input from the standard input.
 *
 * @return The string containing the input line.
 */
string hysh_read_line(void)
{
    string input;
    if(!getline(cin,input)){
        if(cin.eof()) {
            exit(EXIT_SUCCESS);
        } else {
            perror("hysh : getline \n");
            exit(EXIT_FAILURE);
        }
    }
    return input;
}




/**
 * Continuously reads input from the user, processes it, and executes the corresponding command.
 *
 * @returns None
 */
void hysh_loop(void)
{
  string line;
  vector<string> args;
  int status;
  do {
    printf("> ");
    line = hysh_read_line();
    history.push_back(line);
    args = hysh_split_line(line);
    status = hysh_execute(args);
  } while (status);
}




int main(int argc, char **argv)
{
  // Load config files, if any.

  // Run command loop.
  hysh_loop();

  // Perform any shutdown/cleanup.

  return EXIT_SUCCESS;
}
