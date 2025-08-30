#ifndef BTREE_H_INCLUDED
#define BTREE_H_INCLUDED

#include <stdbool.h>

#define ORDEM 3

/*  TO-DO:
 *  - implementar remoção
 */

// typedef int element;

typedef struct node
{
    int n;                          // numero atual de chaves dentro do nó
    bool isLeaf;                    // é uma folha? 1 = true, 0 = false
    int *chave;                     // keys     (2T - 1)
    struct node **filho;            // childs   (2T)
} BtreeNode;

typedef struct
{
    BtreeNode *root;
    int t;                          // ordem
} Btree;

BtreeNode *allocateNode(int t, bool leaf);

void BtreeCreate(Btree *T, int t);

BtreeNode *BtreeSearch(BtreeNode *x, int k);

void BtreeSplitChild(BtreeNode *x, int i);

void BtreeInsertNonFull(BtreeNode *x, int k);

void BtreeInsert(Btree *T, int k);

#endif // BTREE_H_INCLUDED
