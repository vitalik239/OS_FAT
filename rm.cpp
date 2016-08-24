#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "general.h"

fs_info fs;
char* path;

int main(int agrc, char** argv) {
    fs.root_path = argv[1];
    path = argv[2];
    read_fat(fs);

    char* file_name = strrchr(path, '/');
    *file_name = '\0';
    file_name++;

    Descriptor dir = find_by_name(fs, T_DIR, path);
    Descriptor last = find_child(fs, dir, T_FILE, file_name);
    if (last.fat_pos < 0) {
        printf("There is no such file\n");
        exit(0);
    }

    int prev = dir.fat_pos, curr = fs.fat[prev], descs;
    while (curr != fs.fat[curr]) {
        prev = curr;
        curr = fs.fat[curr];
    }

    FILE* file = open_block(fs, fs.root_block + curr);
    fread(&descs, sizeof(int), 1, file);
    descs--;
    fseek(file, descs * sizeof(Descriptor), SEEK_CUR);
    fread(&last, sizeof(Descriptor), 1, file);
    fseek(file, 0, SEEK_SET);
    fwrite(&descs, sizeof(int), 1, file);
    fclose(file);

    if ((descs == 0) && (dir.fat_pos != curr)) {
        fs.fat[curr] = -1;
        fs.fat[prev] = prev;
    }

    if ((last.type == T_FILE) && (strcmp(last.title, file_name) == 0)) {
        remove_file(fs, last);
        write_fat(fs);
        return 0;
    }

    Descriptor des;
    int curr_block = dir.fat_pos;

    while (true) {
        file = open_block(fs, fs.root_block + curr_block);
        fread(&descs, sizeof(int), 1, file);
        for (int i = 0; i < descs; i++) {
            fread(&des, sizeof(Descriptor), 1, file);
            if ((des.type == T_FILE) && (strcmp(des.title, file_name) == 0)) {
                remove_file(fs, des);
                fseek(file, -sizeof(Descriptor), SEEK_CUR);
                fwrite(&last, sizeof(Descriptor), 1, file);
                fclose(file);
                write_fat(fs);
                return 0;
            }
        }
        fclose(file);

        if (curr_block == fs.fat[curr_block]) {
            break;
        }
        curr_block = fs.fat[curr_block];
    }

    printf("Something went wrong! File missed\n");
    return 0;
}
