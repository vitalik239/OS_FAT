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
	sprintf(name, "%s%s%d", fs.root_path, "/block", node / BLOCKS_IN_FILE);
	FILE* res = fopen(name, "r+");
	fseek(res, (node % BLOCKS_IN_FILE) * BLOCK_SIZE, SEEK_SET);
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
    fs.fat.resize(fs.root_block * INTS_IN_BLOCK);
    fread(&fs.fat[0], sizeof(int), INTS_IN_BLOCK - 1, file);
    int fat_left = fs.root_block - 1, curr = 1;
    while (fat_left > 0) {
        fread(&fs.fat[INTS_IN_BLOCK * curr - 1], sizeof(int), INTS_IN_BLOCK, file);
        curr++;
        fat_left--;
        if (curr % BLOCKS_IN_FILE == 0) {
            fclose(file);
            file = open_block(fs, curr);
        }
    }
    fclose(file);
}

void write_fat(fs_info& fs) {
    FILE* file = open_block(fs, 0);
    fwrite(&fs.root_block, sizeof(int), 1, file);

    fwrite(&fs.fat[0], sizeof(int), INTS_IN_BLOCK - 1, file);
    int fat_left = fs.root_block - 1, curr = 1;

    while (fat_left > 0) {
        fwrite(&fs.fat[INTS_IN_BLOCK * curr - 1], sizeof(int), INTS_IN_BLOCK, file);
        curr++;
        fat_left--;
        if (curr % BLOCKS_IN_FILE == 0) {
            fclose(file);
            file = open_block(fs, curr);
        }
    }

    fclose(file);
}


int find_child(fs_info& fs, int node, int type, char* name) {
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
            if ((des.type == type) && (strcmp(des.title, name) == 0)) {
                return check;
            }
        }

        if (left >= INTS_IN_BLOCK) {
            left -= INTS_IN_BLOCK;
        }
    }
    return -1;
}


int find_by_name(fs_info& fs, int type, char* path) {
    int curr_node = 0;
    int child;
    char* next = strtok(path, "/");
    char* curr;
    while (next != NULL) {
        curr = next;
        next = strtok(NULL, "/");
        if (next != NULL)
            child = find_child(fs, curr_node, T_DIR, curr);
        else
            child = find_child(fs, curr_node, type, curr);
        if (child < 0) {
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

int add_child(fs_info& fs, int node) {
    FILE* file = open_block(fs, fs.root_block + node);
    Descriptor des;
    fread(&des, sizeof(Descriptor), 1, file);
    int empty;
    if (des.size % INTS_IN_BLOCK == 0) {
        empty = get_empty_block(fs);
    }
    fseek(file, -sizeof(Descriptor), SEEK_CUR);
    des.size++;
    fwrite(&des, sizeof(Descriptor), 1, file);
    des.size--;
    fclose(file);

    int cur = node;
    while (fs.fat[cur] != cur)
        cur = fs.fat[cur];
    if (des.size % INTS_IN_BLOCK == 0) {
        fs.fat[cur] = empty;
        cur = empty;
    }

    file = open_block(fs, fs.root_block + cur);
    fseek(file, sizeof(int) * (des.size % INTS_IN_BLOCK), SEEK_CUR);
    int child = get_empty_block(fs);
    fwrite(&child, sizeof(int), 1, file);
    fclose(file);
    return child;
}

void remove_file(fs_info& fs, int node) {
    int cur = node, prev = node;
    while (cur != fs.fat[cur]) {
        cur = fs.fat[cur];
        fs.fat[prev] = -1;
        prev = cur;
    }
}


void remove_dir(fs_info& fs, int node) {
    Descriptor des;
    FILE* file = open_block(fs, fs.root_block + node);
    fread(&des, sizeof(des), 1, file);
    fclose(file);

    int curr = node, next = fs.fat[node];
    int list[INTS_IN_BLOCK];
    int left = des.size;

    while (curr != next) {
        fs.fat[curr] = -1;
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
            if (des.type == T_FILE)
                remove_file(fs, check);
            else
                remove_dir(fs, check);
        }

        if (left >= INTS_IN_BLOCK) {
            left -= INTS_IN_BLOCK;
        }
    }
    fs.fat[curr] = -1;
}
