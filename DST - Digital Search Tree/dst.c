#include <stdio.h>
#include <stdlib.h>

#include "dst.h"

// retorna o k-�simo bit da chave
unsigned bit(unsigned chave, int k)
{
    return chave >> (bits_na_chave - 1 - k) & 1; // "& 1" � um AND com 1 (mant�m apenas o menos significativo)
}

// busca recursivamente
Node* busca_rec(Node *arvore, unsigned x, int nivel)
{
    if (arvore == NULL)                             // se arvore for nula, n�o tem o que retornar
        return NULL;

    if (x == arvore->chave)                         // se s�o iguais, encontramos, retorna o n�
        return arvore;

    if (bit(x, nivel) == 0)                         // se o k-�simo bit � 0:
        return busca_rec(arvore->esq, x, nivel+1);  // avan�amos a esquerda
    else
        return busca_rec(arvore->dir, x, nivel+1);  // sen�o (1), avan�amos a direita
}

// chama a busca recursiva
Node* busca(Node *arvore, unsigned x)
{
    return busca_rec(arvore, x, 0);
}

// inserir recursivamente
Node* insere_rec(Node *arvore, unsigned chave, int nivel)
{
    Node *novo;

    if (arvore == NULL)                     // se a arvore/n� for vazio, temos que criar um novo n�
    {
        novo = malloc(sizeof(Node));
        novo->chave = chave;
        novo->esq = novo->dir = NULL;
        return novo;
    }

    if (chave == arvore->chave)             // se a chave que queremos adicionar j� esta na �rvore,
        return arvore;                      // apenas retornamos

    if (bit(chave, nivel) == 0)                                 // se o k-�simo bit � 0:
        arvore->esq = insere_rec(arvore->esq, chave, nivel+1);  // a esquerda recebe o que retornar
    else
        arvore->dir = insere_rec(arvore->dir, chave, nivel+1);  // a direita recebe o que retornar

    return arvore;
}

// chama fun��o de inser��o
void insere(Node **arvore, unsigned chave)
{
    *arvore = insere_rec(*arvore, chave, 0);
}

// remover recursivamente
Node* remover_rec(Node *arvore, unsigned chave, int nivel)
{
    if (arvore == NULL)                                         // se o n� � vazio, chave n�o est� na �rvore
    {
        printf("Chave nao encontrada!\n");
        return NULL;
    }

    if (chave == arvore->chave)                                 // encontramos a chave
    {
        if (arvore->esq == NULL && arvore->dir == NULL)         // caso 1: n� folha
        {
            free(arvore);
            return NULL;
        }
                                                                // caso 2: n� tem um filho
        else if (arvore->esq != NULL && arvore->dir == NULL)    // caso 2a: n� tem um filho a esquerda
        {
            Node *filho = arvore->esq;

            free(arvore);
            return filho;
        }

        else if (arvore->esq == NULL && arvore->dir != NULL)    // caso 2b: n� tem um filho a direita
        {
            Node *filho = arvore->dir;

            free(arvore);
            return filho;
        }

        else if (arvore->esq != NULL && arvore->dir != NULL)    // caso 3: n� tem dois filhos
        {
            Node* predecessor = arvore->esq;                    // a partir daqui, iremos pegar o maior predecessor
                                                                // para depois substituir no lugar da chave atual
            while (predecessor->dir != NULL)
            {
                predecessor = predecessor->dir;
            }

            arvore->chave = predecessor->chave;                 // substituimos

            arvore->esq = remover_rec(arvore->esq, predecessor->chave, nivel+1); // removemos o predecessor de onde pegamos
        }
    }
    else
    {
        if (bit(chave, nivel) == 0)                                 // se o k-�simo bit � 0:
            arvore->esq = remover_rec(arvore->esq, chave, nivel+1); // avan�amos a esquerda
        else
            arvore->dir = remover_rec(arvore->dir, chave, nivel+1); // avan�amos a direita
    }

    return arvore;
}

// chama fun��o de remo��o
void remover(Node **arvore, unsigned chave)
{
    *arvore = remover_rec(*arvore, chave, 0);
}

// fun��o para imprimir
void imprimir(Node *arvore)
{
    if (arvore != NULL)
    {
        imprimir(arvore->esq);          // imprime sub�rvore esquerda
        printf("%u ", arvore->chave);   // imprime a chave do n� atual
        imprimir(arvore->dir);          // imprime sub�rvore direita
    }
}
