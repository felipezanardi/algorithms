#ifndef DST_H_INCLUDED
#define DST_H_INCLUDED

#define bits_na_chave 4

typedef struct node
{
    unsigned chave;
    struct node *esq;
    struct node *dir;
} Node;

// busca
unsigned bit(unsigned chave, int k);
Node* busca_rec(Node *arvore, unsigned x, int nivel);
Node* busca(Node *arvore, unsigned x);

// inserção
Node* insere_rec(Node *arvore, unsigned chave, int nivel);
void insere(Node **arvore, unsigned chave);

// remoção
Node* remover_rec(Node *arvore, unsigned chave, int nivel);
void remover(Node **arvore, unsigned chave);

// imprimir
void imprimir(Node *raiz);

#endif // DST_H_INCLUDED
