what is a tty?:
    "something that can accept inputs from a keyboard" - https://www.youtube.com/watch?v=SYwbEcNrcjI


# Reading My Shell:


Implement interactive and batch modes:
	interactive mode: user inputted commands
		when given no arguments will read commands from standard input
	batch mode: read commands from file or piped in
		will take UPTO one argument	
*both* will read and interpret a sequence of commands

use *read()* to obtain input
	(1). obtain a full command before executing it
	(2). not call **read()** after receiving a newline character until it has executed the received command

*mysh* terminates when it recieves the **exit** command, or when its input stream ends.

### Interactive mode
- print welcome message before first command prompt
- should print "Goodbye" when exiting (whether it be through inputting exit or reaching the end of the input stream)
- "mysh> " is also required to indicate input is allowed.

```bash
$ ./mysh
Welcome to my shell!
mysh> cd subdir
mysh> echo hello
hello
mysh> cd subsubdir
mysh> pwd
/current/path/subdir/subsubdir
mysh> cd directory_that_does_not_exist
cd: No such file or directory
mysh> cd ../..
mysh> exit
mysh: exiting
$
```

---

### Batch mode
```bash
$ cat myscript.sh
echo hello
$ ./mysh myscript.sh
hello
$
```

with no specified file:
```bash
$ cat myscript.sh | ./mysh
hello
$
```


----

### Command format

- We'll read one command per line, describing a job
	- This involves one or more child processes executing specified program with arguments.
		- Might override standard I/O from other arguments.
			- Example, redirecting files. 

- When we process a command, we should get,
	- The path to the executable file.a
	- List of argument strings
	- What files to use for standard I/O

***Wild cards***

- If there's * in the token, it describes a pattern. 
	- *.c describes all files with .c,
	- if it doesn't match the pattern, add the original token to the argument. 

***redirection***

- Tokens > and < introduce input and output redirection. The first arg is the redirection flag. 
	- It should be considered a target.
	- NOTE: It can happen anywhere in a program. 

***pipelines***
- The special token | seperates two programs in a pipeline. it redirects stuff. We can use ```pipe()``` to arrange standard output for the first process. 
	- Assume only two processes. 

***conditionals***
- If the first token is then or else, it isn't added to the arg list. 
	- Then executes if the previous command succeed
	- else if the previous fails. 
	- Status doesn't change if conditional program is not executed. 

### Program name
argument 1 is the program that is wished to be executed
    this can either be:
        1. name of the program
        2. path to the program
        3. a "built-in" command

#### Pathnames
IF first argumant conains / **assume** it is a path

#### Bare names
IF the first argument is not a path (including a /) nor built-in
then search for the name of the argument in these directories sequentually:
    1. /usr/local/bin
    2. /usr/bin
    3. /bin

WE CAN USE *ACCESS()* TO DETERMINE IF A FILE WITH AN APPROPRIATE NAME EXISTS IN THESE (the previously referred) DIRECTORIES?! WHY DIDNT ANYONE TELL ME?! (do not implement cp, mv, cat, etc).


#### Built-in commands
- pwd
    - *(**pwd**) prints the current working directory to standard output. This can be obtained using getcwd().*
- which
    - *(**which**) takes a single argument, which is the name of a program. It prints the path that mysh would use if asked to start that program. (That is, the result of the search used for bare names.) which prints nothing and fails if it is given the wrong number of arguments, or the name of a built-in, or if the program is not found.*
- exit
    - *(**exit**) indicates that mysh should cease reading commands and terminate. Additionally, exit should print any arguments it receives, separated by spaces*
- cd
    - *(**cd**) should print an error message and fail if it is given the wrong number of arguments, or if chdir() fails.*

(these MAY exist in the previous directories but ignore them)


### Wild cards

- If you have a *, it's a wildcard/glob
	- Here, the file needs to match a pattern. 
- If your program finds an entire list of things with the wildcard, replace the wildcard with the list you got. 
	- If you have something that starts with *.txt, you will not match names that begin with a period. 


### Redirection

- '<' is for standard input
- '>' is for standard output
    - if the file does not exist, create it.
    - if something exists in the file- truncate it.
