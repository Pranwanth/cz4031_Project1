#include <assert.h>
namespace LmaoDB {

    template<typename T, typename R>
    LeafNode<T, R>::LeafNode(RegularNode<T, R>* const father_) {
        father = father_;
    }

    template<typename T, typename R>
    R* LeafNode<T, R>::query(const T &key) {
        auto ans = lower_bound(keys.begin(), keys.end(), key);
        if (*ans != key)
            return nullptr;
        else
            return ptr[ans - keys.begin()];
    }

    template<typename T, typename R>
    vector<R *> LeafNode<T, R>::rangeQuery(const T &l, const T &r) {
        vector<R *> ret;
        rangeQuery(ret, l, r);
        return ret;
    }

    template<typename T, typename R>
    void LeafNode<T, R>::rangeQuery(vector<R *> &ret, const T &l, const T &r) {
        auto ans = lower_bound(keys.begin(), keys.end(), l);
        if (ans == keys.end()) {
            return;
        }
        while (ans != keys.end() && *ans <= r) {
            auto pos = ans - keys.begin();
            ret.emplace_back(ptr[pos]);
            ans++;
        }
        if (ans == keys.end()) {
            finalPtr->rangeQuery(ret, l, r);
        }
    }

    template<typename T, typename R>
    shared_ptr<Node<T, R>> LeafNode<T, R>::insert(const T &key, R *const record, const shared_ptr<Node<T, R>>& oldRoot) {
        auto pos = lower_bound(keys.begin(), keys.end(), key) - keys.begin();
        keys.insert(keys.begin() + pos, key);
        ptr.insert(ptr.begin() + pos, record);
        return balance(oldRoot);
    }

    template<typename T, typename R>
    shared_ptr<Node<T, R>> LeafNode<T, R>::balance(const shared_ptr<Node<T, R>>& oldRoot) {
        if (keys.size() <= N) return oldRoot;
        assert(keys.size() == ptr.size() && keys.size() == N + 1);
        cout << "Balance() triggered: keys.size() = " << keys.size() << endl;

        // left has floor((N + 1) / 2) keys, right has rest
        auto newNode = new LeafNode(father);
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
    shared_ptr<Node<T, R>> LeafNode<T, R>::remove(const T &key, const shared_ptr<Node<T, R>> root) {
        if (query(key) == nullptr) {
            cout << "Could not find key" << endl;
            return root;
        }
        auto pos = lower_bound(keys.begin(), keys.end(), key) - keys.begin();
        int left = (N + 1) / 2;

        // Erase the data
        keys.erase(keys.begin() + pos);
        ptr.erase(ptr.begin() + pos);

        // Check whether it is the root node, if so return
        if (father == nullptr) {
            return root;
        }

        // If there are enough minimum nodes to still exist, leave it as it is
        if (keys.size() > left && ptr.size() > left) {
            return root;
        }

        // Borrow keys/ptrs
        if (this->finalPtr != nullptr) {
            // Has enough keys / ptrs to give
            if (this->finalPtr->keys.size() - 1 > left) {
//                this->insert(this->finalPtr->keys[0], this->finalPtr->ptr[0]); // TODO fix
//                this->finalPtr->remove(this->finalPtr->keys[0], root);

                return root;
            }
        }

        // Go to parent to find sibilings to take from
        auto done = father->helpSibling(key, root);
        if (done) {
            return root;
        }

        vector<T> curKeys;
        vector<R *> records;
        for (int i = 0; i < this->keys.size(); i++) {
            curKeys.push_back(this->keys[i]);
            records.push_back(this->ptr[i]);
        }

        return father->mergeNodes(curKeys, records, root);
    }

    template<typename T, typename R>
    shared_ptr<Node<T, R>> LeafNode<T, R>::mergeNodes(vector<T> keys, vector<R *> ptrs, const shared_ptr<Node<T, R>> root) {
        for (int i = 0; i < this->keys.size(); i++) {
            keys.push_back(this->keys[i]);
            ptrs.push_back(this->ptr[i]);
        }

        return nullptr;
    }

    template<typename T, typename R>
    vector<T> LeafNode<T, R>::getKeys() {
        return this->keys;
    }

    template<typename T, typename R>
    vector<R *> LeafNode<T, R>::getPtrs() {
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
}