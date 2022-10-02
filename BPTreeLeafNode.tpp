#include <cassert>
namespace LmaoDB {
    template<typename T, typename R>
    LeafNode<T, R>::LeafNode(RegularNode<T, R>* const father_) {
        father = father_;
    }

    template<typename T, typename R>
    vector<R*> LeafNode<T, R>::query(const T &key) {
        cout << "Query: Accessing node with Key = [";
        for (int i = 0; i <= min(4, (int)keys.size() - 1); ++i) cout << keys[i] << " ";
        cout << "...]" << endl;

        auto ans = lower_bound(keys.begin(), keys.end(), key);
        vector<R*> result;
        if (*ans == key) for (auto e: ptr[ans - keys.begin()]) result.emplace_back(e);
        return result;
    }

    template<typename T, typename R>
    vector<R *> LeafNode<T, R>::rangeQuery(const T &l, const T &r) {
        vector<R *> ret;
        rangeQuery(ret, l, r);
        return ret;
    }

    template<typename T, typename R>
    void LeafNode<T, R>::rangeQuery(vector<R *> &ret, const T &l, const T &r) {
        cout << "Query: Accessing node with Key = [";
        for (int i = 0; i <= min(4, (int)keys.size() - 1); ++i) cout << keys[i] << " ";
        cout << "...]" << endl;
        
        auto ans = lower_bound(keys.begin(), keys.end(), l);
        if (ans != keys.end()) {
            while (ans != keys.end() && *ans <= r) {
                auto pos = ans - keys.begin();
                for (auto e: ptr[pos]) ret.template emplace_back(e);
                ans++;
            }
            if (ans == keys.end() && finalPtr != nullptr) finalPtr->rangeQuery(ret, l, r);
        }
    }

    template<typename T, typename R>
    shared_ptr<Node<T, R>> LeafNode<T, R>::insert(const T &key, R *const record, const shared_ptr<Node<T, R>>& oldRoot) {
        auto pos = lower_bound(keys.begin(), keys.end(), key);
        if (pos != keys.end() && *pos == key) {
            bool hasDuplicate = false;
            // for (auto e: ptr[pos - keys.begin()]) if (e == record) hasDuplicate = true; // TODO optimize time?
            if (!hasDuplicate) ptr[pos - keys.begin()].emplace_back(record);
            return oldRoot;
        } else {
            list<R*> currRecordSet; currRecordSet.emplace_back(record);
            ptr.insert((pos - keys.begin()) + ptr.begin(), currRecordSet);
            keys.insert(pos, key); // only now I insert key, so that above line work
            return balance(oldRoot);
        }
    }

    template<typename T, typename R>
    shared_ptr<Node<T, R>> LeafNode<T, R>::balance(const shared_ptr<Node<T, R>>& oldRoot) {
        if (keys.size() <= N) return oldRoot;
        assert(keys.size() == ptr.size() && keys.size() == N + 1);
        // cout << "Balance() triggered: keys.size() = " << keys.size() << endl;

        // left has floor((N + 1) / 2) keys, right has rest

        auto newNode = new LeafNode(father);
        newNode->finalPtr = this->finalPtr;
        this->finalPtr = newNode;
        int left = (N + 1) / 2;

        // split nodes
        for (int i = left; i <= N; ++i) {
            newNode->keys.emplace_back(std::move(keys[i]));
            newNode->ptr.emplace_back(ptr[i]);
        }
        while (keys.size() > left && ptr.size() > left) {
            keys.pop_back();
            ptr.pop_back();
        }
        // add into father
        if (father == nullptr) { // we are current root, we need new root.
            assert(oldRoot.get() == this);
            father = new RegularNode<T, R>(nullptr);
            newNode->father = father;

            auto newPtr(oldRoot); // copy of shared ptr to oneself
            father->ptr.emplace_back(newPtr); // lifecycle of currNode is OK even if
            father->insertSubNode(newNode, newNode->keys[0]); // lifecycle of newNode is OK
            return shared_ptr<Node<T, R>>(father); // claim. lifecycle does not get interrupt
        } else { // tree structure stays same
            father->insertSubNode(newNode, newNode->keys[0]); // lifecycle of newNode is OK
            return father->balance(oldRoot);
        }
    }

    template<typename T, typename R>
    shared_ptr<Node<T, R>> LeafNode<T, R>::remove(const T &key, const shared_ptr<Node<T, R>>& root) {
//        if (query(key).size() == 0) {
//            cout << "Could not find key" << endl;
//            exit(EXIT_FAILURE);
//        }
//
//        auto pos = lower_bound(keys.begin(), keys.end(), key) - keys.begin();
//        int left = (N + 1) / 2;
//
//        // Erase the data pointer heads
//        keys.erase(keys.begin() + pos);
//        ptr.erase(ptr.begin() + pos);
//
//        // Check for root node (tree size = 1), return if so
//        if (father == nullptr) {
//            return root;
//        }
//
//        // If there are enough minimum nodes to still exist, leave it as it is
//        if (keys.size() >= left && ptr.size() >= left) {
//            return root;
//        }
//        // Borrow keys/ptrs
//        if (this->finalPtr != nullptr) {
//            // Has enough keys / ptrs to give
//            if (this->finalPtr->getKeys().size() - 1 > left) {
//                for (auto e: this->finalPtr->getPtrs().front()) this->insert(this->finalPtr->getKeys().front(), e, root);
//                this->finalPtr->remove(this->finalPtr->getKeys().front(), root);
//                return root;
//            }
//        }
//
//        // Go to parent to find sibilings to take from
//        auto done = father->helpSibling(key, root);
//        if (done) {
//            return root;
//        }
//
//        vector<T> curKeys;
//        vector<list<R*>> records;
//        for (int i = 0; i < this->keys.size(); i++) {
//            curKeys.push_back(this->keys[i]);
//            records.push_back(this->ptr[i]);
//        }
//
//        return father->mergeNodes(curKeys, records, root);
    }

    template<typename T, typename R>
    shared_ptr<Node<T, R>> LeafNode<T, R>::mergeNodes(vector<T> keys, vector<list<R *>> ptrs, const shared_ptr<Node<T, R>>& root) {
//        for (int i = 0; i < this->keys.size(); i++) {
//            keys.push_back(this->keys[i]);
//            ptrs.push_back(this->ptr[i]);
//        }
//        return nullptr;
    }

    template<typename T, typename R>
    vector<T> LeafNode<T, R>::getKeys() {
        return this->keys;
    }

    template<typename T, typename R>
    vector<list<R*>> LeafNode<T, R>::getPtrs() {
        return this->ptr;
    }

    template<typename T, typename R>
    void LeafNode<T, R>::display() {
        cout << "Leaf Node : [ ";
        for (int i = 0; i < this->keys.size(); i++) {
            if (i + 1 == this->keys.size()) {
                cout << this->keys.at(i) << " ";
            } else {
                cout << this->keys.at(i) << ", ";
            }
        }
        cout << "]" << endl;
    }

    template<typename T, typename R>
    int LeafNode<T, R>::treeSize() {
        return 1;
    }

    template<typename T, typename R>
    int LeafNode<T, R>::numNodes() {
        return 1;
    }
    
    template<typename T, typename R>
    int LeafNode<T, R>::getN() {
        return N;
    }

    template<typename T, typename R>
    void LeafNode<T, R>::displayFirstChild(bool caller) {
        for (int i = 0; i < keys.size(); i++) {
                cout << keys[i] << " ";
        }
    }
}