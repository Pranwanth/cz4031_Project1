
namespace LmaoDB {
    template<typename T> using NodePtr = Node<T> *;

    template<typename T>
    LeafNode<T>::LeafNode(NodePtr<T> LeftPtr) {
        if (LeftPtr != nullptr) {
            auto RightPtr = LeftPtr->finalPtr;
            this->finalPtr = RightPtr;
            LeftPtr->finalPtr = this;
        } // else current node is root, no further action is needed
    }

    template<typename T>
    const Record *LeafNode<T>::query(const T &key) {
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
    SharedNodePtr<T> LeafNode<T>::insert(const T &key, const Record *const record) {
        auto pos = lower_bound(keys.begin(), keys.end(), key) - keys.begin();
        keys.insert(keys.begin() + pos, key);
        ptr.insert(ptr.begin() + pos, record);
        return balance();
    }

    template<typename T>
    SharedNodePtr<T> LeafNode<T>::balance() {
        if (keys.size() <= N) return (father == nullptr ? shared_ptr<Node<T>>(this) : father->balance());
        assert(keys.size() == ptr.size() && keys.size() == N + 1);
        cout << "Balance() triggered: keys.size() = " << keys.size() << endl;
        // left has floor((N + 1) / 2) keys, right has rest
        NodePtr<T> newNode = new LeafNode(this);
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
}