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

const int TITLE_LEN = 100;
const int BLOCK_SIZE = 128;
const int FILE_SIZE = 4096;
const int BUFF_SIZE = BLOCK_SIZE - 20;
const int BLOCKS_IN_FILE = FILE_SIZE / BLOCK_SIZE;
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
    int size;
    Descriptor() {
        for (int i = 0; i < TITLE_LEN; i++) {
            title[i] = 0;
        }
    }
    Descriptor(int sz, int tp) : type(tp), size(sz) {
        for (int i = 0; i < TITLE_LEN; i++) {
            title[i] = 0;
        }
    }
};

FILE* open_block(fs_info& fs, int node);
int get_empty_block(fs_info& fs);
void read_fat(fs_info& fs);
void write_fat(fs_info& fs);
int find_child(fs_info& fs, int node, int type, char* name);
int find_by_name(fs_info& fs, int type, char* path);
int add_child(fs_info& fs, int node);
void remove_file(fs_info& fs, int node);
void remove_dir(fs_info& fs, int node);


#endif
