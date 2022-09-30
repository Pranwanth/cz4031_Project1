#ifndef A8CD7498_2167_4ED2_9E6D_B5427F0C5413
#define A8CD7498_2167_4ED2_9E6D_B5427F0C5413

#include <memory>
#include <vector>
#include <algorithm>
#include <iostream>
#include <string>
#include <list>

namespace LmaoDB {
    using namespace std;
    template<typename T, typename R> class RegularNode; // for cyclic dependency
    template<typename T, typename R> class LeafNode;

    template<typename T, typename R>
    class Node {
    public:
        virtual vector<R *> query(const T &key) = 0;
        virtual vector<R *> rangeQuery(const T &l, const T &r) = 0;                       // interface
        virtual void rangeQuery(vector<R*> &ret, const T &l, const T &r) = 0; // actual call; save movement cost
        virtual shared_ptr<Node<T, R>> insert(const T &key, R * record, const shared_ptr<Node<T, R>>& oldRoot) = 0;
        virtual shared_ptr<Node<T, R>> remove(const T &key, shared_ptr<Node<T, R>> root) = 0;
        virtual shared_ptr<Node<T, R>> mergeNodes(vector<T> keys, vector<R *> ptrs, shared_ptr<Node<T, R>> root) = 0;
        virtual vector<T> getKeys() = 0;
        virtual void display() = 0;
        virtual int treeSize() = 0;
        virtual int numNodes() = 0;
        virtual int getN() = 0;
        virtual void displayFirstChild(bool caller) = 0;
        RegularNode<T, R> *father = nullptr;
    protected:
        const static int N = 2; // max number of key
        vector<T> keys;
        virtual shared_ptr<Node<T, R>> balance(const shared_ptr<Node<T, R>>& oldRoot) = 0;
    };

    // @brief abbreviation for Node Pointer
    template<typename T, typename R> using SharedNodePtr = shared_ptr<Node<T, R>>;

    template<typename T, typename R>
    class LeafNode : public Node<T, R> {
        using Node<T, R>::keys, Node<T, R>::N, Node<T, R>::father;
    public:
        explicit LeafNode(RegularNode<T, R>* father = nullptr);
        vector<R*> query(const T &key);
        vector<R*> rangeQuery(const T &l, const T &r);
        void rangeQuery(vector<R *> &ret, const T &l, const T &r);
        shared_ptr<Node<T, R>> insert(const T &key, R * record, const shared_ptr<Node<T, R>>& oldRoot);

        shared_ptr<Node<T, R>> remove(const T &key, shared_ptr<Node<T, R>> root);
        shared_ptr<Node<T, R>> mergeNodes(vector<T> keys, vector<R *> ptrs, shared_ptr<Node<T, R>> root);
        vector<T> getKeys();
        vector<list<R*>> getPtrs();
        void display();
        int treeSize();
        int numNodes();
        int getN();
        void displayFirstChild(bool caller);
    private:
        vector<list<R*>> ptr;
        LeafNode<T, R> *finalPtr;
        shared_ptr<Node<T, R>> balance(const shared_ptr<Node<T, R>>& oldRoot); // attempt to balance current node recursively. return the new root.
    };

    template<typename T, typename R>
    class RegularNode : public Node<T, R> {
        using Node<T, R>::keys, Node<T, R>::father, Node<T, R>::N;
    public:
        explicit RegularNode(RegularNode<T, R>* father_); // this constructor do not register in father.
        vector<R *> query(const T &key);
        vector<R *> rangeQuery(const T &l, const T &r);
        void rangeQuery(vector<R *> &ret, const T &l, const T &r);
        shared_ptr<Node<T, R>> insert(const T &key, R * record, const shared_ptr<Node<T, R>>& oldRoot);
        shared_ptr<Node<T, R>> remove(const T &key, shared_ptr<Node<T, R>> root);
        bool helpSibling(const T &key, shared_ptr<Node<T, R>> root);
        shared_ptr<Node<T, R>> mergeNodes(vector<T> keys, vector<R *> ptrs, shared_ptr<Node<T, R>> root);
        vector<T> getKeys();
        vector<R *> getPtrs();
        void display();
        int treeSize();
        int numNodes();
        int getN();
        void displayFirstChild(bool caller);
        friend LeafNode<T, R>; // I need to expose insertSubNode to Leaf's private functions; no need extra template here
        friend Node<T, R>; // needed for access father
    private:
        vector<shared_ptr<Node<T, R>>> ptr;
        Node<T, R>* queryImmediateNext(const T &key);
        void insertSubNode(Node<T, R>* newPtr, const T &key); // insert new sub node (and manage its lifecycle), does not balance
        shared_ptr<Node<T, R>> balance(const shared_ptr<Node<T, R>>& oldRoot); // attempt to balance current node recursively. return the new root.
    };
}

#include "BPTreeLeafNode.tpp"
#include "BPTreeRegularNode.tpp"

#endif /* A8CD7498_2167_4ED2_9E6D_B5427F0C5413 */
