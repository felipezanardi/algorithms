#ifndef BTREE_H_INCLUDED
#define BTREE_H_INCLUDED

#include <stdbool.h>

#define ORDEM 3

/*
 * TO-DO:
 *  - n�o usar vari�vel global
 *  - leitura/escrita de arquivos (trabalho)
 */

// typedef int element;

typedef struct node
{
    int n;                          // numero atual de chaves dentro do n�
    bool isLeaf;                    // � uma folha? 1 = true, 0 = false
    int *chave;                     // keys     (2T - 1)
    struct node **filho;            // childs   (2T)
} BtreeNode;

typedef struct
{
    BtreeNode *root;
    int t;                          // ordem
} Btree;

// INICIALIZA��O & BUSCA
BtreeNode *allocateNode(int t, bool leaf);
void BtreeCreate(Btree *T, int t);

BtreeNode *BtreeSearch(BtreeNode *x, int k);

// INSER��O
void BtreeSplitChild(BtreeNode *x, int i);
void BtreeInsertNonFull(BtreeNode *x, int k);
void BtreeInsert(Btree *T, int k);

// REMO��O
void BtreeDeleteLeaf(BtreeNode *x, int i);

void BtreeMerge(BtreeNode *x, int i);
void BtreeDeleteNonLeaf(BtreeNode *x, int i);

void BtreeBorrowLeft(BtreeNode *x, int i);
void BtreeBorrowRight(BtreeNode *x, int i);
void BtreeFill(BtreeNode *x, int i);

void BtreeDeletion(BtreeNode *x, int k);
void BtreeDelete(Btree *T, int k);

// OUTROS
void BtreePrintNode(BtreeNode *x, int level);
void BtreePrint(Btree *T);

#endif // BTREE_H_INCLUDED
