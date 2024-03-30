#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

int main (int argc, char** argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    int fd = open(argv[1], O_RDONLY);
    if (fd == -1) {
        perror("Error opening file");
        return 1;
    }

    if (!isatty(fd)) {
        printf("Is a valid file\n");
    } else {
        printf("Is not a valid file\n");
    }

    close(fd);

    return 0;
}
