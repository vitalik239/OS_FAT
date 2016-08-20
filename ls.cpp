#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "general.h"

fs_info fs;
char* path;

void print(int node) {
	Descriptor des;
	FILE* file = open_block(fs, fs.root_block + node);
	fread(&des, sizeof(des), 1, file);
	fclose(file);

	int curr = node, next = fs.fat[node];
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
			printf("%s ", des.title);
			if (des.type == T_DIR)
				printf("directory\n");
			else
				printf("file\n");
		}

		if (left >= INTS_IN_BLOCK) {
			left -= INTS_IN_BLOCK;
		}
	}

}

int main(int argc, char** argv) {
	fs.root_path = argv[1];
	read_fat(fs);
	path = argv[2];
    int dir_num = find_by_name(fs, T_DIR, path);
	print(dir_num);
	return 0;
}
