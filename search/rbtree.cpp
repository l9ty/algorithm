#include <cstdlib>
#include <cassert>

// < > ==
template <typename Key>
class RBTree {
private:
    struct Node;
    using Node::Red;
    using Node::Black;

public:
    RBTree(): nil(0, Black), root(&nil) {};

    RBTree(const RBTree&) = delete;
    RBTree& operator=(const RBTree&) = delete;

    void Insert(const Key& key);
    void Delete(const Key& key);
    bool Contains(const Key& key);

private:
    Node* root;
    Node nil;

    Node* FindEqOrLast(const Key& key);
    void LeftRotate(Node *x);
    void RightRotate(Node *y);
    void InsertFixup(Node *z);
    void DeleteFixup(Node *z);

    bool NodeHasKey(Node* node, const Key& key) {
        return node != &nil && node->key == key;
    }

    void ReplaceTree(Node* node, Node* rep) {
         if (node->parent == &nil) {
            root = rep;
         } else if (node == node->parent->left) {
            node->parent->left = rep;
         } else {
            node->parent->right = rep;
         }
         rep->parent = node->parent;
    }

    Node* Minimum(Node* node) {
        while(node->left != &nil) {
            node = node->left;
        }
        return node;
    }
};


template <typename Key>
struct RBTree<Key>::Node {
public:
    enum Color { Black = 1, Red };

    Node(Key& k, Color c): 
        key(k), color(c), left(this), right(this), parent(this) {}
    Node(Key& k, Node* l, Node* r, Node* p, Color c): 
        key(k), color(c), left(l), right(r), parent(p) {}

    Color color;
    Key const key;
    Node *left, *right, *parent;
};

template <typename Key>
void RBTree<Key>::Insert(const Key& key) {
    Node* y = FindEqOrLast(key);
    if (NodeHasKey(y, key)) {
        return;
    }

    Node* z = new Node(key, &nil, &nil, y, Red);
    if (y == &nil) {
        root = z;
    } else if (z->key < y->key) {
        y->left = z;
    } else {
        y->right = z;
    }

    // z = { left: &nil, right: &nil, parent: y }
    // if (z == &nil) { return; }
    InsertFixup(z);
}


template <typename Key>
void RBTree<Key>::InsertFixup(Node *z) {
    // z != &nil
    while (z->parent->color == Red) {
        Node* unc;
        if (z->parent == z->parent->parent->left) {
            unc = z->parent->parent->right;
            if (unc->color == Red) {
                unc->color = Black;
                z->parent->color = Black;
                z->parent->parent = Red;
                z = z->parent->parent;
            } else if (z == z->parent->right) {
                // z = z->parent;
                LeftRotate(z);
            }
            z->parent->color = Black;
            z->parent->parent->color = Red;
            RightRotate(z->parent->parent);
        } else {
            unc = z->parent->parent->left;
            if (unc->color == Red) {
                unc->color = Black;
                z->parent->color = Black;
                z->parent->parent->color = Black;
                z = z->parent->parent;
            } else if (z == z->parent->left) {
                // z = z->parent;
                RightRotate(z);
            }
            z->parent->color = Black;
            z->parent->parent->color = Red;
            LeftRotate(z->parent->parent);
        }
    }
    root->color = Black;
}


template <typename Key>
void RBTree<Key>::LeftRotate(Node *x) {
    Node *y = x->left;
    assert(y);

    x->right = y->left;
    if (y->left != &nil) {
        y->left->parent = x;
    }

    y->parent = x->parent;
    if (x == root) {
        root = y;
    } else if (x == x->parent->left) {
        x->parent->left = y;
    } else {
        x->parent->right = y;
    }

    y->left = x;
    x->parent = y;
}


template <typename Key>
void RBTree<Key>::RightRotate(Node *y) {
    Node* x = y->left;
    assert(x);

    y->left = x->right;
    if (x->right != &nil) {
        x->right->parent = y;
    }

    x->parent = y->parent;
    if (y == root) {
        root = y;
    } else if (y == y->parent->left) {
        y->parent->left = x;
    } else {
        y->parent->right = x;
    }

    x->right = y;
    y->parent = x;
}


template <typename Key>
typename RBTree<Key>::Node*
RBTree<Key>::FindEqOrLast(const Key& key) {
    Node* x = root;
    Node* p = &nil;
    while (x != &nil) {
        if (x->key < key) {
            x = x->left;
        } else if (x->key > key) {
            x = x->right;
        } else {
            return x;
        }
    }

    return p;
}

template <typename Key>
bool RBTree<Key>::Contains(const Key& key) {
    Node *x = FindEqOrLast(key);
    return NodeHasKey(x, key);
}


template <typename Key>
void RBTree<Key>::Delete(const Key& key) {
    Node* del = FindEqOrLast(key);
    if (!NodeHasKey(del, key)) {
        return; 
    }

    Node* succ = del;
    auto raw_color = succ->color;

    Node* fix;
    if (del->left == &nil) {
        fix = del->left;
        ReplaceTree(del, del->right);
    } else if (del->right == &nil) {
        fix = del->right;
        ReplaceTree(del, del->left);
    } else {
        succ = Minimum(del->right);
        raw_color = succ->color;
        fix = succ->right;

        if (succ->parent == del) {
            fix->parent = succ;
        } else {
            ReplaceTree(succ, succ->right);
            succ->right = del->right;
            succ->right->parent = succ;
        }

        ReplaceTree(del, succ);
        succ->left = del->left;
        succ->left->parent = succ;
        succ->color = del->color;
    }

    if (raw_color == Black) {
        DeleteFixup(fix);
    }
}

// TODO
template <typename Key>
void RBTree<Key>::DeleteFixup(Node *z) {

}
