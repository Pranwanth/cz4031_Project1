#ifndef A8CD7498_2167_4ED2_9E6D_B5427F0C5413
#define A8CD7498_2167_4ED2_9E6D_B5427F0C5413

#include <memory>
#include <vector>
#include <algorithm>
#include <iostream>
#include <string>

struct Record {
    int key;
    std::string val;
}; // TODO connect with storage team

namespace LmaoDB {
    using namespace std;
    template<typename T> class RegularNode; // for cyclic dependency

    template<typename T>
    class Node {
    public:
        virtual Record * query(const T &key) = 0;
        virtual vector<Record *> rangeQuery(const T &l, const T &r) = 0;                       // interface
        virtual void rangeQuery(vector<Record *> &ret, const T &l, const T &r) = 0; // actual call; save movement cost
        virtual shared_ptr<Node<T>> insert(const T &key, Record *const record, const shared_ptr<Node<T>>& oldRoot) = 0;
        virtual shared_ptr<Node<T>> remove(const T &key, const shared_ptr<Node<T>> root) = 0;
        virtual shared_ptr<Node<T>> mergeNodes(vector<T> keys, vector<Record *> ptrs, const shared_ptr<Node<T>> root) = 0;
        virtual vector<T> getKeys() = 0;
        virtual vector<Record *> getPtrs() = 0;
        virtual void display() = 0;
    protected:
        const static int N = 4; // max number of key
        RegularNode<T> *father = nullptr;
        vector<T> keys;
        virtual shared_ptr<Node<T>> balance(const shared_ptr<Node<T>>& oldRoot) = 0;
    };

    // @brief abbreviation for Node Pointer
    template<typename T> using SharedNodePtr = shared_ptr<Node<T>>;

    template<typename T>
    class LeafNode : public Node<T> {
        using Node<T>::keys, Node<T>::N, Node<T>::father;
    public:
        LeafNode(LeafNode<T>* LeftPtr = nullptr);
        Record* query(const T &key);
        vector<Record *> rangeQuery(const T &l, const T &r);
        void rangeQuery(vector<Record *> &ret, const T &l, const T &r);
        shared_ptr<Node<T>> insert(const T &key, Record *const record, const shared_ptr<Node<T>>& oldRoot);

        shared_ptr<Node<T>> remove(const T &key, const shared_ptr<Node<T>> root);
        shared_ptr<Node<T>> mergeNodes(vector<T> keys, vector<Record *> ptrs, const shared_ptr<Node<T>> root);
        vector<T> getKeys();
        vector<Record *> getPtrs();
        void display();
    private:
        vector<Record *> ptr;
        LeafNode<T> *finalPtr;
        shared_ptr<Node<T>> balance(const shared_ptr<Node<T>>& oldRoot); // attempt to balance current node recursively. return the new root.
    };

    template<typename T>
    class RegularNode : public Node<T> {
        using Node<T>::keys, Node<T>::father, Node<T>::N;
    public:
        RegularNode(RegularNode<T>* const father_); // this constructor do not register in father.
        Record * query(const T &key);
        vector<Record *> rangeQuery(const T &l, const T &r);
        void rangeQuery(vector<Record *> &ret, const T &l, const T &r);
        shared_ptr<Node<T>> insert(const T &key, Record *const record, const shared_ptr<Node<T>>& oldRoot);

        shared_ptr<Node<T>> remove(const T &key, const shared_ptr<Node<T>> root);
        bool helpSibiling(const T &key, const shared_ptr<Node<T>> root);
        shared_ptr<Node<T>> mergeNodes(vector<T> keys, vector<Record *> ptrs, const shared_ptr<Node<T>> root);
        vector<T> getKeys();
        vector<Record *> getPtrs();
        void display();
        
        friend LeafNode<T>; // I need to expose insertSubNode to Leaf's private functions; no need extra template here
    private:
        vector<shared_ptr<Node<T>>> ptr;
        Node<T>* queryImmediateNext(const T &key);
        void insertSubNode(Node<T>* newPtr, const T &key); // insert new subnode (and manage its lifecycle), does not balance
        shared_ptr<Node<T>> balance(const shared_ptr<Node<T>>& oldRoot); // attempt to balance current node recursively. return the new root.
    };
}

#include "BPTreeLeafNode.tpp"
#include "BPTreeRegularNode.tpp"

#endif /* A8CD7498_2167_4ED2_9E6D_B5427F0C5413 */