- Use mode 0640 (S_IRUSR |S_IWUSR|S_IRGRP) when creating

an argument after '>' or '<' is automatically considered a path to a file. it IS NOT included in the argument list for the program

-  mysh should open the specified file in the appropriate mode and use dup2() in the child process to redefine file 0 or 1 before calling execv().
- If mysh is unable to open the file in the requested mode, it should report an error and set the last exit status to 1.

### Pipes
- Pipe connects standard input from one program to the standard output of another.
	- This allows data to "flow" from one program to the next. 
	- Use pipe() to make a pipe, then dup2() to set the standard of the child process to the first process to the standard input of the second process.
		- If pipe fails, print an error message. 
### Additional notes
- Assume no wildcard after < or >. 
	- use execv to run program.

---



# Project C-Shell from Brown abbreviated (for tips)

## Assignment Outline

- typically your shell should read from STDIN and write to STDOUT. BUT the command line can contain input/output redirection tokens, in which case it must set up the appropriate files to deal with this.

Good Steps Checklist:
1. Ensure Makefile is up and running
2. Read & parse input as recommended in Parsing the Command Line
3. Implement built-in commands
4. Handle child processes (get fork & execv working)
5. Handle input/output redirection
6. Implement error handling for syscalls and bad user input

not following the order of the list can lead to VeryBadTime™
## Getting Started
### Roadmap
1. **Parse the Command Line:**
	
	- you must use read function
	- program can handle incorrect input
		- examples being:
			- User can enter nothing and hit the return key
			- User should enter a supported command if there are any non-whitespace text
			- User can enter redirection token at ANY point in a command
			- User cannot use any more than one of the same redirect in one line (ie. one input, one output)
			- User can enter any amount of whitespace between tokens 
2. **Implementing ~~cd, rm, ln, and exit~~ built-in functions:**
	- take a look at chdir, link, and unlink
3. **Handle Child Processes:**
	- to handle ANY other process, you will need to create a new process using the *fork system command*
		- make sure that the parent process waits for the child to complete (i.e. using the wait function)
			- if the parent exits before the child you create an **orphan**
			- if the child exits before the parent you create a **zombie**
	- Once forked properlly you can use execv. replacing the child with the program of your choosing
		- the full arguments to execv should be the full path to the program you wish to execute and an array of arguments for that program.
			- to get the full path to a program you may want to run:
				- **which \<command>**
4. **Handle Input/Output Redirects:**
	- There are three redirect symbols that your program will need to handle: < (redirect input), >, and >> (redirect output. I dont think we have to do this one for our shell)
	- Keep in mind that redirects are not guaranteed to appear in any particular order. A redirect can appear after or before a command.

5. **System Call Error Checking:**
	- Almost all system calls return a value. you can use these for error checking.


### Invalid Command-Line Input
your code should be able to handle all of these cases:
```bash
33sh> /bin/cat < foo < gub
ERROR - Can’t have two input redirects on one line.
33sh> /bin/cat <
ERROR - No redirection file specified.
33sh> > gub
ERROR - No command.
33sh> < bar /bin/cat
OK - Redirection can appear anywhere in the input.
33sh> [TAB]/bin/ls <[TAB] foo
OK - Any amount of whitespace is acceptable.
33sh> /bin/bug -p1 -p2 foobar
OK - Make sure parameters are parsed correctly.
```

a buffer length of 1024 bytes should be used

## Executing Shell Commands

### UNIX System Calls for Built-In Functions
You can read the manual for these commands by running the shell command "man 2 \<syscall>". It is highly recommended that you read ll the man pages for these syscalls before starting to implement built-in commands

examples:
```c
int chdir(const char *path);
int link(const char *existing, const char *new);
int unlink(const char *path);
```

### Executing a Program
when a UNIX process executes another program it takes over in its entirety. so- you must defer the task to another process. here is a list of system calls, functions and shell commands useful to this project, related to executing a program:

