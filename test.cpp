#include "BPTree.h"
#include "storage.h"
using namespace LmaoDB;

void treeInfo(shared_ptr<Node<int, Record>> root);

int main() {
    FILE *fp = fopen(DATA_PATH, "r");
    if (fp == nullptr) {
        perror("Failed: ");
        return 1;
    }
    char *line = nullptr;
    size_t len = 0;
    ssize_t read;

    Record *record;
    Block *firstBlock, *tempBlock = nullptr;    // pointer to the current block (last block)

    shared_ptr<Node<int, Record>> root(new LeafNode<int, Record>);

    read = getline(&line, &len, fp); //get the first line
    int flag = 0;   // initial flag
    int DEBUG_CNT = 0;
    while ((read = getline(&line, &len, fp)) != -1) {
        record = (Record *) malloc(sizeof(Record));
        createRecord(line, record);
        root = root->insert(record->numVotes, record, root);
        DEBUG_CNT++;
        if (DEBUG_CNT % 10000 == 0) cout << DEBUG_CNT << endl;
        if (flag == 0) { // initial stage
            firstBlock = createBlock(nullptr);
            tempBlock = firstBlock;
            flag = 1;
        } else if (isFull(tempBlock, record)) {// current block is full
            tempBlock = createBlock(tempBlock);
        }

        insertRecord(record, tempBlock);
    }
    free(line);
    printf("total number of blocks: %d\n", getBlockNum(firstBlock));
    fclose(fp);

    // Part 3
    cout << "Answering part (3)" << endl;
    treeInfo(root);

    // Part 4 / 5
    cout << endl;
    cout << "Answering part (4 & 5)" << endl;

    // query
    while (true) {
        int key; cin >> key;
        cout << "Result: " << endl;
        for (auto e: root->query(key)) cout << " " << e->id << ", " << e->averageRating << ", " << e->numVotes << endl;
    }


}

// Use for part 3 and part 6
void treeInfo(shared_ptr<Node<int, Record>> root) {
    cout << "Parameter N: " << root->getN() << endl;
    cout << "Number of nodes: " << root->numNodes() << endl;
    cout << "Current tree size: " << root->treeSize() << endl;
    cout << "Content of root node: [";
    for (int i = 0; i < root->getKeys().size(); i++) {
        cout << root->getKeys().at(i) << " ";
    }
    cout << "]" << endl;
    cout << "Content of 1st child node: [";
    root->displayFirstChild(true);
    cout << "]" << endl; 
    
    cout << endl;
}