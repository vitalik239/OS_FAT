#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "general.h"

char* path;
fs_info fs;

void create_dir(int dir_num, char* new_dir) {
	if (find_child(fs, dir_num, T_DIR, new_dir) > 0) {
		fprintf(stderr, "Directory %s exists", new_dir);
	    exit(0);
    }
    int new_block = add_child(fs, dir_num);

    Descriptor des(0, T_DIR);
	strcpy(des.title, new_dir);
	FILE* file = open_block(fs, fs.root_block + new_block);
	fwrite(&des, sizeof(Descriptor), 1, file);
	fclose(file);
}

int main(int argc, char** argv) {
    fs.root_path = argv[1];
	read_fat(fs);
	path = argv[2];
	char* dir_name = strrchr(path, '/');
	int dir_num;
	*dir_name = '\0'; 
	dir_name++;
	dir_num = find_by_name(fs, T_DIR, path);
	create_dir(dir_num, dir_name);
    write_fat(fs);

	return 0;	
}
