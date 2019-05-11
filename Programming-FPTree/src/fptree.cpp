#include"fptree/fptree.h"

using namespace std;

// Initial the new InnerNode
InnerNode::InnerNode(const int& d, FPTree* const& t, bool _isRoot) {
    // TODO
}

// delete the InnerNode
InnerNode::~InnerNode() {
    // TODO
}

// binary search the first key in the innernode larger than input key
int InnerNode::findIndex(const Key& k) {
    // TODO
    return 0;
}

// insert the node that is assumed not full
// insert format:
// ======================
// | key | node pointer |
// ======================
// WARNING: can not insert when it has no entry
void InnerNode::insertNonFull(const Key& k, Node* const& node) {
    // TODO
    int i;
    for(i = 0; i < this->nKeys; ++ i) {
        if(this->keys[i] == k) return; //重复
        if(this->keys[i] > k) {
            break;
        }
        i++;
    }
    for(int j = this->nKeys; j > i; -- j) {
        this->keys[j] = this->keys[j-1];
        this->childrens[j] = this->childrens[j-1];
    }
    this->keys[i] = k;
    this->childrens[i] = node;
}

// insert func
// return value is not NULL if split, returning the new child and a key to insert
KeyNode* InnerNode::insert(const Key& k, const Value& v) {
    KeyNode* newChild = NULL;

    // 1.insertion to the first leaf(only one leaf)
    if (this->isRoot && this->nKeys == 0) {
        // TODO
        LeafNode* node = new LeafNode(tree);
        node -> insert(k, v);
        insertNonFull(k, node);
        return newChild;
    }
    
    // 2.recursive insertion
    // TODO
    int find = findIndex(k);
    newChild = childrens[find]->insert(k, v);
    if(newChild != NULL) {
        if(nKeys > 2 * degree) {
            insertNonFull(newChild.key, newChild.node);
            newChild = split();
            if(this->isRoot) {
                InnerNode* newRoot = new InnerNode(this->degree, tree, true);
                isRoot = false;
                newRoot->insertNonFull(newChild.key, this);
                newRoot->insertNonFull(newChild.key, newChild.node);
                this->tree->root = nowRoot;
            }
        } else {
            insertNonFull(newChild.key, newChild.node);
            KeyNode* newChild = NULL;
        }
    }
    return newChild;
}

// ensure that the leaves inserted are ordered
// used by the bulkLoading func
// inserted data: | minKey of leaf | LeafNode* |
KeyNode* InnerNode::insertLeaf(const KeyNode& leaf) {
    KeyNode* newChild = NULL;
    // first and second leaf insertion into the tree
    if (this->isRoot && this->nKeys == 0) {
        // TODO
        newChild->Key = leaf->key;
        newChild->node = leaf->node;
        this->childrens[0] = (Node *)leaf;
        // this->
        return newChild;
    }
    
    // recursive insert
    // Tip: please judge whether this InnerNode is full
    for(int i = 0; i < this->nKeys-1; ++ i) {
        if(this->keys[i] == leaf->key || this->keys[i+1] == leaf->key) return newChild;
        //1、如果是直接已经存在的点，那么就不需要再加入了，直接返回就行了


        //2、最一般的情况，找到并直接插入，不满
        if(this->keys[i] < leaf->key && this->keys[i+1] > leaf->key) {
           

            //找到了对应的位置了
            if(this->nChild == 0 && this->nKeys < (2*this->degree+1)) { //这一层正好为页,且不满
                //直接插入即可，但是要按照准顺序插入
                int flag = 0; //用来找到插入的位置，然后后面的数据都向后移
                for(; flag < this->nKeys; ++ flag) {
                    if(leaf->key > this->keys[flag]) break;
                }
                // Key temp = this->keys[flag];
                // this->keys[flag] = leaf->key;
                for(int j = this->nKeys; j > flag+1; -- j) {
                    this->keys[j] = this->kesy[j-1];
                }
                this->key[flag] = leaf->key;
                this->nKeys ++;

                return NULL;
            }

           
            //3 这一层为叶，但是满了，要重新分配
            //规则为：现在的假设为 1，2，3，4，5。插入的是6，那么要将中间的值分裂，中间的
            //值放到父节点，
            else if(this->nChild == 0 && this->nKeys == (2*this->degree+1)) {
                int nkey = this->nKeys+1;
                int mid = (this->nKeys+1)/2;
                Key * temp = this->keys;
                int length = this->nKeys;
                // this->keys = new int[mid];
                //分裂
                InnerNode * left(this->degree, this->f(FpTree), false);
                left->keys = new keys[mid];
                left->nKeys = mid;
                left->nChild = 0;
                for(int j = 0; j < mid; ++ j) left->keys[j] = this->keys[j];
                InnerNode * right(this->degree, this->f(FpTree), false);
                right->nKeys = nkey - mid;
                right->nChild = 0;
                for(int j = 0; j < right; ++ j) right->keys[j] = this->keys[j+mid];
                delete this->keys;
                this->keys = new int[1]
                this->nKeys += 1;
                this->nChild += 2;
                this->keys = right->keys[0];
                this->childrens = new int[2];
                this->childrens[0] = (Node*)left;
                this->childrens[1] = (Node*)right;
                leaf->key = left->getKey();
                return leaf;

            }

            // //这一层不是叶，那么需要再次向下找
            // else if(this->nChild != 0) {
            //     //这个时候使用递归，进行广度遍历
            //     for(int j = 0; j < nChild; ++ j) {
            //         this = (Node*)this->childrens[j];
            //         return insertLeaf(leaf);
            //     }

            // }
        }
    }

    // next level is not leaf, just insertLeaf
    // TODO
    // next level is leaf, insert to childrens array
    // TODO

    return newChild;
}

