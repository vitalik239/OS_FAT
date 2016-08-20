#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include "general.h"

fs_info fs;
char* path;

int main(int agrc, char** argv) {
    fs.root_path = argv[1];
    path = argv[2];
    read_fat(fs);

    char* dir_name = strrchr(path, '/');
    *dir_name = '\0';
    dir_name++;

    int node = find_by_name(fs, T_DIR, path);

    Descriptor des;
    FILE* file = open_block(fs, fs.root_block + node);
    fread(&des, sizeof(des), 1, file);
    fseek(file, -sizeof(Descriptor), SEEK_CUR);
    des.size--;
    fwrite(&des, sizeof(Descriptor), 1, file);
    fclose(file);

    int prev = node, curr = fs.fat[node], last;
    while (curr != fs.fat[curr]) {
        prev = curr;
        curr = fs.fat[curr];
    }
    file = open_block(fs, fs.root_block + curr);
    fseek(file, (des.size % INTS_IN_BLOCK) * sizeof(int), SEEK_CUR);
    fread(&last, sizeof(int), 1, file);
    fclose(file);

    if (des.size % INTS_IN_BLOCK == 0) {
        fs.fat[prev] = prev;
        fs.fat[curr] = -1;
    }

    Descriptor last_des;
    file = open_block(fs, fs.root_block + last);
    fread(&last_des, sizeof(des), 1, file);
    fclose(file);
    if ((last_des.type == T_DIR) && (strcmp(last_des.title, dir_name) == 0)) {
        remove_dir(fs, last);
        write_fat(fs);
        return 0;
    }

    curr = node;
    int next = fs.fat[node];
    int list[INTS_IN_BLOCK];
    int left = des.size;

    while (curr != next) {
        curr = next;
        next = fs.fat[next];

        file = open_block(fs, fs.root_block + curr);
        fread(list, sizeof(int), INTS_IN_BLOCK, file);
        fclose(file);

        for (int i = 0; i < min(INTS_IN_BLOCK, left); i++) {
            int check = list[i];
            file = open_block(fs, fs.root_block + check);
            fread(&des, sizeof(des), 1, file);
            fclose(file);
            if ((des.type == T_DIR) && (strcmp(des.title, dir_name) == 0)) {
                remove_dir(fs, check);
                file = open_block(fs, fs.root_block + curr);
                fseek(file, sizeof(int) * i, SEEK_CUR);
                fwrite(&last, sizeof(int), 1, file);
                fclose(file);
                write_fat(fs);
                return 0;
            }
        }

        if (left >= INTS_IN_BLOCK) {
            left -= INTS_IN_BLOCK;
        }
    }
    printf("Something went wrong! File missed");
    return 0;
}
