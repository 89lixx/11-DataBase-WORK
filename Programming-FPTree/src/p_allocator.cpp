#include "utility/p_allocator.h"
#include <iostream>
#include <cstring>
using namespace std;

// the file that store the information of allocator
const string P_ALLOCATOR_CATALOG_NAME = "p_allocator_catalog";
// a list storing the free leaves
const string P_ALLOCATOR_FREE_LIST    = "free_list";

PAllocator* PAllocator::pAllocator = new PAllocator();

PAllocator* PAllocator::getAllocator() {
    if (PAllocator::pAllocator == NULL) {
        PAllocator::pAllocator = new PAllocator();
    }
    return PAllocator::pAllocator;
}

/* data storing structure of allocator
   In the catalog file, the data structure is listed below
   | maxFileId(8 bytes) | freeNum = m | treeStartLeaf(the PPointer) |
   In freeList file:
   | freeList{(fId, offset)1,...(fId, offset)m} |
*/
PAllocator::PAllocator() {
    string allocatorCatalogPath = DATA_DIR + P_ALLOCATOR_CATALOG_NAME;
    string freeListPath         = DATA_DIR + P_ALLOCATOR_FREE_LIST;
    ifstream allocatorCatalog(allocatorCatalogPath, ios::in|ios::binary);
    ifstream freeListFile(freeListPath, ios::in|ios::binary);
    // judge if the catalog exists
    if (allocatorCatalog.is_open() && freeListFile.is_open()) {
        //TODO
        allocatorCatalog.read((char *)&(this->maxFileId),sizeof(this->maxFileId));
        allocatorCatalog.read((char *)&(this->freeNum),sizeof(this->freeNum));
        allocatorCatalog.read((char *)&(this->startLeaf.fileId),sizeof(this->startLeaf.fileId));
        allocatorCatalog.read((char *)&(this->startLeaf.offset),sizeof(this->startLeaf.offset));
        this->freeList.resize(freeNum);
        for(int i = 0;i < this->freeNum;++i){
            freeListFile.read((char *)&(this->freeList[i].fileId),sizeof(this->freeList[i].fileId));
            freeListFile.read((char *)&(this->freeList[i].offset),sizeof(this->freeList[i].offset));
        }
        allocatorCatalog.close();
        freeListFile.close();
    } else {
        // not exist, create catalog and free_list file, then open.
        //TODO
        ofstream allocatorCatalogout(allocatorCatalogPath, ios::out|ios::binary);
        ofstream freeListFileout(freeListPath, ios::out|ios::binary);
        this->freeNum = 0;
        this->maxFileId = 1;
        this->startLeaf.fileId = 0;
        this->startLeaf.offset = LEAF_GROUP_HEAD;
        allocatorCatalogout.close();
        freeListFileout.close();
    }
    this->initFilePmemAddr();
}

PAllocator::~PAllocator() {
    //TODO
    this->persistCatalog();
    this->pAllocator = NULL;
}

// memory map all leaves to pmem address, storing them in the fId2PmAddr
void PAllocator::initFilePmemAddr() {
    //TODO
    char *pmemaddr;
    size_t mapped_len;
    int is_pmem;
    for(int i = 1;i < this->maxFileId;++i){
        string str = DATA_DIR + to_string(i);
        pmemaddr = (char *)pmem_map_file(str.c_str(), LEAF_GROUP_HEAD + LEAF_GROUP_AMOUNT * calLeafSize(),PMEM_FILE_CREATE|PMEM_FILE_EXCL,0666, &mapped_len, &is_pmem);
        fId2PmAddr[i] = pmemaddr;
        pmem_persist(pmemaddr,mapped_len);
    }
}

// get the pmem address of the target PPointer from the map fId2PmAddr
char* PAllocator::getLeafPmemAddr(PPointer p) {
    //TODO
    map<uint64_t, char*>::iterator it;
    it = fId2PmAddr.begin();
    while(it != fId2PmAddr.end()){
        if(it->first == p.fileId)
            return it->second + p.offset;
        it ++;
    }
    return NULL;
}

// get and use a leaf for the fptree leaf allocation
// return 
bool PAllocator::getLeaf(PPointer &p, char* &pmem_addr) {
    //TODO
    if(freeList.size() == 0)
        if(this->newLeafGroup() == false){
            return false;
        };
    p.fileId = freeList[freeNum - 1].fileId;
    p.offset = freeList[freeNum - 1].offset;
    pmem_addr = this->getLeafPmemAddr(p);
    uint64_t *used = (uint64_t*)fId2PmAddr[p.fileId];
    *used += 1;
    fId2PmAddr[p.fileId] = (char*)used;
    fId2PmAddr[p.fileId] += ((p.offset - LEAF_GROUP_HEAD) / calLeafSize() + sizeof(uint64_t));
    *fId2PmAddr[p.fileId] = 1;
    fId2PmAddr[p.fileId] -= ((p.offset - LEAF_GROUP_HEAD) / calLeafSize() + sizeof(uint64_t));
    pmem_persist(fId2PmAddr[p.fileId], LEAF_GROUP_HEAD + LEAF_GROUP_AMOUNT * calLeafSize());
    --freeNum;
    freeList.pop_back();
    return true;
}

