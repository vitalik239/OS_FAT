#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "general.h"

fs_info fs;
char* path;
char* export_path;


int main(int agrc, char** argv) {
	fs.root_path = argv[1];
	export_path = argv[3];
	path = argv[2];
	read_fat(fs);

	Descriptor file = find_by_name(fs, T_FILE, path);

	FILE* fout = fopen(export_path, "w");
	FILE* fin;

	int curr = file.fat_pos, next = fs.fat[file.fat_pos];
	char buffer[BLOCK_SIZE];
	int left = file.file_size;

	while (left > 0) {
		fin = open_block(fs, fs.root_block + curr);
		fread(buffer, sizeof(char), min(BLOCK_SIZE, left), fin);
		fclose(fin);

		fwrite(buffer, sizeof(char), min(BLOCK_SIZE, left), fout);

		if (left >= BLOCK_SIZE) {
			left -= BLOCK_SIZE;
		} else {
			left = 0;
		}
		curr = next;
		next = fs.fat[next];
	}
	fclose(fout);

	return 0;
}
