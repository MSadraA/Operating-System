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
#include <regex>


using namespace std;

const char DELIM = ',';
const char REC_DELIM = '|';


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

const int BUFF_SIZE = 1024;

struct GameRecord {
    string title;
    float original_price;
    float discount_percent;
    int recent_review_summary;
    int all_review_summary;
    int recent_review_number;
    int all_review_number;
};

enum class GameField {
    Title,
    OriginalPrice,
    DiscountPercent,
    RecentReviewSummary,
    AllReviewSummary,
    RecentReviewNumber,
    AllReviewNumber
};

struct WorkerInfo {
    int id;
    int pid;
};

struct Score{
    string title;
    float score;
};

//pipes
const string EXTRACT_PIPE_PATH = "pipes/extract_pipe";
const string REGISTER_PIPE_PATH = "pipes/register_pipe";
const string WORKER_PIPE_PATH = "pipes/worker_pipe_";
const string FINAL_RES_PIPE_PATH = "pipes/final_res_pipe";

#endif