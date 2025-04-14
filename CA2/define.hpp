#ifndef DEFINE_H
#define DEFINE_H

#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <unordered_map> 
#include <iostream>
#include <fcntl.h> 
#include <unistd.h>
#include <cstdio>
#include <sys/stat.h>
#include <sys/wait.h> 
#include <sys/stat.h> 
#include <cstdlib>




using namespace std;

const char DELIM = ',';

enum {
    TITLE = 0,
    ORIGINAL_PRICE,
    DISCOUNTED_PRICE,
    LINK,//
    DESCRIPTION,//
    RECENT_REVIEW_SUMMARY,
    ALL_REVIEW_SUMMARY,
    RECENT_REVIEW_NUMBER,
    ALL_REVIEW_NUMBER,
    DEVELOPER,//
    PUBLISHER,//
    TAGS,//
    FEATURES,//
    MIN_REQUIREMENTS//
};

struct GameRecord {
    string title;
    float original_price;
    float discount_percent;
    int recent_review_summary;
    int all_review_summary;
    int recent_review_number;
    int all_review_number;
};

#endif