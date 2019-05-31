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
    int i = findIndex(k);
    for(int j = this->nKeys; j > i; -- j) {
        this->keys[j] = this->keys[j-1];
        this->childrens[j] = this->childrens[j-1];
    }
    this->keys[i] = k;
    this->childrens[i+1] = node;
    this->nKeys ++;
    this->nChild ++;
}

// insert func
// return value is not NULL if split, returning the new child and a key to insert
KeyNode* InnerNode::insert(const Key& k, const Value& v) {
        KeyNode* newChild = NULL;

    // 1.insertion to the first leaf(only one leaf)
    if (this->isRoot && this->nKeys == 0 && this->nChild == 0) {
        // TODO
        KeyNode* newch = new KeyNode[1];
        newch->node = new LeafNode(tree);
        newch->node->insert(k,v);
        newch->key = k;
        // this->keys[0] = k;
        this->childrens[0] = newch->node;

        // this->nKeys = 1;
        this->nChild ++;
        return NULL;
    }
    int find = findIndex(k);
    newChild = childrens[find]->insert(k,v);

    if(newChild != NULL) {
        insertNonFull(newChild->key, newChild->node);

        if(this->nKeys > degree * 2) {
            newChild = this->split();
            if (this->isRoot) {
                this->isRoot = false;
                InnerNode * newRoot = new InnerNode(this->degree, tree, true);
                newRoot->nKeys = 1;
                newRoot->nChild = 2;
                newRoot->keys[0] = newChild->key;
                newRoot->childrens[0] = this;
                newRoot->childrens[1] = newChild->node;
                this->tree->changeRoot(newRoot);
                return NULL;
            }
        }
        else
            return NULL;
    }
    else
        return NULL;
    return newChild;

}

// ensure that the leaves inserted are ordered
// used by the bulkLoading func
// inserted data: | minKey of leaf | LeafNode* |
KeyNode* InnerNode::insertLeaf(const KeyNode& leaf) {
    KeyNode* newChild = NULL;
    // first and second leaf insertion into the tree
    if (this->isRoot && this->nKeys == 0 && this->nChild == 0) {
        // TODO
        
        childrens[0] = leaf.node;
        keys[0] = leaf.key;
        ++ nChild;
        return newChild;
    }

    // next level is not leaf, just insertLeaf
    // TODO
    int index = findIndex(leaf.key);

    if (this->childrens[0]->ifLeaf()) {
        this->insertNonFull(leaf.key, leaf.node);
        if (this->nKeys == 2 * degree + 1) {
            KeyNode * NewInnerNode = this->split();
            if(this->isRoot) {
                InnerNode *NewRoot = new InnerNode(degree, tree, true);
                this->isRoot = false;
                NewRoot->childrens[0] = this;
                ++ (NewRoot->nChild);
                NewRoot->insertNonFull(NewInnerNode->key, NewInnerNode->node);
                //NewRoot->nKeys = 1;
                tree->changeRoot(NewRoot);
                return NULL;
            }
            else return NewInnerNode;
        }
        else return NULL;
    }


    else{
        newChild = ((InnerNode*) childrens[index])->insertLeaf(leaf);
        if(newChild == NULL) return newChild;
        else {
            insertNonFull(newChild->key, newChild->node);
            if (this->nKeys == 2 * degree + 1) {
                /*KeyNode * */
                KeyNode * NewInnerNode = this->split();
                if(this->isRoot) {
                    InnerNode *NewRoot = new InnerNode(degree, tree, true);
                    this->isRoot = false;
                    NewRoot->childrens[0] = this;
                    ++ (NewRoot->nChild);
                    NewRoot->insertNonFull(NewInnerNode->key, NewInnerNode->node);
                    //NewRoot->nKeys = 1;
                    tree->changeRoot(NewRoot);
                    return NULL;
                }
                else return NewInnerNode;
            }
            else return NULL;
        }
    }

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
    if(this->isRoot && this->nKeys == 0) {
        ifRemove = ((LeafNode *)(this->childrens[0]))->remove(k, index, parent, ifDelete);
        if(ifRemove && ifDelete) {  //remove success , decrease 
            this->nChild --;
            ifDelete = false;
        }
        return ifRemove;
    }   
    // // recursive remove
    // // TODO
    int idx = this->findIndex(k);
    ifRemove = this->childrens[idx]->remove(k,idx,this,ifDelete);
    if(ifDelete) {
        this->removeChild(idx, idx+1);
        //not a root
        if(!this->isRoot && this->nKeys < this->degree) {
            //find it's left and right brp
            InnerNode * leftBro, * rightBro;
            this->getBrother(index, parent, leftBro, rightBro);

            if(leftBro != NULL && leftBro->nKeys > this->degree) {
                //have a left brother
                this->redistributeLeft(index, leftBro, parent);
            }
            else if (rightBro != NULL && rightBro->nKeys > this->degree) {
                this->redistributeRight(index, rightBro, parent);
            }
            else if(leftBro != NULL) {
                if(parent->isRoot && parent->nKeys == 1) {
                    this->mergeParentLeft(parent, leftBro);
                }
                else {
                    this->mergeLeft(leftBro, parent->keys[index-1]);
                }
            }
            else if(rightBro != NULL) {
                    if(parent->isRoot && parent->nKeys == 1) {
                        this->mergeParentRight(parent, rightBro);
                    }
                    else {
                     this->mergeRight(rightBro, parent->keys[index-1]);
                    }
            }
        }
    }
    ifDelete = false;
    return ifDelete;
}

