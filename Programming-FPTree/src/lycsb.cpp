#include <leveldb/db.h>
#include <string>
#include <string.h>
#include <fstream>
#include <iostream>
#include <cctype>
#include <cstdlib>
#define KEY_LEN 8
#define VALUE_LEN 8
using namespace std;

const string workload = "/home/lixx/Desktop/Programming-FPTree/workloads";

const string load = workload + "/1w-rw-50-50-load.txt"; // TODO: the workload_load filename
const string run  = workload + "/1w-rw-50-50-run.txt"; // TODO: the workload_run filename

const string filePath = "";

const int READ_WRITE_NUM = 10000; // TODO: how many operations

// char * transform(string s) {
//     char * res;
//     for(int i = 7; i < s.size(); ++ i) {
//         res += s[i];
//     }
//     return res;
// }
// pair<string, string> transform_(string s) {
//     pair<string, string> res;
//     string s1,s2;
//     int i = 0;
//     for(; i < s.size(); ++ i) {
//         if(s[i] == ' ') break;
//         s1 += s[i];
//     }
//     for(i = i + 1; i < s.size(); ++ i) {
//         s2 += s[i];
//     }
//     return make_pair(s1,s2);
// }
int main()
{        
    leveldb::DB* db;
    leveldb::Options options;
    leveldb::WriteOptions write_options;
    // TODO: open and initial the levelDB
    options.create_if_missing = true;
    leveldb::Status status = leveldb::DB::Open(options, "/tmp/testdb", &db);
    assert(status.ok());

    uint64_t inserted = 0, queried = 0, t = 0;
    uint64_t* key = new uint64_t[2200000]; // the key and value are same
    bool* ifInsert = new bool[2200000]; // the operation is insertion or not
	ifstream ycsb_load, ycsb_run; // the files that store the ycsb operations
    // char * buf = NULL;
    char buff[6];
	size_t len = 0;
    struct timespec start, finish; // use to caculate the time
    double single_time; // single operation time

    printf("Load phase begins \n");
    // TODO: read the ycsb_load and store
    // ifstream ycsb_load;
    ycsb_load.open(load);
    int count = 0;
    for(int i = 0; i < READ_WRITE_NUM; ++ i) {
        ycsb_load>>buff;
        ycsb_load>>key[i];
        ifInsert[i] = true;
        inserted++;
    }
    ycsb_load.close();
    clock_gettime(CLOCK_MONOTONIC, &start);

    // TODO: load the workload in LevelDB
    for(int i = 0; i < READ_WRITE_NUM; ++ i) {
        string va;
        va = to_string(key[i]);
        status = db->Put(leveldb::WriteOptions(), va, va);
        assert(status.ok());
    }
    clock_gettime(CLOCK_MONOTONIC, &finish);
	single_time = (finish.tv_sec - start.tv_sec) * 1000000000.0 + (finish.tv_nsec - start.tv_nsec);

    printf("Load phase finishes: %d items are inserted \n", inserted);
    printf("Load phase used time: %fs\n", single_time / 1000000000.0);
    printf("Load phase single insert time: %fns\n", single_time / inserted);

	int operation_num = 0;
    inserted = 0;		

    // TODO:read the ycsb_run and store

    // ifstream ycsb_run(run);
    string s;
   ycsb_run.open(run);
    for(int i = 0; i < READ_WRITE_NUM; ++ i) {
        ycsb_run>>buff;
        ycsb_run>>key[i];
        if(buff[0] == 'R') ifInsert[i] = false;
        else {
            ifInsert[i] = true;
            inserted ++;
        }
        operation_num++;
    }

    clock_gettime(CLOCK_MONOTONIC, &start);

    // TODO: operate the levelDB
    for(int i = 0; i < operation_num; ++ i) {
        string va = to_string(key[i]);
        if(!ifInsert[i]){
            status = db->Get(leveldb::ReadOptions(), va, &va);
             assert(status.ok());
        }
        else {
            status = db->Put(leveldb::WriteOptions(), va,va);
            assert(status.ok());
        }
    }



	clock_gettime(CLOCK_MONOTONIC, &finish);
	single_time = (finish.tv_sec - start.tv_sec) + (finish.tv_nsec - start.tv_nsec) / 1000000000.0;
    printf("Run phase finishes: %d/%d items are inserted/searched\n", operation_num - inserted, inserted);
    printf("Run phase throughput: %f operations per second \n", READ_WRITE_NUM/single_time);	
    return 0;
}
