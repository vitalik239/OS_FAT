#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "general.h"

fs_info fs;
char* path;
char* import_path;

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

    Descriptor dir = find_by_name(fs, T_DIR, path);

    Descriptor nw_file = find_child(fs, dir, T_FILE, file_name);
    if (nw_file.fat_pos > 0) {
        fprintf(stderr, "File %s exists", file_name);
        exit(0);
    }

    strcpy(nw_file.title, file_name);
    nw_file.type = T_FILE;

    FILE* fin = fopen(import_path, "r");
    size_t bytes;
    char buffer[BLOCK_SIZE];
    int cur = nw_file.fat_pos = get_empty_block(fs);

    bytes = fread(buffer, 1, sizeof(buffer), fin);
    nw_file.file_size += bytes;
    FILE* fout = open_block(fs, fs.root_block + cur);
    fwrite(buffer, 1, bytes, fout);
    fclose(fout);

    while ((bytes = fread(buffer, 1, sizeof(buffer), fin)) > 0) {
        nw_file.file_size += bytes;
        fs.fat[cur] = get_empty_block(fs);
        cur = fs.fat[cur];
        fout = open_block(fs, fs.root_block + cur);
        fwrite(buffer, 1, bytes, fout);
        fclose(fout);
    }
    fclose(fin);

    add_child(fs, dir, nw_file);

    write_fat(fs);
	return 0;
}
