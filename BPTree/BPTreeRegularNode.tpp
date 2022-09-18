#include "BPTree.h"

namespace LmaoDB {
    template<typename T>
    RegularNode<T>::RegularNode(const shared_ptr<Node<T>> &currFather) {
        father = new weak_ptr<Node<T>>(currFather);
    }

    template<typename T>
    const Record *RegularNode<T>::query(const T &key) {
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
    SharedNodePtr<T> RegularNode<T>::insert(const T &key, const Record *const record) {
        return queryImmediateNext(key)->insert(key, record);
//        balance();
    }

    template<typename T>
    SharedNodePtr<T> RegularNode<T>::queryImmediateNext(const T &key) {
        auto loc = upper_bound(keys.begin(), keys.end(), key) - keys.begin();
        return ptr[loc];
    }

    template<typename T>
    void RegularNode<T>::insertSubNode(NodePtr<T> newPtr, const T &key) {
        assert(keys.find(key) == keys.end());
        uint32_t position = lower_bound(keys.begin(), keys.end(), key)- keys.begin();
        keys.insert(keys.begin() + position, key);
        ptr.insert(ptr.begin() + position + 1, shared_ptr<Node<T>>(
                newPtr)); // such location will always be valid; now father will manage the life of son.
    }

    template<typename T>
    SharedNodePtr<T> RegularNode<T>::balance() {
        if (keys.size() <= N) return (father == nullptr ? shared_ptr<Node<T>>(this) : father->balance());
        else {
            assert(keys.size() == N + 1 && keys.size() == ptr.size());
            cout << "Balance() triggered: keys.size() = " << keys.size() << endl;
            shared_ptr<Node<T>> newNode(new RegularNode(father));

            // 1. Populate Right Node

            for (int i = N / 2 + 1; i < keys.size(); ++i) newNode->keys.emplace_back(move(keys[i]));
            for (int i = N / 2 + 1; i < ptr.size(); ++i) newNode->ptr.emplace_back(ptr[i]);
            auto extraKey = keys[N / 2];
            assert(newNode->keys.size() >= N / 2);
            assert(newNode->ptr.size() >= N / 2 + 1);
            assert(newNode->keys.size() + 1 == newNode->ptr.size());

            // 2. Clean Left Node
            keys.resize(N / 2), ptr.resize(N / 2 + 1);

            // 3. register new "node"
            if (father == nullptr) { // if there is no father, we can trivially build father with 1 key & 2 ptr
                father = new RegularNode<T>(nullptr);
                father->ptr.emplace_back(SharedNodePtr<T>(this));
                father->ptr.emplace_back(newNode);
                father->keys.emplace_back(extraKey);
            } else {

            }

            // 4. update new father for right half

            assert(father != nullptr); // split of nonleaf node always generate father
            return father->balance();
        }
    }
}