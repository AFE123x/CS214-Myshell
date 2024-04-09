# My Shell

- This is a shell implementation developed using unix libraries

## Tests

### Test one - testing  single redirects

1)
```
./sum < input.txt
```
in the P3 directory, input.txt contains 9 and 10, print 90 on the console. 

2)
```
./sum > output.txt 
```
will take standard input from the user and store the result to the console. 


### Test Two - testing two redirects

1)
```
./sum < input.txt > output.txt
```
will pass in 9 and 10 to sum and output 90 to the output.txt file. 

### Test three - piping 

2)
```
ls -l | grep .c
```
will take the output of ls -l and pass it into grep with the arg .c 


3)

```
./sum arg1 arg2 arg3 | grep 81 
```
assuming 9 and 9 are inputted, 81 will be outputted, piped to the into grep 81 and will print 81. 

### Test four - redirects and piping

```
./sum < input.txt | grep 90 > output.txt
```

will redirect input.txt to sum, input contains 9 and 10, grep 90 will take the output of sum, and the output of grep will be redirected to output.txt


### Test five - wild cards 

```
ls *.c
```
will list out all files containing .c files. 

```
cat *.c
```
will print out the contents of all .c files in a directory. 

### Test six - conditional 

```
echo hello
then echo success
```
will print success


```
ls santoehun taoheuc ado.rud
else failure
```

will print failure 

### Test seven - neofetch

```
neofetch 
```
will run neofetch, searching the directory 

### Test eight - testing built in functions

```
cd ..
```
will go to the parent derictory

```
pwd
```

will print the current working directory. 

```
which vim
```

will print the location of vim, /usr/bin/vim