KeyNode* InnerNode::split() {
    KeyNode* newChild = new KeyNode();
    // right half entries of old node to the new node, others to the old node. 
    // TODO

    return newChild;
}

// remove the target entry
// return TRUE if the children node is deleted after removement.
// the InnerNode need to be redistributed or merged after deleting one of its children node.
bool InnerNode::remove(const Key& k, const int& index, InnerNode* const& parent, bool &ifDelete) {
    bool ifRemove = false;
    // only have one leaf
    // TODO
    
    // recursive remove
    // TODO
    return ifRemove;
}

// If the leftBro and rightBro exist, the rightBro is prior to be used
void InnerNode::getBrother(const int& index, InnerNode* const& parent, InnerNode* &leftBro, InnerNode* &rightBro) {
    // TODO
}

// merge this node, its parent and left brother(parent is root)
void InnerNode::mergeParentLeft(InnerNode* const& parent, InnerNode* const& leftBro) {
    // TODO
}

// merge this node, its parent and right brother(parent is root)
void InnerNode::mergeParentRight(InnerNode* const& parent, InnerNode* const& rightBro) {
    // TODO
}

// this node and its left brother redistribute
// the left has more entries
void InnerNode::redistributeLeft(const int& index, InnerNode* const& leftBro, InnerNode* const& parent) {
    // TODO
}

// this node and its right brother redistribute
// the right has more entries
void InnerNode::redistributeRight(const int& index, InnerNode* const& rightBro, InnerNode* const& parent) {
    // TODO
}

// merge all entries to its left bro, delete this node after merging.
void InnerNode::mergeLeft(InnerNode* const& leftBro, const Key& k) {
    // TODO
}

// merge all entries to its right bro, delete this node after merging.
void InnerNode::mergeRight(InnerNode* const& rightBro, const Key& k) {
    // TODO
}

// remove a children from the current node, used by remove func
void InnerNode::removeChild(const int& keyIdx, const int& childIdx) {
    // TODO
}

// update the target entry, return true if the update succeed.
bool InnerNode::update(const Key& k, const Value& v) {
    // TODO
    return false;
}

// find the target value with the search key, return MAX_VALUE if it fails.
Value InnerNode::find(const Key& k) {
    // TODO
    return MAX_VALUE;
}

// get the children node of this InnerNode
Node* InnerNode::getChild(const int& idx) {
    // TODO
    return NULL;
}

// get the key of this InnerNode
Key InnerNode::getKey(const int& idx) {
    if (idx < this->nKeys) {
        return this->keys[idx];
    } else {
        return MAX_KEY;
    }
}

// print the InnerNode
void InnerNode::printNode() {
    cout << "||#|";
    for (int i = 0; i < this->nKeys; i++) {
        cout << " " << this->keys[i] << " |#|";
    }
    cout << "|" << "    ";
}

// print the LeafNode
void LeafNode::printNode() {
    cout << "||";
    for (int i = 0; i < 2 * this->degree; i++) {
        if (this->getBit(i)) {
            cout << " " << this->kv[i].k << " : " << this->kv[i].v << " |";
        }
    }
    cout << "|" << " ====>> ";
}

// new a empty leaf and set the valuable of the LeafNode
LeafNode::LeafNode(FPTree* t) {
    // TODO
}

// reload the leaf with the specific Persistent Pointer
// need to call the PAllocator
LeafNode::LeafNode(PPointer p, FPTree* t) {
    // TODO
}

LeafNode::~LeafNode() {
    // TODO
}

// insert an entry into the leaf, need to split it if it is full
KeyNode* LeafNode::insert(const Key& k, const Value& v) {
    KeyNode* newChild = NULL;
    if (this->n==this->degree*2)//判断叶子结点是否是满的
    {
        newchild=this->split();
    }
    this->insertNonFull(k,v);
    return newChild;
}

// insert into the leaf node that is assumed not full
void LeafNode::insertNonFull(const Key& k, const Value& v) {
    int i;
    for ( i = 0; i < this->degree*2; i++)
    {
        if(!this->getBit(i)) break; //getBit=0
    }
    this->bitmap[i/8] |=(1 << i % 8);
    this->kv[i].k=k;
    this->kv[i].v=v;
    n++;
}

