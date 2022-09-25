#include "BPTree.h"

namespace LmaoDB {
    template<typename T>
    RegularNode<T>::RegularNode(RegularNode<T>* const father_) {
        father = father_;
    }

    template<typename T>
    Record * RegularNode<T>::query(const T &key) {
        return queryImmediateNext(key)->query(key);
    }

    template<typename T>
    vector<Record *> RegularNode<T>::rangeQuery(const T &l, const T &r) {
        vector<Record *> ret;
        rangeQuery(ret, l, r);
        return ret;
    }

    template<typename T>
    void RegularNode<T>::rangeQuery(vector<Record *> &ret, const T &l, const T &r) {
        queryImmediateNext(l)->rangeQuery(ret, l, r);
    }

    template<typename T>
    shared_ptr<Node<T>> RegularNode<T>::insert(const T &key, Record *const record, const shared_ptr<Node<T>>& oldRoot) {
        return queryImmediateNext(key)->insert(key, record, oldRoot); // balance is called at leaf node, and recurse back
    }

    template<typename T>
    Node<T>* RegularNode<T>::queryImmediateNext(const T &key) {
        auto loc = upper_bound(keys.begin(), keys.end(), key) - keys.begin();
        return ptr[loc].get();
    }

    template<typename T>
    void RegularNode<T>::insertSubNode(Node<T>* newPtr, const T &key) {
        assert(find(keys.begin(), keys.end(), key) == keys.end());
        uint32_t position = lower_bound(keys.begin(), keys.end(), key) - keys.begin();
        keys.insert(keys.begin() + position, key);
        ptr.insert(ptr.begin() + position + 1, shared_ptr<Node<T>>(
                newPtr)); // such location will always be valid; now father will manage the life of son.
    }

    template<typename T>
    shared_ptr<Node<T>> RegularNode<T>::balance(const shared_ptr<Node<T>>& oldRoot) {
        if (keys.size() <= N) return oldRoot;
        else {
            assert(keys.size() == N + 1);
            cout << "Balance() triggered: keys.size() = " << keys.size() << endl;
            auto newNode = new RegularNode(father);
            int left = (N + 1) / 2;
            // 1. Populate Right Node
            auto x = keys[0];
            for (int i = N / 2 + 1; i < keys.size(); ++i) newNode->keys.emplace_back(move(keys[i]));
            for (int i = N / 2 + 1; i < ptr.size(); ++i) ptr[i]->father = newNode, newNode->ptr.emplace_back(ptr[i]); // change father as well
            auto extraKey = keys[N / 2];
            assert(newNode->keys.size() >= N / 2);
            assert(newNode->ptr.size() >= N / 2 + 1);
            assert(newNode->keys.size() + 1 == newNode->ptr.size());

            // 2. Clean Left Node
            keys.resize(N / 2), ptr.resize(N / 2 + 1);

            // 3. register new "node"
            if (father == nullptr) { // if there is no father, we can trivially build father with 1 key & 2 ptr
                assert(oldRoot.get() == this);
                father = new RegularNode<T>(nullptr);
                newNode->father = father;

                auto cpyCurr(oldRoot);
                father->ptr.emplace_back(cpyCurr); // lifecycle of curr node
                father->ptr.emplace_back(SharedNodePtr<T>(newNode)); // lifecycle of new node
                father->keys.emplace_back(extraKey);
                return shared_ptr<Node<T>>(father);
            } else {
                father->insertSubNode(newNode, newNode->keys[0]); // lifecycle of new node
                return father->balance(oldRoot);
            }
        }
    }
    template<typename T>
    shared_ptr<Node<T>> RegularNode<T>::remove(const T &key, const shared_ptr<Node<T>> root) { 
        return queryImmediateNext(key)->remove(key, root);
    }
    
    template<typename T>
    bool RegularNode<T>::helpSibling(const T &key, const shared_ptr<Node<T>> root) {
        int leftKeys = (N - 1) / 2;
        uint32_t position = lower_bound(keys.begin(), keys.end(), key) - keys.begin();
        if (position != 0) {
            vector<T> keys = this->ptr.at(position-1)->getKeys();
            vector<Record *> ptrs = this->ptr.at(position-1)->getPtrs();
            if (keys.size() - 1 > leftKeys) {
                this->ptr.at(position)->insert(keys.back(), ptrs.back(), root);
                this->ptr.at(position-1)->remove(keys.back(), root);

                return true;
            }
        }

        return false;
    }

    template<typename T>
    shared_ptr<Node<T>> RegularNode<T>::mergeNodes(vector<T> keys, vector<Record *> ptrs, const shared_ptr<Node<T>> root) {
        // If this is called it means re-balance this entire side of the tree
        auto loc = upper_bound(this->keys.begin(), this->keys.end(), keys.back()) - this->keys.begin();
        this->ptr.erase(this->ptr.begin() + loc);
        this->keys.erase(this->keys.begin() + loc);
        
        int minNumPtrs = N / 2;
        if (father != nullptr) {
            // When node still has minimum nodes, insert back in
            if (this->ptr.size() > minNumPtrs) {
                for (int j = 0; j < keys.size(); j++) {
                    this->insert(keys[j], ptrs[j], root);
                }

                return root;
            }
            else {      
                // Retrieve all the nodes data here 
                for (int i = 0; i < this->keys.size(); i++) {
                        this->ptr[i]->mergeNodes(keys, ptrs, root);
                }
            }
            
            // Try with parent node to merge
            return father->mergeNodes(keys, ptrs, root);
        }

        // Once root is reached, check if there are ways to insert it
        if (this->ptr.size() == 1) {
            // When it is left with 1 pointer
            for (int i = 0; i < keys.size(); i++) {
                this->ptr[0]->insert(keys[i], ptrs[i], root);
            }
            // Remaining node becomes the new root node
            return this->ptr[0];
        }

        // Since there are still more than 2 pointers, insert the keys and pointers
        for (int i = 0; i < this->ptr.size(); i++) {
            this->insert(keys[i], ptrs[i], root);
        }

        return root;
    }

    template<typename T>
    vector<T> RegularNode<T>::getKeys() {
        return this->keys;
    }

    template<typename T>
    vector<Record *> RegularNode<T>::getPtrs() {
        exit(EXIT_FAILURE);
    }

    template<typename T>
    void RegularNode<T>::display() {
        cout << "Regular Node: { ";
        for (int i = 0; i < keys.size(); i++) {
            if (i + 1 == keys.size()) {
                cout << keys[i] << " ";
            } else {
                cout << keys[i] << ", ";
            }
        }
        cout << "} ->" << endl;

        for (int i = 0; i < ptr.size(); i++) {
            ptr[i]->display();
        }
    }
}