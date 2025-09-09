#include <stdio.h>
#include <stdlib.h>
#include <limits.h> // UINT_MAX -> chave proibida (valor máximo de unsigned)

#include "patricia.h"

void inicializa(Node **arvore)                  // dummy -> funcionar corretamente mesmo quando nenhum dado real foi inserido
{                                               // em uma PATRICIA, não temos apontadores NULL
    *arvore = malloc(sizeof(Node));
    (*arvore)->chave = UINT_MAX;                // chave proibida
    (*arvore)->esq = (*arvore)->dir = *arvore;
    (*arvore)->bit = -1;                        // indica começo da árvore (dummy)
}

// retorna o k-ésimo bit da chave
unsigned bit(unsigned chave, int k)             // k -> índice do bit que queremos analisar
{
    return chave >> (bits_na_chave - 1 - k) & 1;// "& 1" é um AND com 1 (mantém apenas o menos significativo)
}

// busca recursiva
Node* busca_rec(Node *arvore, unsigned x, int w)// w -> índice do bit que comparamos anteriormente (do pai)
{
    if (arvore->bit <= w)                       // se o indice do bit que queremos comparar agora for menor que o anterior,
        return arvore;                          // significa que voltamos ou subimos na árvore (chegamos em uma "folha"), assim, retornamos para comparar-lo depois

    if (bit(x, arvore->bit) == 0)               // senão, seguimos a busca analisando o bit no índice indicado pelo nó
        return busca_rec(arvore->esq, x, arvore->bit);
    else
        return busca_rec(arvore->dir, x, arvore->bit);
}

// função para chamar busca e comparar chave
Node* busca(Node* arvore, unsigned x)
{
    Node *t = busca_rec(arvore->esq, x, -1);    // inciamos a busca descendo pela esquerda do dummy, que retorna um "nó folha" // t -> target

    return t->chave == x ? t : NULL;            // comparamos a chave do nó que recebemos com a chave que procurávamos
}                                               // igual = retorna nó, diferente = retorna null

// inserção recursiva
Node* insere_rec(Node *arvore, unsigned chave, int w, Node *pai) // w -> ponto de inserção (bits se diferem)
{
    Node *novo;

    // caso base (achou o ponto de inserção)
    if ((arvore->bit >= w) || (arvore->bit <= pai->bit))    // (ponto de quebra) || (cheguei numa folha)
    {
        novo = malloc(sizeof(Node));                        // novo nó
        novo->chave = chave;
        novo->bit = w;

        if (bit(chave, novo->bit) == 1)                     // "direita ou esquerda"
        {
            novo->esq = arvore;
            novo->dir = novo;                               // aponta para si próprio pela direita
        }
        else
        {
            novo->esq = novo;                               // aponta para si próprio pela esquerda
            novo->dir = arvore;
        }

        return novo;
    }

    // se o ponto de inserção ainda não foi encontrado, avançamos (a direita ou esquerda)
    if (bit(chave, arvore->bit) == 0)
        arvore->esq = insere_rec(arvore->esq, chave, w, arvore);
    else
        arvore->dir = insere_rec(arvore->dir, chave, w, arvore);

    return arvore;
}

// função que chama inserção
void insere(Node **arvore, unsigned chave)
{
    int i;

    Node *t = busca_rec((*arvore)->esq, chave, -1);         // busca é chamada para ver com qual nó que vai dar conflito

    if (chave == t->chave)                                  // se a chave que retornou da busca for igual a que queremos inserir,
        return;                                             // apenas retornamos

    for (i = 0; bit(chave, i) == bit(t->chave, i); i++);    // loop para identificar o primeiro bit distinto entre as chaves

    (*arvore)->esq = insere_rec((*arvore)->esq, chave, i, *arvore); // insere recursivamente
}
