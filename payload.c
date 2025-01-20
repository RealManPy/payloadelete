#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <dirent.h>
#include <sys/types.h>
#include <unistd.h>
#endif

void overwrite_file(const char *filepath) {
    FILE *file = fopen(filepath, "r+b");
    if (!file) {
        fprintf(stderr, "Failed to open file: %s\n", filepath);
        return;
    }

    // Seek to the end to determine file size
    fseek(file, 0, SEEK_END);
    long filesize = ftell(file);
    fseek(file, 0, SEEK_SET);

    // Write null bytes to overwrite file content
    char *buffer = calloc(1, filesize);
    if (!buffer) {
        fprintf(stderr, "Memory allocation failed for file: %s\n", filepath);
        fclose(file);
        return;
    }

    fwrite(buffer, 1, filesize, file);
    free(buffer);
    fclose(file);

    // Optionally delete the file if needed
    if (remove(filepath) != 0) {
        fprintf(stderr, "Failed to delete file: %s\n", filepath);
    }
}

#ifdef _WIN32
void process_directory(const char *directory) {
    WIN32_FIND_DATA findFileData;
    HANDLE hFind;
    char search_path[MAX_PATH];
    snprintf(search_path, MAX_PATH, "%s\\*", directory);
    hFind = FindFirstFile(search_path, &findFileData);
    if (hFind == INVALID_HANDLE_VALUE) {
        fprintf(stderr, "Error opening directory: %s\n", directory);
        return;
    }

    do {
        if (strcmp(findFileData.cFileName, ".") == 0 || strcmp(findFileData.cFileName, "..") == 0) {
            continue;
        }
        char full_path[MAX_PATH];
        snprintf(full_path, MAX_PATH, "%s\\%s", directory, findFileData.cFileName);
        if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            process_directory(full_path);
        } else {
            overwrite_file(full_path);
        }
    } while (FindNextFile(hFind, &findFileData) != 0);
    FindClose(hFind);
}
#else
void process_directory(const char *directory) {
    struct dirent *entry;
    DIR *dir = opendir(directory);
    if (!dir) {
        fprintf(stderr, "Error opening directory: %s\n", directory);
        return;
    }

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        char full_path[1024];
        snprintf(full_path, sizeof(full_path), "%s/%s", directory, entry->d_name);

        if (entry->d_type == DT_DIR) {
            process_directory(full_path);
        } else {
            overwrite_file(full_path);
        }
    }
    closedir(dir);
}
#endif

int main(int argc, char *argv[]) {
    const char *target_dir = argc > 1 ? argv[1] : ".";
    printf("Processing directory: %s\n", target_dir);
    process_directory(target_dir);
    printf("Completed overwriting files.\n");
    return 0;
}

