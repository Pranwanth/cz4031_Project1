#include <assert.h>
namespace LmaoDB {

    template<typename T>
    LeafNode<T>::LeafNode(LeafNode<T>* LeftPtr) {
        if (LeftPtr != nullptr) {
            auto RightPtr = LeftPtr->finalPtr;
            this->finalPtr = RightPtr;
            LeftPtr->finalPtr = this;
        } // else current node is root, no further action is needed
    }

    template<typename T>
    Record* const LeafNode<T>::query(const T &key) {
        auto ans = lower_bound(keys.begin(), keys.end(), key);
        if (*ans != key)
            return nullptr;
        else
            return ptr[ans - keys.begin()];
    }

    template<typename T>
    vector<Record *> LeafNode<T>::rangeQuery(const T &l, const T &r) {
        vector<Record *> ret;
        rangeQuery(ret, l, r);
        return ret;
    }

    template<typename T>
    void LeafNode<T>::rangeQuery(vector<Record *> &ret, const T &l, const T &r) {
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

    template<typename T>
    shared_ptr<Node<T>> LeafNode<T>::insert(const T &key, Record *const record) {
        auto pos = lower_bound(keys.begin(), keys.end(), key) - keys.begin();
        keys.insert(keys.begin() + pos, key);
        ptr.insert(ptr.begin() + pos, record);
        return balance();
    }

    template<typename T>
    shared_ptr<Node<T>> LeafNode<T>::balance() {
        if (keys.size() <= N) return (father == nullptr ? shared_ptr<Node<T>>(nullptr) : father->balance());
        assert(keys.size() == ptr.size() && keys.size() == N + 1);
        cout << "Balance() triggered: keys.size() = " << keys.size() << endl;
        // left has floor((N + 1) / 2) keys, right has rest
        LeafNode<T>* newNode = new LeafNode(this);
        int left = (N + 1) / 2;
//            newNode->keys.reserve(right);
        for (int i = left; i <= N; ++i) {
            newNode->keys.emplace_back(move(keys[i]));
            newNode->ptr.emplace_back(ptr[i]);
        }
        while (keys.size() > left && ptr.size() > left) {
            keys.pop_back();
            ptr.pop_back();
        }
        if (father == nullptr) {
            father = new RegularNode<T>(nullptr);
            father->ptr.emplace_back(SharedNodePtr<T>(this));
            father->insertSubNode(newNode, newNode->keys[0]);
        }
        assert(father != nullptr);
        return father->balance();
    }

    template<typename T>
    shared_ptr<Node<T>> LeafNode<T>::remove(const T &key, const shared_ptr<Node<T>> root) { 
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
                this->insert(this->finalPtr->keys[0], this->finalPtr->ptr[0]);
                this->finalPtr->remove(this->finalPtr->keys[0], root);

                return root;
            }
        }

        // Go to parent to find sibilings to take from
        auto done = father->helpSibiling(key, root);
        if (done) {
            return root;
        }

        vector<T> curKeys;
        vector<Record *> records;
        for (int i = 0; i < this->keys.size(); i++) {
            curKeys.push_back(this->keys[i]);
            records.push_back(this->ptr[i]);
        }

        return father->mergeNodes(curKeys, records, root);
    }

    template<typename T>
    shared_ptr<Node<T>> LeafNode<T>::mergeNodes(vector<T> keys, vector<Record *> ptrs, const shared_ptr<Node<T>> root) {
        for (int i = 0; i < this->keys.size(); i++) {
            keys.push_back(this->keys[i]);
            ptrs.push_back(this->ptr[i]);
        }

        return nullptr;
    }

    template<typename T>
    vector<T> LeafNode<T>::getKeys() {
        return this->keys;
    }

    template<typename T>
    vector<Record *> LeafNode<T>::getPtrs() {
        return this->ptr;
    }
}