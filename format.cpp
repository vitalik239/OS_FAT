#include <cstdio>
#include <cstdlib>
#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <vector>
#include <unistd.h>
#include <cstring>
#include "general.h"

char* root_path;
int file_count;

void create() {
	char name[100];
	for (int i = 0; i < file_count; i++) {
		sprintf(name, "%s%s%d", root_path, "/block", i);
		FILE* file = fopen(name, "w");
		ftruncate(fileno(file), BLOCK_SIZE);
		fclose(file);
	}
}

void init_fs() {
	fs_info fs;
	int tot_blocks = file_count;

	int fat_blocks = ceil((tot_blocks + 1.0) * 4.0 / (BLOCK_SIZE + 4.0));
	int fat_size = tot_blocks - fat_blocks;

    fs.fat.resize(fat_blocks * INTS_IN_BLOCK);
	for (int i = 0; i < fat_size; i++) {
		fs.fat[i] = -1;
	}
    for (int i = fat_size; i < (int)fs.fat.size(); i++) {
        fs.fat[i] = 0;
    }

    fs.root_block = fat_blocks;
	fs.root_path = root_path;

    fs.fat[0] = 0;
    write_fat(fs);

    int zero = 0;
    FILE* file = open_block(fs, fs.root_block);
    fwrite(&zero, sizeof(int), 1, file);
}

int main(int argc, char** argv) {
	if (argc < 3) 
		printf("Not enough arguments");
	root_path = argv[1];
	file_count = atoi(argv[2]);
	create();
	init_fs();

	return 0;
}
