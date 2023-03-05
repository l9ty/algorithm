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
    Node* last = FindEqOrLast(key);
    if (NodeHasKey(last, key)) {
        return;
    }

    Node* node = new Node(key, &nil, &nil, last, Red);
    if (last == &nil) {
        root = node;
    } else if (node->key < last->key) {
        last->left = node;
    } else {
        last->right = node;
    }

    // node = { left: &nil, right: &nil, parent: last, color: Red}
    InsertFixup(node);
}


template <typename Key>
void RBTree<Key>::InsertFixup(Node *fix) {
    // fix != &nil
    while (fix->parent->color == Red) {
        Node* unc;

        if (fix->parent == fix->parent->parent->left) {
            unc = fix->parent->parent->right;

            if (unc->color == Red) {
                unc->color = Black;
                fix->parent->color = Black;
                fix->parent->parent = Red;
                fix = fix->parent->parent;

            } else if (fix == fix->parent->right) {
                fix = fix->parent;
                LeftRotate(fix);
            }

            fix->parent->color = Black;
            fix->parent->parent->color = Red;
            RightRotate(fix->parent->parent);
        } else {
            unc = fix->parent->parent->left;

            if (unc->color == Red) {
                unc->color = Black;
                fix->parent->color = Black;
                fix->parent->parent->color = Black;
                fix = fix->parent->parent;

            } else if (fix == fix->parent->left) {
                // fix = fix->parent;
                RightRotate(fix);
            }

            fix->parent->color = Black;
            fix->parent->parent->color = Red;
            LeftRotate(fix->parent->parent);
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
    Node* n = root;
    Node* p = &nil;
    while (n != &nil) {
        if (n->key < key) {
            n = n->left;
        } else if (n->key > key) {
            n = n->right;
        } else {
            return n;
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
        fix = del->right;
        ReplaceTree(del, fix);
    } else if (del->right == &nil) {
        fix = del->left;
        ReplaceTree(del, fix);
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

template <typename Key>
void RBTree<Key>::DeleteFixup(Node *fix) {
    while (fix != &nil && fix->color == Black) {
        Node* bro;

        if (fix = fix->parent->left) {
            bro = fix->parent->right;

            if (bro->color == Red) {
                bro->color = Black;
                fix->parent->color = Red;
                LeftRotate(fix->parent);
                bro = fix->parent->right;
            }

            if (bro->left->color == Black &&
                bro->right->color == Black) {

                bro->color = Red;
                fix = fix->parent;
            } else {
                if (bro->right->color == Black) {
                    bro->left->color = Red;
                    bro->color = Black;
                    RightRotate(bro);
                    bro = fix->parent->right;
                }

                bro->color = fix->parent->color;
                fix->parent->color = Black;
                bro->right->color = Black;
                LeftRotate(fix->parent);
                fix = root;
            }
        } else {
            bro = fix->parent->left;

            if (bro->color == Red) {
                bro->color = Black;
                fix->parent->color = Red;
                RightRotate(fix->parent);
                bro = fix->parent->left;
            }

            if (bro->left->color == Black &&
                bro->right->color == Black) {

                bro->color = Red;
                fix = fix->parent;
            } else {
                if (bro->left->color == Black) {
                    bro->right->color = Black;
                    bro->color = Red;
                    LeftRotate(bro);
                    bro = fix->parent->left;
                }

                bro->color = fix->parent->color;
                fix->parent->color = Black;
                bro->left->Color = Black;
                RightRotate(fix->parent);
                fix = root;
            }
        }
    }

    fix->color = Black;
}