// If the leftBro and rightBro exist, the rightBro is prior to be used
void InnerNode::getBrother(const int& index, InnerNode* const& parent, InnerNode* &leftBro, InnerNode* &rightBro) {
    // TODO
}

// merge this node, its parent and left brother(parent is root)
void InnerNode::mergeParentLeft(InnerNode* const& parent, InnerNode* const& leftBro) {
    // TODO
    parent->isRoot = false;
    leftBro->isRoot = true;
    leftBro->tree->changeRoot(leftBro);
    leftBro->keys[leftBro->nKeys] = parent->keys[0];
    int nchild = leftBro->nChild;
    int nkey = leftBro->nKeys;
    int i = 0;
    //put this's childrens into leftBro's
    for(; i < parent->nChild; ++ i) {
        leftBro->childrens[nchild + i] = parent->childrens[i];
        if(i != 0) leftBro->keys[i + nkey] = parent->keys[i-1];
    }

    //uodate leftBro's nKeys and nChile
    leftBro->nKeys += parent->nKeys+1;
    leftBro->nChild += parent->nChild;

    //reset parent's nkeys and nChild
    parent->nChild  = 0;
    parent->nKeys = 0;
}

// merge this node, its parent and right brother(parent is root)
void InnerNode::mergeParentRight(InnerNode* const& parent, InnerNode* const& rightBro) {
    // TODO
    parent->isRoot = false;
    rightBro->isRoot = true;
    rightBro->tree->changeRoot(rightBro);
    rightBro->keys[rightBro->nKeys] = parent->keys[0];
    int nchild = rightBro->nChild;
    int nkey = rightBro->nKeys;
    int i = 0;
    //put this's childrens into rightBro's
    for(; i < parent->nChild; ++ i) {
        rightBro->childrens[nchild + i] = parent->childrens[i];
        if(i != 0) rightBro->keys[i + nkey] = parent->keys[i-1];
    }

    //uodate rightBro's nKeys and nChile
    rightBro->nKeys += parent->nKeys+1;
    rightBro->nChild += parent->nChild;

    //reset parent's nkeys and nChild
    parent->nChild  = 0;
    parent->nKeys = 0;
}

// this node and its left brother redistribute
// the left has more entries
void InnerNode::redistributeLeft(const int& index, InnerNode* const& leftBro, InnerNode* const& parent) {
    // TODO
    int i = 0;
    for(i = parent->nChild; i > 0; -- i) {
        parent->childrens[i] = parent->childrens[i-1];
        if(i < parent->nChild){
            parent->keys[i] = parent->keys[i-1];
        }
    }

    parent->childrens[0] = leftBro->childrens[leftBro->nChild-1];
    parent->keys[0] = parent->keys[index-1];
    parent->keys[index-1] = leftBro->keys[leftBro->nKeys -1];
    
    parent->nChild++;
    parent->nKeys ++;
    leftBro->nChild --;
    leftBro->nKeys --;
}

