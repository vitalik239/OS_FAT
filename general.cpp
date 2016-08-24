#include <cstdio>
#include <cstdlib>
#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <vector>
#include <cstring>
#include "general.h"


FILE* open_block(fs_info& fs, int node) {
	char name[256];	
	sprintf(name, "%s%s%d", fs.root_path, "/block", node);
	FILE* res = fopen(name, "r+");
	return res;
}


int get_empty_block(fs_info& fs) {
	for (int i = 1; i < (int)fs.fat.size(); i++) {
        if (fs.fat[i] == -1) {
            fs.fat[i] = i;
            return i;
        }
    }
    fprintf(stderr, "Not enough space");
    exit(0);
}

void read_fat(fs_info& fs) {
    FILE* file = open_block(fs, 0);
    fread(&fs.root_block, sizeof(int), 1, file);

    fs.fat.resize(fs.root_block * INTS_IN_BLOCK - 1);
    fread(&fs.fat[0], sizeof(int), INTS_IN_BLOCK - 1, file);
    fclose(file);

    int curr = 1;
    while (curr < fs.root_block) {
        file = open_block(fs, curr);
        fread(&fs.fat[INTS_IN_BLOCK * curr - 1], sizeof(int), INTS_IN_BLOCK, file);
        curr++;
        fclose(file);
    }
}

void write_fat(fs_info& fs) {
    FILE* file = open_block(fs, 0);
    fwrite(&fs.root_block, sizeof(int), 1, file);
    fwrite(&fs.fat[0], sizeof(int), INTS_IN_BLOCK - 1, file);
    fclose(file);

    int curr = 1;
    while (curr < fs.root_block) {
        file = open_block(fs, curr);
        fwrite(&fs.fat[INTS_IN_BLOCK * curr - 1], sizeof(int), INTS_IN_BLOCK, file);
        curr++;
        fclose(file);
    }
}


Descriptor find_child(fs_info& fs, Descriptor parent, int type, char* name) {
    Descriptor res;
    int curr_block = parent.fat_pos, descs;
    FILE* file;

    while (true) {
        file = open_block(fs, fs.root_block + curr_block);
        fread(&descs, sizeof(int), 1, file);
        for (int i = 0; i < descs; i++) {
            fread(&res, sizeof(Descriptor), 1, file);
            if ((res.type == type) && (strcmp(res.title, name) == 0)) {
                if (res.fat_pos == -1) {
                    res.fat_pos = get_empty_block(fs);
                    int zero = 0;
                    FILE* pos = open_block(fs, fs.root_block + res.fat_pos);
                    fwrite(&zero, sizeof(int), 1, pos);
                    fclose(pos);
                    fseek(file, -sizeof(Descriptor), SEEK_CUR);
                    fwrite(&res, sizeof(Descriptor), 1, file);
                }
                fclose(file);
                return res;
            }
        }
        fclose(file);

        if (curr_block == fs.fat[curr_block]) {
            res.fat_pos = -1;
            return res;
        }
        curr_block = fs.fat[curr_block];
    }
}


Descriptor find_by_name(fs_info& fs, int type, char* path) {
    Descriptor curr_node;
    curr_node.fat_pos = 0;

    Descriptor child;
    char* next = strtok(path, "/");
    char* curr;

    while (next != NULL) {
        curr = next;
        next = strtok(NULL, "/");
        if (next != NULL)
            child = find_child(fs, curr_node, T_DIR, curr);
        else
            child = find_child(fs, curr_node, type, curr);
        if (child.fat_pos == -1) {
            if (type == T_DIR)
                printf("Directory %s doesn't exist\n", curr);
            else
                printf("File %s doesn't exist\n", curr);
            exit(0);
        } else {
            curr_node = child;
        }
    }
    return curr_node;
}


void add_child(fs_info& fs, Descriptor dir, Descriptor child) {
    FILE* file;

    int cur = dir.fat_pos, descs;
    while (fs.fat[cur] != cur)
        cur = fs.fat[cur];

    file = open_block(fs, fs.root_block + cur);
    fread(&descs, sizeof(int), 1, file);
    if (descs == DESC_IN_FILE) {
        fclose(file);
        fs.fat[cur] = get_empty_block(fs);
        cur = fs.fat[cur];
        file = open_block(fs, fs.root_block + cur);
        descs = 1;
        fwrite(&descs, sizeof(int), 1, file);
    } else {
        fseek(file, -sizeof(int), SEEK_CUR);
        descs++;
        fwrite(&descs, sizeof(int), 1, file);
        descs--;
        fseek(file, sizeof(Descriptor) * descs, SEEK_CUR);
    }

    fwrite(&child, sizeof(Descriptor), 1, file);
    fclose(file);
}

void remove_file(fs_info& fs, Descriptor file) {
    int cur = file.fat_pos, prev = cur;
    while (cur != fs.fat[cur]) {
        cur = fs.fat[cur];
        fs.fat[prev] = -1;
        prev = cur;
    }
    fs.fat[cur] = -1;
}


void remove_dir(fs_info& fs, Descriptor dir) {
    Descriptor des;
    int curr_block = dir.fat_pos, descs, temp;
    if (dir.fat_pos == -1)
        return;
    FILE* file;

    while (true) {
        file = open_block(fs, fs.root_block + curr_block);
        fread(&descs, sizeof(int), 1, file);
        for (int i = 0; i < descs; i++) {
            fread(&des, sizeof(Descriptor), 1, file);
            if (des.type == T_DIR)
                remove_dir(fs, des);
            else
                remove_file(fs, des);
        }
        fclose(file);
        if (curr_block == fs.fat[curr_block]) {
            fs.fat[curr_block] = -1;
            return;
        }
        temp = curr_block;
        curr_block = fs.fat[curr_block];
        fs.fat[temp] = -1;
    }
}