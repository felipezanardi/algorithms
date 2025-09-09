#ifndef PATRICIA_H_INCLUDED
#define PATRICIA_H_INCLUDED

#define bits_na_chave 4

typedef struct node
{
    unsigned chave;
    int bit;        // bit comparativo

    struct node *esq;
    struct node *dir;
} Node;

// bit & dummy
unsigned bit(unsigned chave, int k);
void inicializa(Node **arvore);

// busca
Node* busca_rec(Node *arvore, unsigned x, int w);
Node* busca(Node* arvore, unsigned x);

//inserção
Node* insere_rec(Node *arvore, unsigned chave, int w, Node *pai);
void insere(Node **arvore, unsigned chave);

#endif // PATRICIA_H_INCLUDED
