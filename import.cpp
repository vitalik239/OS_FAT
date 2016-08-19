#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "general.h"

fs_info fs;
char* path;
char* import_path;

void create_file(int dir_num, char* file_name) {
	if (find_child(fs, dir_num, T_FILE, file_name) != -1) {
        fprintf(stderr, "File with such name %s already exists\n", file_name);
		exit(0);	
	}
	
	int new_block = add_child(fs, dir_num);

	Descriptor des(0, T_FILE);
	strcpy(des.title, file_name);

	FILE* fin = fopen(import_path, "r");
	size_t bytes;
	int cur = new_block;
	char buffer[BLOCK_SIZE];
	while ((bytes = fread(buffer, 1, sizeof(buffer), fin)) > 0) {
		des.size += bytes;
		int next_block = get_empty_block(fs);
		fs.fat[cur] = next_block;
		cur = next_block;
		FILE* fout = open_block(fs, fs.root_block + next_block);
		fwrite(buffer, 1, bytes, fout);
		fclose(fout);	
	}
	fclose(fin);

	FILE* file = open_block(fs, fs.root_block + new_block);
	fwrite(&des, sizeof(Descriptor), 1, file);
	fclose(file);
}


int main(int agrc, char** argv) {
	fs.root_path = argv[1];
	read_fat(fs);

	import_path = argv[2];
	path = argv[3];
	
	char* file_name = strrchr(path, '/');
	if (file_name == NULL) {
		fprintf(stderr, "Add '/' before the filename\n");
		return 0;	
	} else {
		*file_name = '\0';
		file_name++;
	}	
	int parent_dir = find_by_name(fs, T_DIR, path);
	create_file(parent_dir, file_name);

    write_fat(fs);
	return 0;
}
