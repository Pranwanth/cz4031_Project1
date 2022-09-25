#include "BPTree.h"
using namespace LmaoDB;

Record rec[8];
const int NUM_ENTRY = 6;
int main() {
    rec[1].key = 1, rec[1].val = "first entry";
    rec[2].key = 2, rec[2].val = "second entry";
    rec[3].key = 3, rec[3].val = "third entry";
    rec[4].key = 4, rec[4].val = "fourth entry";
    rec[5].key = 5, rec[5].val = "fifth entry";
    rec[6].key = 6, rec[6].val = "sixth entry";

    cout << "Part I: testing insertion ------------------------------ " << endl;
    shared_ptr<Node<int>> root(new LeafNode<int>);
    for (int i = 1; i <= NUM_ENTRY; ++i) {
        auto newRoot = root->insert(rec[i].key, &rec[i], root);
        root = newRoot; // can safely discard old (?)
        cout << "After i = " << i << ": \n";
        root->display();
    }

    cout << "Part II: testing single query -------------------------- " << endl;
    cout << "Please input number of queries: " << endl;
    int T; cin >> T;
    for (int i = 1; i <= T; ++i) {
        cout << "Please input query: " << endl;
        int key; cin >> key;
        cout << "Querying key = " << key << ": " << root->query(key)->val << endl;
    }

    cout << "Part III: testing range query -------------------------- " << endl;
    cout << "Please input number of queries: " << endl;
    cin >> T;
    for (int i = 1; i <= T; ++i) {
        cout << "Please input query: " << endl;
        int l, r; cin >> l >> r;
        for (auto e: root->rangeQuery(l, r)) cout << e->val << "; ";
        cout << endl;
    }

    // root = root->remove(rec[1].key, root);
    root->display();
}