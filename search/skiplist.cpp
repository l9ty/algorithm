// Reference: leveldb/db/skiplist.h

#include <cstring>
#include <cstdlib>
#include <random>
#include <functional>


template <typename Key>
class SkipList {
private:
    struct Node;

public:
    explicit SkipList();

    SkipList(const SkipList&) = delete;
    SkipList& operator=(const SkipList&) = delete;

    bool Contains(const Key& key);
    void Insert(const Key& key);
private:
    enum { kMaxHeight = 14 };

    int RandomHeight();
    Node* NewNode(const Key& key, int height);
    Node* FindGreaterOrEqual(const Key& key, Node **prev);
    bool KeyIsAfterNode(const Key& key, Node *node);

    int max_height_;
    Node* const head_;
    decltype(std::bind(std::uniform_int_distribution<>{0, 0},
                       std::default_random_engine{})) random_;
};


template <typename Key>
struct SkipList<Key>::Node {
    explicit Node(const Key& k): key(k) {}
    Key const key;

    Node* Next(int n)            { return next_[n]; }
    void SetNext(int n, Node *x) { next_[n] = x; }
private:
    Node *next_[1];
};


template <typename Key>
SkipList<Key>::SkipList():
    max_height_(1),
    head_(NewNode(0, kMaxHeight)) {

    int seed = static_cast<int>(this);
    random_ = std::bind(std::uniform_int_distribution<>{1, 4},
                        std::default_random_engine{seed});
    for (int i = 0; i < kMaxHeight; i++) {
        head_->SetNext(nullptr, i);
    }
}

template <typename Key>
typename SkipList<Key>::Node*
SkipList<Key>::NewNode(const Key& key, int height)
{
    size_t size = sizeof(Node) + sizeof(Node*) * (height-1);
    void* p = (Node *)calloc(1, size);
    return new (p) Node(key);
}


template <typename Key>
int SkipList<Key>::RandomHeight()
{
    int height = 1;
    while (height < kMaxHeight && random_() % 4 == 0) {
        height++;
    }
    return height;
}

template <typename Key>
bool SkipList<Key>::KeyIsAfterNode(const Key& key, Node* node)
{
    // NOTE: nullptr return false
    return node != nullptr && node->key < key;
}


template <typename Key>
typename SkipList<Key>::Node*
SkipList<Key>::FindGreaterOrEqual(const Key& key, Node **prev)
{
    Node *x = head_;
    int level = max_height_ - 1;
    while (true) {
        Node *next = x->Next(level);
        if (KeyIsAfterNode(key, next)) {
            x = next;
        } else {
            if (prev != nullptr) prev[level] = x;
            if (level == 0) {
                return next;
            } else {
                level--;
            }
        }
    }
}


template <typename Key>
bool SkipList<Key>::Contains(const Key& key)
{
    Node *x = FindGreaterOrEqual(key, nullptr);
    return x != nullptr && x->key == key;
}


template <typename Key>
void SkipList<Key>::Insert(const Key& key)
{
    Node* prev[kMaxHeight];
    Node* x = FindGreaterOrEqual(key, prev);

    assert(x == nullptr || x->key != x->key);

    int height = RandomHeight();
    if (height > max_height_) {
        for (int i = max_height_; i < height; i++) {
            prev[i] = head_;
        }
        max_height_ = height;
    }

    x = NewNode(key, height);
    for (int i = 0; i < height; i++) {
        x->SetNext(prev[i]->Next(), i);
        prev[i]->SetNext(x, i);
    }
}