// split the leaf node
KeyNode* LeafNode::split() {
    KeyNode* newChild = new KeyNode();
    // TODO
    return newChild;
}

// use to find a mediant key and delete entries less then middle
// called by the split func to generate new leaf-node
// qsort first then find
Key LeafNode::findSplitKey() {
    Key midKey = 0;
    // TODO
    return midKey;
}

// get the targte bit in bitmap
// TIPS: bit operation

int LeafNode::getBit(const int& idx) {
    if (idx > this->degree*2) {return -1;}
    else
    {
        return (bitmap[idx / 8] >> idx % 8 ) & 1;//get the value of the bit at idx
    }


}

Key LeafNode::getKey(const int& idx) {
    return this->kv[idx].k;
}

Value LeafNode::getValue(const int& idx) {
    return this->kv[idx].v;
}

PPointer LeafNode::getPPointer() {
    return this->pPointer;
}

// remove an entry from the leaf
// if it has no entry after removement return TRUE to indicate outer func to delete this leaf.
// need to call PAllocator to set this leaf free and reuse it
bool LeafNode::remove(const Key& k, const int& index, InnerNode* const& parent, bool &ifDelete) {
    bool ifRemove = false;
    // TODO
    return ifRemove;
}

// update the target entry
// return TRUE if the update succeed
bool LeafNode::update(const Key& k, const Value& v) {
    bool ifUpdate = false;
    // TODO
    return ifUpdate;
}

// if the entry can not be found, return the max Value
Value LeafNode::find(const Key& k) {
    // TODO
    return MAX_VALUE;
}

// find the first empty slot
int LeafNode::findFirstZero() {
    // TODO
    return -1;
}

// persist the entire leaf
// use PMDK
void LeafNode::persist() {
    // TODO
}

// call by the ~FPTree(), delete the whole tree
void FPTree::recursiveDelete(Node* n) {
    if (n->isLeaf) {
        delete n;
    } else {
        for (int i = 0; i < ((InnerNode*)n)->nChild; i++) {
            recursiveDelete(((InnerNode*)n)->childrens[i]);
        }
        delete n;
    }
}

FPTree::FPTree(uint64_t t_degree) {
    FPTree* temp = this;
    this->root = new InnerNode(t_degree, temp, true);
    this->degree = t_degree;
    bulkLoading();
}

FPTree::~FPTree() {
    recursiveDelete(this->root);
}

// get the root node of the tree
InnerNode* FPTree::getRoot() {
    return this->root;
}

// change the root of the tree
void FPTree::changeRoot(InnerNode* newRoot) {
    this->root = newRoot;
}

void FPTree::insert(Key k, Value v) {
    if (root != NULL) {
        root->insert(k, v);
    }
}

bool FPTree::remove(Key k) {
    if (root != NULL) {
        bool ifDelete = false;
        InnerNode* temp = NULL;
        return root->remove(k, -1, temp, ifDelete);
    }
    return false;
}

bool FPTree::update(Key k, Value v) {
    if (root != NULL) {
        return root->update(k, v);
    }
    return false;
}

Value FPTree::find(Key k) {
    if (root != NULL) {
        return root->find(k);
    }
}

// call the InnerNode and LeafNode print func to print the whole tree
// TIPS: use Queue
void FPTree::printTree() {
    // TODO
}

// bulkLoading the leaf files and reload the tree
// need to traverse leaves chain
// if no tree is reloaded, return FALSE
// need to call the PALlocator
bool FPTree::bulkLoading() {
    // TODO
    PPointer PP = PAllocator::getAllocator()->getStartPointer();
    if (PP.fileId == 0){
        this->root = new InnerNode(this->degree,this,true);
        return false;
    }
    LeafNode * lnode = new LeafNode(PP,this);
    queue<Node *> nodes;
    size_t len_now = 0, len_new = 0;
    for (;lnode != NULL;lnode = lnode->next){
        nodes.push(lnode);
        ++len_now;
    }
    while(nodes.size() > 1 || nodes.front()->ifLeaf()){
        InnerNode* Innode = new InnerNode(degree,this);
        size_t  s   = len_now < 2 * degree + 1 ? len_now : degree;
        for (size_t i = 0; i < s; ++i){
            Node* node = nodes.front();
            Key  minKey = MAX_KEY;
            if(node->ifLeaf())
                for (size_t j = 0; j < node->bitmapSize; ++j)
                    if(node->getBit(j)){
                        minKey = minKey > node->getKey(j) ? node->getKey(j):minKey;
                    }
            else{
                minKey = keys[0];
            }
            Innode->insertLeaf(KeyNode{minKey, node});
            nodes.pop();
            --len_now;
        }
        nodes.push(Innode);
        ++len_new;
        if(len_now == 0){
            len_now = len_new;
            len_new = 0;
        }
    }
    this->root->isRoot = true;
    this->root = (InnerNode *)nodes.front();
    return true;
}
