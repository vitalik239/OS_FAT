#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "general.h"

char* path;
fs_info fs;


int main(int argc, char** argv) {
    fs.root_path = argv[1];
	read_fat(fs);
	path = argv[2];
	char* new_dir = strrchr(path, '/');
	*new_dir = '\0';
	new_dir++;

	Descriptor dir = find_by_name(fs, T_DIR, path);

	Descriptor nw_dir = find_child(fs, dir, T_DIR, new_dir);
	if (nw_dir.fat_pos > 0) {
		fprintf(stderr, "Directory %s exists", new_dir);
		exit(0);
	}

	strcpy(nw_dir.title, new_dir);
	nw_dir.fat_pos = -1;
    nw_dir.type = T_DIR;

	add_child(fs, dir, nw_dir);

	write_fat(fs);

	return 0;	
}
