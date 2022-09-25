#include "BPTree.h"
#include "storage.h"

using namespace LmaoDB;

const int NUM_ENTRY = 6;

int main() {
    FILE *fp;
    fp = fopen(PATH, "r");
    if (fp == nullptr) {
        perror("Failed: ");
        return 1;
    }
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    Record *record;
    Block *firstBlock;
    Block *tempBlock = NULL;    // pointer to the current block (last block)

    shared_ptr<Node<int>> root(new LeafNode<int>);

    read = getline(&line, &len, fp); //get the first line
    int flag = 0;   // initial flag
    while ((read = getline(&line, &len, fp)) != -1) {
        record = (Record *) malloc(sizeof(Record));
        createRecord(line, record);
        root = root->insert(record->numVotes, record, root);
        if (flag == 0) { // initial stage
            firstBlock = createBlock(NULL);
            tempBlock = firstBlock;
            flag = 1;
        } else if (isFull(tempBlock, record)) {// current block is full
            tempBlock = createBlock(tempBlock);
        }

        insertRecord(record, tempBlock);

    }
    free(line);
    printf("total number of blocks: %d\n", getBlockNum(startBlock));
    fclose(fp);
}

void handTest() {
    cout << "Part I: testing insertion ------------------------------ " << endl;
    shared_ptr<Node<int>> root(new LeafNode<int>);

    cout << "Part II: testing single query -------------------------- " << endl;
    cout << "Please input number of queries: " << endl;
    int T;
    cin >> T;
    for (int i = 1; i <= T; ++i) {
        cout << "Please input query: " << endl;
        int key;
        cin >> key;
        cout << "Querying key = " << key << ": " << root->query(key)->val << endl;
    }

    cout << "Part III: testing range query -------------------------- " << endl;
    cout << "Please input number of queries: " << endl;
    cin >> T;
    for (int i = 1; i <= T; ++i) {
        cout << "Please input query: " << endl;
        int l, r;
        cin >> l >> r;
        for (auto e: root->rangeQuery(l, r)) cout << e->val << "; ";
        cout << endl;
    }

    // root = root->remove(rec[1].key, root);
    root->display();
}