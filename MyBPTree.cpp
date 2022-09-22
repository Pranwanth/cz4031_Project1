#include <memory>
#include <vector>
#include <algorithm>
using ll = long long;

struct Record {
    char id;
    int numVotes;
    float avgVotes;
}; // TODO connect with storage team

namespace BPTree {
    #define nullRecord (shared_ptr<Record>(nullptr))
    using namespace std;

    template <typename T> class Node {
        public:
            virtual const Record * query(const T& val) = 0;
            virtual vector<Record*> rangeQuery(const T& l, const T& r) = 0;            // interface
            virtual void rangeQuery(vector<Record*>& ret, const T& l, const T& r) = 0; // actual call; save movement cost
        protected:
            vector<T> key;
    };

    template <typename T> class RegularNode: public Node<T> {
        using Node<T>::key;
        public:
            const Record * query(const T& val) {
                return queryImmediateNext(val)->query(val);
            }
            vector<Record*> rangeQuery(const T& l, const T& r) {
                vector<Record*> ret;
                rangeQuery(ret, l, r);
                return ret;
            }
            void rangeQuery(vector<Record*>& ret, const T& l, const T& r) {
                queryImmediateNext(l)->rangeQuery(ret, l, r);
            }
        private:
            const static int N = 4; // max number of key
            vector<shared_ptr<Node<T>>> ptr;
            shared_ptr<Node<T>> queryImmediateNext(const T& val) {
                auto loc = upper_bound(key.begin(), key.end(), val) - key.begin();
                return ptr[loc];
            }
            
    };

    template <typename T> class LeafNode: public Node<T> {
        using Node<T>::key;
        public:
            const Record * query(const T& val) {
                auto ans = lower_bound(key.begin(), key.end(), val);
                if(*ans != val)
                    return nullptr;
                else
                    return ptr[ans - key.begin()];
            }
            vector<Record*> rangeQuery(const T& l, const T& r){
                vector<Record*> ret;
                rangeQuery(ret, l, r);
                return ret;
            }
            void rangeQuery(vector<Record*>& ret, const T& l, const T& r){
                auto ans = lower_bound(key.begin(), key.end(), l);
                if(ans == key.end()){
                    return;
                }   
                while(ans != key.end() && *ans <= r){
                    auto pos = ans - key.begin();
                    ret.emplace_back(ptr[pos]);
                    ans++;
                }
                if(ans == key.end()){
                    finalPtr->rangeQuery(ret, l, r);
                }
            }
        private:
            const static int N = 4; // max number of key
            vector<Record*> ptr;
            shared_ptr<LeafNode<T>> finalPtr;
    };
}


int main() {
    auto x = BPTree::RegularNode<int>();
}