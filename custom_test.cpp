//
// Created by xade on 10/1/22.
//

#include"BPTree.h"
#include"storage.h"
using namespace LmaoDB;
int main() {
    shared_ptr<Node<int, int>> root(new LeafNode<int, int>);
    for (int i = 10; i >= 1; --i) {
        cout << "Test for insertion of i = " << i << endl;
        root = root->insert(i, (int*)i, root);
        root->display();
    }
    auto ret = root->rangeQuery(5, 7);
}