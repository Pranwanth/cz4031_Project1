#include "BPTree.h"
using namespace LmaoDB;

Record rec[3];
const int NUM_ENTRY = 2;
int main() {
    rec[1].key = 1, rec[1].val = "first entry";
    rec[2].key = 2, rec[2].val = "second entry";

    Node<int>* tmpPtr = new LeafNode<int>; // okay to pass upcasted to shared_ptr constructor
    shared_ptr<Node<int>> root(tmpPtr);
    for (int i = 1; i <= NUM_ENTRY; ++i) {
        auto newRoot = root->insert(rec[i].key, &rec[i]);
        if (newRoot.get() != nullptr) root = newRoot; // can safely discard old
    }

    for (int i = 1; i <= NUM_ENTRY; ++i) {
        cout << "The value of " << i << "th entry is " << root->query(rec[i].key)->val << endl;
    }

    root = root->remove(rec[1].key, root);

    cout << "The value of 1th entry is " << root->query(rec[2].key)->val << endl;
}