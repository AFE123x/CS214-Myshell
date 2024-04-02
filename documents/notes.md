what is a tty?:
    "something that can accept inputs from a keyboard" - https://www.youtube.com/watch?v=SYwbEcNrcjI


Reading My Shell:
--

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