bool PAllocator::ifLeafUsed(PPointer p) {
    //TODO
    if(ifLeafExist(p))
        if(!ifLeafFree(p))
            return true;
    return false;
}

bool PAllocator::ifLeafFree(PPointer p) {
    //TODO
    for(int i = 0;i < freeList.size();++i){
        if(p == freeList[i])
            return true;
    }
    return false;
}

// judge whether the leaf with specific PPointer exists. 
bool PAllocator::ifLeafExist(PPointer p) {
    //TODO
    if(p.fileId < 0 || p.fileId >= this->maxFileId)
        return false;
    if((p.offset - LEAF_GROUP_HEAD) > 0 &&
       (p.offset - LEAF_GROUP_HEAD) % calLeafSize() == 0 &&
       (p.offset - LEAF_GROUP_HEAD) / calLeafSize() < LEAF_GROUP_AMOUNT)
        return true;
    return false;
}

// free and reuse a leaf
bool PAllocator::freeLeaf(PPointer p) {
    //TODO
    if(ifLeafUsed(p)){
        freeNum++;
        freeList.push_back(p);
        fId2PmAddr[p.fileId][(p.offset - LEAF_GROUP_HEAD) / calLeafSize() + sizeof(uint64_t)] = 0;
        ((uint64_t*)this->fId2PmAddr[p.fileId])[0] -= 1;
        return true;
    }
    return false;
}

bool PAllocator::persistCatalog() {
    //TODO
    string allocatorCatalogPath = DATA_DIR + P_ALLOCATOR_CATALOG_NAME;
    string freeListPath         = DATA_DIR + P_ALLOCATOR_FREE_LIST;
    ofstream allocatorCatalog(allocatorCatalogPath, ios::out|ios::binary);
    ofstream freeListFile(freeListPath, ios::out|ios::binary);
    if (allocatorCatalog.is_open() && freeListFile.is_open()) {
        allocatorCatalog.write((char *)&(this->maxFileId),sizeof(this->maxFileId));
        allocatorCatalog.write((char *)&(this->freeNum),sizeof(this->freeNum));
        allocatorCatalog.write((char *)&(this->startLeaf.fileId),sizeof(this->startLeaf.fileId));
        allocatorCatalog.write((char *)&(this->startLeaf.offset),sizeof(this->startLeaf.offset));
        for(int i = 0;i < this->freeNum;++i){
            freeListFile.write((char *)&(this->freeList[i].fileId),sizeof(this->freeList[i].fileId));
            freeListFile.write((char *)&(this->freeList[i].offset),sizeof(this->freeList[i].offset));
        }
        allocatorCatalog.close();
        freeListFile.close();
        return true;
    } else {
        return false;
    }
}

/*
  Leaf group structure: (uncompressed)
  | usedNum(8b) | bitmap(n * byte) | leaf1 |...| leafn |
*/
// create a new leafgroup, one file per leafgroup
bool PAllocator::newLeafGroup() {
    //TODO
    string str = DATA_DIR + to_string(this->maxFileId);
    ofstream newleaf(str,ios::out|ios::binary);
    if(!newleaf){
        persistCatalog();
        return false;
    }
    else{
        uint64_t usedNum = 0;
        char bitmap[LEAF_GROUP_AMOUNT];
        char leafs[LEAF_GROUP_AMOUNT * calLeafSize()];
        for(int i = 0;i < LEAF_GROUP_AMOUNT;++i){
            bitmap[i] = '0';
        }
        memset(leafs, 0, sizeof(leafs));
        newleaf.write((char*)&(usedNum), sizeof(usedNum));
        newleaf.write((char*)bitmap, sizeof(bitmap));
        newleaf.write((char*)&leafs, sizeof(leafs));
        char *pmemaddr;
        size_t mapped_len;
        int is_pmem;
        string str = DATA_DIR + to_string(maxFileId);
        pmemaddr = (char *)pmem_map_file(str.c_str(), LEAF_GROUP_HEAD + LEAF_GROUP_AMOUNT * calLeafSize(),PMEM_FILE_CREATE|PMEM_FILE_EXCL,0666, &mapped_len, &is_pmem);
        fId2PmAddr[maxFileId] = pmemaddr;
        PPointer pp;
        pp.fileId = maxFileId;
        for (int i = 0; i < LEAF_GROUP_AMOUNT; ++i) {
            pp.offset = LEAF_GROUP_HEAD + i * calLeafSize();
            freeList.push_back(pp);
        }
        if(maxFileId == 1) {
            this->startLeaf.fileId = 1;
            this->startLeaf.offset = LEAF_GROUP_HEAD + (LEAF_GROUP_AMOUNT - 1) * calLeafSize();
        }
        this->maxFileId ++;
        newleaf.close();
        persistCatalog();
        return true;
    }
}
