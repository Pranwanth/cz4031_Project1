#ifndef A8CD7498_2167_4ED2_9E6D_B5427F0C5413
#define A8CD7498_2167_4ED2_9E6D_B5427F0C5413

#include <memory>
#include <vector>
#include <algorithm>
#include <iostream>
#include <string>

namespace LmaoDB {
    using namespace std;
    template<typename T> class RegularNode; // for cyclic dependency
    template<typename T> class LeafNode;

    template<typename T, typename R>
    class Node {
    public:
        virtual R * query(const T &key) = 0;
        virtual vector<R *> rangeQuery(const T &l, const T &r) = 0;                       // interface
        virtual void rangeQuery(vector<R *> &ret, const T &l, const T &r) = 0; // actual call; save movement cost
        virtual shared_ptr<Node<T, R>> insert(const T &key, R * record, const shared_ptr<Node<T, R>>& oldRoot) = 0;
        virtual shared_ptr<Node<T, R>> remove(const T &key, shared_ptr<Node<T, R>> root) = 0;
        virtual shared_ptr<Node<T, R>> mergeNodes(vector<T> keys, vector<R *> ptrs, shared_ptr<Node<T, R>> root) = 0;
        virtual vector<T> getKeys() = 0;
        virtual vector<R *> getPtrs() = 0;
        virtual void display() = 0;
        RegularNode<T> *father = nullptr;
    protected:
        const static int N = 2; // max number of key
        vector<T> keys;
        virtual shared_ptr<Node<T, R>> balance(const shared_ptr<Node<T, R>>& oldRoot) = 0;
    };

    // @brief abbreviation for Node Pointer
    template<typename T> using SharedNodePtr = shared_ptr<Node<T>>;

    template<typename T>
    class LeafNode : public Node<T> {
        using Node<T>::keys, Node<T>::N, Node<T>::father;
    public:
        explicit LeafNode(RegularNode<T>* father = nullptr);
        Record* query(const T &key);
        vector<Record *> rangeQuery(const T &l, const T &r);
        void rangeQuery(vector<Record *> &ret, const T &l, const T &r);
        shared_ptr<Node<T>> insert(const T &key, Record * record, const shared_ptr<Node<T>>& oldRoot);

        shared_ptr<Node<T>> remove(const T &key, shared_ptr<Node<T>> root);
        shared_ptr<Node<T>> mergeNodes(vector<T> keys, vector<Record *> ptrs, shared_ptr<Node<T>> root);
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
        explicit RegularNode(RegularNode<T>* father_); // this constructor do not register in father.
        Record * query(const T &key);
        vector<Record *> rangeQuery(const T &l, const T &r);
        void rangeQuery(vector<Record *> &ret, const T &l, const T &r);
        shared_ptr<Node<T>> insert(const T &key, Record * record, const shared_ptr<Node<T>>& oldRoot);
        shared_ptr<Node<T>> remove(const T &key, shared_ptr<Node<T>> root);
        bool helpSibling(const T &key, shared_ptr<Node<T>> root);
        shared_ptr<Node<T>> mergeNodes(vector<T> keys, vector<Record *> ptrs, shared_ptr<Node<T>> root);
        vector<T> getKeys();
        vector<Record *> getPtrs();
        void display();
        friend LeafNode<T>; // I need to expose insertSubNode to Leaf's private functions; no need extra template here
        friend Node<T>; // needed for access father
    private:
        vector<shared_ptr<Node<T>>> ptr;
        Node<T>* queryImmediateNext(const T &key);
        void insertSubNode(Node<T>* newPtr, const T &key); // insert new sub node (and manage its lifecycle), does not balance
        shared_ptr<Node<T>> balance(const shared_ptr<Node<T>>& oldRoot); // attempt to balance current node recursively. return the new root.
    };
}

#include "BPTreeLeafNode.tpp"
#include "BPTreeRegularNode.tpp"

#endif /* A8CD7498_2167_4ED2_9E6D_B5427F0C5413 */
