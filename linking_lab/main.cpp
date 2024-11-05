#include <iostream>
#include "leveldb/db.h"


int main(){
    leveldb::DB* db;
    leveldb::Options options;
    options.create_if_missing = true;

    leveldb::Status status = leveldb::DB::Open(options, "mydb", &db);

    if(!status.ok()){
        std::cerr << "Open/Create Error"  << status.ToString() << std::endl;
        return -1;
    }

    std::string key = "Woojin";
    std::string value = "Seoul";

    status = db->Put(leveldb::WriteOptions(), key, value);
    if(!status.ok()){
        std::cerr << "Failed to Write" << status.ToString() << std::endl;
    }

    std::string read_value;
    status = db->Get(leveldb::ReadOptions(), key, &read_value);
    if(!status.ok()){
        std::cerr << "Failed to Get" << status.ToString() << std::endl;
    } else {
        std::cout << "Read :" << read_value << std::endl;
    }

    status = db->Delete(leveldb::WriteOptions(), key);
    if(!status.ok()){
        std::cerr << "Failed to Delete" << status.ToString() << std::endl;
    }
    return 0;

}