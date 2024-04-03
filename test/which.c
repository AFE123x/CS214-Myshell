#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
void search_directory(const char *path, const char* program) {
    DIR *directory;
    struct dirent *entry;
    struct stat info;

    directory = opendir(path);
    if (directory == NULL) {
        perror("opendir");
        return;
    }

    while ((entry = readdir(directory)) != NULL) {
        char full_path[PATH_MAX];
        memset(full_path, 0, PATH_MAX);
        snprintf(full_path, sizeof(full_path), "%s/%s", path, entry->d_name);

        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        // printf("%s\n", entry->d_name);

        if (!strcmp(entry->d_name, program)) {
            write(STDOUT_FILENO, full_path, strlen(full_path));
            write(STDOUT_FILENO, "\n", 1); // Add newline
            closedir(directory); // Close directory since program is found
            return; // Exit the function
        }
    }

    // Close the directory
    closedir(directory);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <program_name>\n", argv[0]);
        return EXIT_FAILURE;
    }

    search_directory("/usr/local/bin", argv[1]);
    search_directory("/usr/bin", argv[1]);
    search_directory("/bin", argv[1]);

    return EXIT_SUCCESS;
}
