#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "general.h"

fs_info fs;
char* path;
char* export_path;

void export_file(int file_num) {
    FILE* fout = fopen(export_path, "w");
	FILE* fin = open_block(fs, fs.root_block + file_num);
	Descriptor des;
	fread(&des, sizeof(Descriptor), 1, fin);
	fclose(fin);

    int curr = file_num, next = fs.fat[file_num];
	char buffer[BLOCK_SIZE];
	int left = des.size;

	while (curr != next) {
		curr = next;
		next = fs.fat[next];

		fin = open_block(fs, fs.root_block + curr);
		fread(buffer, sizeof(char), min(BLOCK_SIZE, left), fin);
        fclose(fin);

		fwrite(buffer, sizeof(char), min(BLOCK_SIZE, left), fout);

        if (left >= BLOCK_SIZE) {
			left -= BLOCK_SIZE;
		}
	}
	fclose(fout);
}


int main(int agrc, char** argv) {
	fs.root_path = argv[1];
	export_path = argv[3];
	path = argv[2];
	read_fat(fs);
	int file_num = find_by_name(fs, T_FILE, path);
	export_file(file_num);

	return 0;
}
