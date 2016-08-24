#ifndef GENERAL_H
#define GENERAL_H

#include <cstdio>
#include <cstdlib>
#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <vector>

using namespace std;

const int TITLE_LEN = 256;
const int BLOCK_SIZE = 4096;
const int T_EMPTY = 3, T_DIR = 1, T_FILE = 2;
const int INTS_IN_BLOCK = BLOCK_SIZE / sizeof(int);

struct fs_info {
    const char* root_path;
    int root_block;
    vector<int> fat;
};

struct Descriptor {
    char title[TITLE_LEN];
    int type;
    int file_size;
    int fat_pos;
    Descriptor() {
        for (int i = 0; i < TITLE_LEN; i++) {
            title[i] = 0;
        }
    }
};

const int DESC_IN_FILE = BLOCK_SIZE / sizeof(Descriptor);

FILE* open_block(fs_info& fs, int node);
int get_empty_block(fs_info& fs);
void read_fat(fs_info& fs);
void write_fat(fs_info& fs);
Descriptor find_child(fs_info& fs, Descriptor parent, int type, char* name);
Descriptor find_by_name(fs_info& fs, int type, char* path);
void add_child(fs_info& fs, Descriptor dir, Descriptor child);
void remove_file(fs_info& fs, Descriptor filq);
void remove_dir(fs_info& fs, Descriptor dir);


#endif
