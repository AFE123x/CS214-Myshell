#include <stdio.h>
#include <unistd.h>

int main() {
    int fd;
    printf("Enter any character: ");
    fflush(stdout); // Flush stdout to ensure prompt is displayed before input

    // Check if stdin (file descriptor 0) is associated with a terminal
    if (isatty(0)) {
        printf("\nstdin is associated with a terminal.\n");
    } else {
        printf("\nstdin is NOT associated with a terminal.\n");
    }

    return 0;
}

