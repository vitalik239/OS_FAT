#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "general.h"

fs_info fs;
char* path;

int main(int argc, char** argv) {
	fs.root_path = argv[1];
	read_fat(fs);
	path = argv[2];

    Descriptor dir = find_by_name(fs, T_DIR, path);
    Descriptor des;
    int curr_block = dir.fat_pos, descs;
    FILE* file;

    while (true) {
        file = open_block(fs, fs.root_block + curr_block);
        fread(&descs, sizeof(int), 1, file);
        for (int i = 0; i < descs; i++) {
            fread(&des, sizeof(Descriptor), 1, file);
            if (des.type == T_DIR)
                printf("%s directory\n", des.title);
            else
                printf("%s file\n", des.title);
        }
        fclose(file);
        if (curr_block == fs.fat[curr_block]) {
            write_fat(fs);
            return 0;
        }
        curr_block = fs.fat[curr_block];
    }
}