#### fork()
```c
pid_t fork(void)
```
First, you’ll need to create a new process. This must be done using the system call fork(), which creates a new “child” process which is an exact replica of the “parent” (the process which executed the system call). This child process begins execution at the point where the call to fork() returns. fork() returns 0 to the child process, and the child’s process ID (abbreviated pid) to the parent.

#### execv()
```c
int execv(const char *filename, char *const argv[])
```
To actually execute a program, use the library function execv(). Because execv() replaces the entire process image with that of the new program, this function never returns if it is successful. Its arguments include filename, the full path to the program to be executed, and argv, a null-terminated argument vector. Note that argv[0] MUST be the binary name (the final path 1 component of filepath), NOT the full path to the program (which means you will have to do some processing in constructing argv[0] from filename).

As an example, the shell command /bin/echo 'Hello World!' would have an argv that looks like this:

```c
char *argv[4];
argv[0] = "echo";
argv[1] = "Hello";
argv[2] = "world!";
argv[3] = NULL;
```
*you can use **which** to find the full path to a program*

**HERE** is an example of forking and executing **/bin/ls**, with error checking:
```c
if (!fork()) {
/* now in child process */
char *argv[] = {"ls", NULL};
execv("/bin/ls", argv);
/* we won’t get here unless execv failed */
perror("execv");
/* hint: man perror */
exit(1);
}
/* parent process continues to run code out here */
```

#### wait()
```c
pid_t wait(int *status)
```

This command waits for a status change in the child- such as termination. This prevents the parent from moving on without the child. you wont need to pass excess information to wait, you can just use NULL, which will tell it to not store any data.

### Files, File Descriptors, Terminal I/O
#### File descriptors
A file descriptor is simply an integer which the operating system maps to a file location. Processes do not directly access files using FILE structs but rather through the kernal by using file descriptors and low-level system calls.

Subprocesses inherit open files and their corresponding file descriptors from their parent process. As a result, processes started from within a normal UNIX shell inherit three open files:
stdin, stdout, and stderr, which are assigned file descriptors 0, 1, and 2 respectively. Since 2 your shell will be run from within the system’s built-in shell, it inherits these file descriptors; processes executed within your shell will then also inherit them. As a result, whenever your shell or any process executed within it writes characters to file descriptor 1 (the descriptor corresponding to stdout), those characters will appear in the terminal window.

#### open()
```c
int open(const char *pathname, int flags, mode_t mode)
```
open opens a file for reading or writing, located at the relative, or absolute pathname, and returns a new file descriptor which maps to that file.

the other values are how the file should be opened and the flags indacate the status flags and access modes. mode is used to determine the default permissions of the file if it must be created.

#### close()
```c
int close(int fd)
```
close() closes an open file descriptor, which allows it to be reopened and reused later in the
life of the calling process. If no other file descriptors of the calling process map to the same file,
any system resources associated with that file are freed. close() returns 0 on success and −1
on error.


#### read()
```c
ssize_t read(int fd, void *buf, size_t count)
```
read() reads up to count bytes from the given file descriptor (fd) into the buffer pointed to by
buf. It returns the number of characters read and advances the file position by that many bytes,
or returns −1 if an error occurred. Check and use this return value. It is otherwise impossible
to safely use the buffer contents. **Note that read does not null terminate the buffer.**

If a user types CTRL-D on a line by itself, read will return 0, indicating that no more data is
available to be read—a condition called end of file (EOF). In this case, your shell should exit.

**NOTE:** The values **0**, **NULL**, and **\0** are interchangeable and completely
 equivalent.

#### write()
```c
ssize_t write(int fd, const void *buf, size_t count)
```
write() writes up to count bytes from the buffer pointed to by buf to the given file descriptor
(fd). It returns the number of bytes successfully written, or −1 on an error.

#### printf()
```c
int printf(const char *format, ...)
```

the difference between this and write is that by default printf outputs to STDOUT. no file descriptor is needed

**NOTE:** if you’re using printf() to write a string that doesn’t end in a newline (hint: your prompt), you must use fflush(stdout) after printf() to actually write your output to the terminal.

