#include "cachelab.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <getopt.h>

typedef struct {
  bool valid;
  int tag;
  int timestamp;
} line_t; // cache block 구조체

typedef struct {
  line_t *lines;
} set_t;  // line들을 모아 하나의 set을 만듬

typedef struct {
  set_t *sets;
  size_t set_num;  // Number of set
  size_t line_num; // Number of lines per set
} cache_t; // 여러 set이 모여 하나의 cache 공간을 만듬

cache_t cache = {};
int set_bits = 0, block_bits = 0; // Cache metadata
size_t hits = 0, misses = 0, evictions = 0; // Simulation results


void simulate(int addr);
int main(int argc, char *argv[]) {
    //
    // Parse parameter
    //
    FILE *file = 0;
    for (int opt; (opt = getopt(argc, argv, "s:E:b:t:")) != -1;) {
        switch (opt) {
        case 's':
            set_bits = atoi(optarg); // Number of set index bits
            cache.set_num = 2 << set_bits; // Number of sets
            break;
        case 'E':
            cache.line_num = atoi(optarg); // Number of lines per set (set associativity)
            break;
        case 'b':
            block_bits = atoi(optarg); // Number of block bits
            break;
        case 't': // Input filename
            if (!(file = fopen(optarg, "r"))) { return 1; }
            break;
        default:
            // Unknown option
            return 1;
        }
    }
    if (!set_bits || !cache.line_num || !block_bits || !file) { return 1; }


    //
    // Initialize virtual cache
    //
    cache.sets = malloc(sizeof(set_t) * cache.set_num);
    for (int i = 0; i < cache.set_num; ++i) {
        cache.sets[i].lines = calloc(sizeof(line_t), cache.line_num);
    }


    //
    // Parse trace input
    //
    char kind;
    int addr;
    // Parse only kind and access address, ignore access size,
    while (fscanf(file, " %c %x%*c%*d", &kind, &addr) != EOF) {
        // Ignore instruction load
        if (kind == 'I') { continue; }

        simulate(addr);
        if ('M' == kind) { simulate(addr); }
    }
    printSummary(hits, misses, evictions);


    //
    // Clean up
    //
    fclose(file);
    for (size_t i = 0; i < cache.set_num; ++i) { free(cache.sets[i].lines); }
    free(cache.sets);
    return 0;
}


void update(set_t *set, size_t line_no);
void simulate(int addr) {
    //
    // Get set index and tag bits from the address
    //
    //     31                           s+b                  b               0
    //     |             tag             |    set index      |    offset     |
    //
    size_t set_index = (0x7fffffff >> (31 - set_bits)) & (addr >> block_bits);
    int tag = 0xffffffff & (addr >> (set_bits + block_bits));

    // Select a set
    set_t *set = &cache.sets[set_index];

    // Look up for cache hit
    for (size_t i = 0; i < cache.line_num; ++i) {
        line_t* line = &set->lines[i];

        // Check if the cache line is valid
        if (!line->valid) { continue; }
        // Compare tag bits
        if (line->tag != tag) { continue; }

        // Cache hit!
        ++hits;
        update(set, i);
        return;
    }

    // Cache miss!
    ++misses;

    // Look up for empty cache line
    for (size_t i = 0; i < cache.line_num; ++i) {
        line_t* line = &set->lines[i];

        if (line->valid) { continue; }

        line->valid = true;
        line->tag = tag;
        update(set, i);
        return;
    }

    // No empty cache line, eviction!
    ++evictions;

    // Look up for least recently used cache line
    for (size_t i = 0; i < cache.line_num; ++i) {
        line_t* line = &set->lines[i];

        if (line->timestamp) { continue; }

        line->valid = true;
        line->tag = tag;
        update(set, i);
        return;
    }
}


void update(set_t *set, size_t line_no) {
    line_t *line = &set->lines[line_no];

    for (size_t i = 0; i < cache.line_num; ++i) {
        line_t *it = &set->lines[i];
        if (!it->valid) { continue; }
        if (it->timestamp <= line->timestamp) { continue; }

        --it->timestamp;
    }

    line->timestamp = cache.line_num - 1;
}