// this node and its right brother redistribute
// the right has more entries
void InnerNode::redistributeRight(const int& index, InnerNode* const& rightBro, InnerNode* const& parent) {

    // TODO
    Key key = rightBro->keys[0];
    this->keys[nKeys++] = parent->keys[index];
    parent->keys[index] = key;
    this->childrens[nChild++] = rightBro->childrens[0];
    rightBro->nChild--;
    rightBro->nKeys--;
    int i = 0;
    for(; i < rightBro->nChild; ++i) {
        if (i < rightBro->nChild - 1){
            rightBro->keys[i] = rightBro->keys[i + 1];
        }
        rightBro->childrens[i] = rightBro->childrens[i + 1];
    }
}

// merge all entries to its left bro, delete this node after merging.
void InnerNode::mergeLeft(InnerNode* const& leftBro, const Key& k) {
    // TODO

    int i = 0;
    int nchild = leftBro->nChild;
    int nkey = leftBro->nKeys;
    for(; i < this->nChild; ++ i) {
        leftBro->childrens[nchild + i] = this->childrens[i];
        if(i == 0) leftBro->keys[i+nkey] = k;
        else leftBro->keys[i+nkey] = this->keys[i-1];
    }

    //update
    leftBro->nChild += this->nChild;
    leftBro->nKeys += this->nKeys + 1;
    // delete this;
    this->nChild = 0;
    this->nKeys = 0;
}

// merge all entries to its right bro, delete this node after merging.
void InnerNode::mergeRight(InnerNode* const& rightBro, const Key& k) {
    // TODO

    int i = 0;
    int nchild = rightBro->nChild;
    int nkey = rightBro->nKeys;
    for(; i < this->nChild; ++ i) {
        rightBro->childrens[nchild + i] = this->childrens[i];
        if(i != nchild-1) rightBro->keys[i] = this->keys[i];
        else rightBro->keys[i] = k;
    }

    for (i = nchild + this->nChild - 1; i >= this->nChild; --i) {
        rightBro->childrens[i] = rightBro->childrens[i - this->nChild];
        if (i != nchild+ this->nChild - 1)
            rightBro->keys[i] = rightBro->keys[i - this->nChild];
    }

    //update
    rightBro->nChild += this->nChild;
    rightBro->nKeys += this->nKeys + 1;
    // delete this;
    this->nChild = 0;
    this->nKeys = 0;
}

// remove a children from the current node, used by remove func
void InnerNode::removeChild(const int& keyIdx, const int& childIdx) {
    // TODO
}

// update the target entry, return true if the update succeed.
bool InnerNode::update(const Key& k, const Value& v) {
    // TODO
    int find = findIndex(k);
    return this->childrens[find]->update(k,v);
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
    // TODO
        if (this->n >= this->degree*2 - 1)
    {
        insertNonFull(k, v);
        newChild=split();
        this->next = (LeafNode *)newChild->node;
        ((LeafNode* )(newChild->node))-> prev =this;
    }
    else 
        insertNonFull(k,v);
    return newChild;
}

// insert into the leaf node that is assumed not full
void LeafNode::insertNonFull(const Key& k, const Value& v) {
    // TODO
    int i = findFirstZero(); 
    this->fingerprints[i] = keyHash(k);
    this->bitmap[i / 8] |= 1 << (i % 8);
    this->kv[i].k = k;
    this->kv[i].v = v;
    n++;
    persist();
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
    // TODO
    return 0;
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
    Byte hash = keyHash(k);
    int i = 0;
    for(; i < this->degree * 2; ++ i) {
        if(getBit(i) && (this->kv[i].k == k) && (this->fingerprints[i] == hash)) {
            ifRemove = true;
            this->fingerprints[i] = 0;
            bitmap[i/8] &= ~(1 << (i % 8));
            this->n --;
            if(this->n == 0) {
                ifDelete = true;
                LeafNode * pre, * nex;
                pre = this->prev;
                nex = this->next;
                if(pre != NULL) pre->next = nex;
                if(nex != NULL) nex->prev = prev;
                PAllocator* pa = PAllocator::getAllocator();
                pa->freeLeaf(this->pPointer);
            }
            this->persist();
        }
    }
    return ifRemove;
}

// update the target entry
// return TRUE if the update succeed
bool LeafNode::update(const Key& k, const Value& v) {
    bool ifUpdate = false;
    // TODO
    for(int i = 0; i < 2 * degree; i ++) {
        if(getBit(i) == 1 && fingerprints[i] == keyHash(k) && getKey(i) == k) {
            kv[i].v = v;
            ifUpdate = true;
            persist();
            break;
        }
    }
    
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
    return false;
}