### Prompt Format (this section is for testing)
While the contents of your shell’s prompt are up to you, you must implement a particular feature
in order to make your shell easier to grade. Specifically, you should surround the statement that
prints your prompt with the C preprocessor directives #ifdef PROMPT and #endif, which will
cause the compiler to include anything in between the two directives only when the PROMPT
macro is defined.

For example, if you print your prompt with the statement

printf("33sh> "); you would replace it with the following:

```c
#ifdef PROMPT
if (printf("33sh> ") < 0) {
/* handle a write error */
}
#endif
```
**Note:** If you choose to use printf() to write your prompts, and not write(), there is an
additional step you will have to take to get the prompt to show up in the terminal, because the
prompt does not end in a newline. See the printf() section for more details.

## Input and Output Redirection
### File Redirection
**File redirection should be the last shell functionality you implement.**

the redirection commands (<, >, or >>) can appear anywhere within a command in any order

- < [path] - Use file [path] as standard input (file descriptor 0).
- \> [path] - Use file [path] as standard output (file descriptor 1). If the file does not exist, it is created; otherwise, it is truncated to zero length. (See the description of the O_CREAT and O_TRUNC flags in the open(2) man page.)
- \>> [path] - Use file [path] as standard output. If the file does not exist, it is created; otherwise, output is appended to the end of it. (See the description of the O_APPEND flag in the open(2) man page.)

it is **ILLEGAL** to reirect input or output twice (although it is perfectly legal to redirect input and redirect output).

### Redirecting a File Descriptor (a good and useful read)
To make a program executed by your child process read input from or write output to a specific file, rather than use the default stdin and stdout, we have to redirect the stdin and stdout file descriptors to point to the specified input and output files. Luckily, the kernel’s default behavior provides an elegant solution to this problem: when a file is opened, the kernel returns the smallest file descriptor available, regardless of its traditional association. Thus, if we close file descriptor 1 (stdout) and then open a file on disk, that file will be assigned file descriptor 1. Then, when our program writes to file descriptor 1, it will be writing to the file we’ve opened
rather than stdout (which traditionally corresponds to file descriptor 1 by default).

For the purposes of this project, we won’t be concerned with restoring the original file
descriptors for stdout and stdin in the child process as it won’t affect your shell. If you’re interested in the technically safer (but more complex) way to redirect files, check out the dup() and dup2() man pages.

## Use of Library Functions
You should use the read() system call to read from file descriptors **STDIN_FILENO** (a macro defined as 0), **STDOUT_FILENO** (1), and **STDERR_FILENO** (2), which correspond to the file streams for standard input, standard output, and standard error respectively. You should use the write() system call to write to **STDOUT_FILENO** or **STDERR_FILENO** OR the higher level non-system calls printf() (which doesn’t require a specified file descriptor) and fprintf().

*the allowed non-sys calls from brown. I suppose useful*

Memory-Related:
- memset() memmove() memchr() memcmp() memcpy()

Strings Manipulation:
- str(n)cat() tolower() strtol() isalnum() isalpha() iscntrl() isdigit() islower() isprint() ispunct() isspace() isxdigit() str(n)cat() str(n)cmp() str(n)cpy() strtol() isgraph() isupper() strlen() strpbrk() strstr() strtok() str(r)chr() str(c)spn() toupper() atoi()

Error-Handling:
- perror() assert() strerror()

Output:
- fflush() printf() (v)s(n)printf() fprintf()

Misc:
- exit() execv() opendir() readdir() closedir()

### Error Handling
dont forget to make functions to handle some functions error handling. since most functions return a value like -1 when an error is caused

## Support
### which()
```c
which <program name>
```

In order to execute a program in your shell, you will need that program’s full pathname. You will not be able to use only a shortcut, as you would in a bash terminal for programs such as ls, cat, xpdf, gedit, etc. To execute these programs from your shell, you must enter /bin/cat, /usr/bin/xpdf, /usr/bin/gedit, and so on. To find the full pathname for any arbitrary program, use which.

Example usage:
```bash
$ which cat
/bin/cat
$ which gedit
/usr/bin/gedit
```
For more information, see the man page for which. You can even use which in your shell, once you have determined its full path (type which which in a system terminal to find its full path)!

