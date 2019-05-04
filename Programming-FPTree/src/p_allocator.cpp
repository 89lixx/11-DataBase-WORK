#include"utility/p_allocator.h"
#include<iostream>
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
        // exist
        // TODO
	allocatorCatalog>>this->maxFileId>>this->freeNum>>this->startLeaf.fileId>>this->startLeaf.offset;
        for(int i = 0;i < this->freeNum;++i){
            PPointer pt;
            freeListFile>>pt.fileId>>pt.offset;
            freeList.push_back(pt);
        }
    } else {
        // not exist, create catalog and free_list file, then open.
        // TODO
	ofstream allocatorCatalogout(allocatorCatalogPath, ios::out|ios::binary);
        ofstream freeListFileout(freeListPath, ios::out|ios::binary);
        this->freeNum = 0;
        this->maxFileId = 1;
        this->startLeaf.fileId = 0;
        this->startLeaf.offset = 0;
        allocatorCatalogout.close();
        freeListFileout.close();
    }
    this->initFilePmemAddr();
}

PAllocator::~PAllocator() {
    // TODO
    string allocatorCatalogPath = DATA_DIR + P_ALLOCATOR_CATALOG_NAME;
    string freeListPath         = DATA_DIR + P_ALLOCATOR_FREE_LIST;
    ofstream allocatorCatalog(allocatorCatalogPath, ios::out|ios::binary);
    ofstream freeListFile(freeListPath, ios::out|ios::binary);   
    allocatorCatalog<<maxFileId<<freeNum<<startLeaf.fileId<<startLeaf.offset;
    for(int i = 0;i < freeList.size();++i){
        freeListFile<<freeList[i].fileId<<freeList[i].offset;
    }
    allocatorCatalog.close();
    freeListFile.close();
    this->persistCatalog();
    this->maxFileId = 1;
    freeList.clear();
    fId2PmAddr.clear();	
}

// memory map all leaves to pmem address, storing them in the fId2PmAddr
void PAllocator::initFilePmemAddr() {
    // TODO
    char *pmemaddr;
    size_t mapped_len;
    int is_pmem;
    for(int i = 1;i < this->maxFileId;++i){
        string str = DATA_DIR + to_string(i);
        pmemaddr = pmem_map_file(str, LEAF_GROUP_HEAD + LEAF_GROUP_AMOUNT * calLeafSize(),PMEM_FILE_CREATE|PMEM_FILE_EXCL,0666, &mapped_len, &is_pmem);
        fId2PmAddr[i] = pmemaddr;
    }
}

// get the pmem address of the target PPointer from the map fId2PmAddr
char* PAllocator::getLeafPmemAddr(PPointer p) {
    // TODO
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
    // TODO
    if(freeList.size() == 0)
        this->newLeafGroup();
    p = freeList[freeList.size() - 1];
    pmem_addr = this->getLeafPmemAddr(p);
    string str = DATA_DIR + to_string(p.fileId);
    char *addr = this->getLeafPmemAddr(p);
    addr[sizeof(uint64_t) + (p.offset - LEAF_GROUP_HEAD)/calLeafSize()] = 1;
    char ch[8];
    for(int i = 0;i < 8;++i){
        ch[i] = addr[i];
    }
    int t = 0; 
    while(1){
        if(ch[t] == '9'){
            ch[t] = '0';
        }
        else{
            ch[t] += 1;
            break;
        }
        ++t;
        if(t == 8)
            break;
    }
    for(int i = 0;i < 8;++i){
        addr[i] = ch[i];
    }
    --freeNum;
    freeList.pop_back();
    if(this->startLeaf.fileId == 0){
        this->startLeaf.fileId = p.fileId;
        this->startLeaf.offset = p.offset;
    }
    return true;
}

bool PAllocator::ifLeafUsed(PPointer p) {
    // TODO
    if(p.fileId >= this->maxFileId)
        return false;
    for(int i = 0;i < freeList.size();++i){
        if(p == freeList[i])
            return false;
    }
    return true;
}

bool PAllocator::ifLeafFree(PPointer p) {
    // TODO
    for(int i = 0;i < freeList.size();++i){
        if(p == freeList[i])
            return true;
    }
    return false;
}

// judge whether the leaf with specific PPointer exists. 
bool PAllocator::ifLeafExist(PPointer p) {
    // TODO
    if(p.fileId >= this->maxFileId)
        return false;
    if((p.offset - LEAF_GROUP_HEAD) > 0 &&
       (p.offset - LEAF_GROUP_HEAD) % calLeafSize() == 0 &&
       (p.offset - LEAF_GROUP_HEAD) / calLeafSize() < LEAF_GROUP_AMOUNT)
        return true;
    return false;
}

// free and reuse a leaf
bool PAllocator::freeLeaf(PPointer p) {
    // TODO
    freeList.push_back(p);
    char *addr = getLeafPmemAddr(p);
    addr[sizeof(uint64_t) + (p.offset - LEAF_GROUP_HEAD)/calLeafSize()] = 0;
    char ch[8];
    for(int i = 0;i < 8;++i){
        ch[i] = addr[i];
    }
    int t = 0; 
    while(1){
        if(ch[t] == '0'){
            ch[t] = '9';
        }
        else{
            ch[t] -= 1;
            break;
        }
        ++t;
        if(t == 8)
            break;
    }
    for(int i = 0;i < 8;++i){
        addr[i] = ch[i];
    }
    ++freeNum;
    return true;
}

bool PAllocator::persistCatalog() {
    // TODO
    map<uint64_t, char*>::iterator it;
    it = fId2PmAddr.begin();
    size_t length = LEAF_GROUP_HEAD + LEAF_GROUP_AMOUNT * calLeafSize();
    while(it != fId2PmAddr.end()){
        pmem_persist(it->second, length);
        it ++;
    }
    return true;
}

/*
  Leaf group structure: (uncompressed)
  | usedNum(8b) | bitmap(n * byte) | leaf1 |...| leafn |
*/
// create a new leafgroup, one file per leafgroup
bool PAllocator::newLeafGroup() {
    // TODO
    string str = DATA_DIR + to_string(this->maxFileId);
    ofstream create(str,ios::out|ios::binary);
    uint64_t t = 0;
    create<<t;
    Byte by = 0;
    for(int i = 0;i < LEAF_GROUP_AMOUNT;++i){
        create<<by;
    }
    create.close();
    char *pmemaddr;
    size_t mapped_len;
    int is_pmem;
    pmemaddr = pmem_map_file(str.c_str(), LEAF_GROUP_HEAD + LEAF_GROUP_AMOUNT * calLeafSize(),PMEM_FILE_CREATE|PMEM_FILE_EXCL,0666, &mapped_len, &is_pmem);
    fId2PmAddr[this->maxFileId] = pmemaddr;
    PPointer p;
    for(int j = 0;j < LEAF_GROUP_AMOUNT;++j){
        p.fileId = this->maxFileId;
        p.offset = LEAF_GROUP_HEAD + j * calLeafSize();
        freeList.push_back(p);
    }
    this->maxFileId ++;
    this->freeNum += LEAF_GROUP_AMOUNT;
    return true;
}
