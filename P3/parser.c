
#include<fcntl.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
void parsestring(char* buffer){
    int length = strlen(buffer);
    for(int i = 0; i < )
}
int main(int argc, char** argv){
    char buffer[1024];
    memset(buffer,0,1024);
    int boi = read(STDIN_FILENO,buffer,1024);
    //printf("%s\n",buffer)
    boi = write(STDOUT_FILENO,buffer,boi + 1);
    return 0;
}
