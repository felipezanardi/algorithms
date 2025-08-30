#include <stdlib.h>

#include "btree.h"

int t = ORDEM; // Ordem -> Variável Global

BtreeNode *allocateNode(int t, bool leaf)
{
    BtreeNode *x = malloc(sizeof(BtreeNode));

    x->n = 0;
    x->isLeaf = leaf;

    x->chave = malloc((2*t-1) * sizeof(int));
    x->filho = malloc((2*t) * sizeof(BtreeNode));

    return x;
}

// função deve ter complexidade O(1) (de acordo com o livro do Cormen)
void BtreeCreate(Btree *T, int t)
{
    BtreeNode *x = allocateNode(t, true);

    T->t = t;
    T->root = x;
}

// parâmetro: ponteiro para o nó de raiz x da árvore, e uma chave k que deve ser procurada
BtreeNode *BtreeSearch(BtreeNode *x, int k)
{
    int i=0;

    while (i < x->n && k > x->chave[i])         // achar a posição correta
        i++;

    if (i < x->n && k == x->chave[i])           // oq eu procuro é igual a essa chave?
        return x;                               // retorna o nó em que a chave se encontra (o endereço)

    else if (x->isLeaf)                         // se chegarmos em uma folha e nenhuma chave foi encontrada, voltamos sem nada
        return NULL;

    else
        return BtreeSearch(x->filho[i], k);     // avançar ainda mais na busca
}

// parâmetro: x é um nó não cheio, e i um indice que indica um filho cheio de x (x->filho[i])
void BtreeSplitChild(BtreeNode *x, int i)
{
    BtreeNode *y = x->filho[i];                 // y = filho cheio
    BtreeNode *z = allocateNode(t, y->isLeaf);  // z = novo nó

    z->n = t-1;

    for (int j = 0; j < t-1; j++)               // passa chaves do filho cheio para o novo nó (apenas chaves > chave mediana)
    {
        z->chave[j] = y->chave[j+t];
    }

    if (!y->isLeaf)                             // filho cheio não é folha? (tem filho?) sim = entra no loop
    {
        for (int j = 0; j < t; j++)             // passa os filhos do filho cheio para o novo nó z (apenas os filhos pós a chave mediana)
        {
            z->filho[j] = y->filho[j+t];
        }
    }
    y->n = t-1;                                 // filho cheio tem seu n ajustado, ou seja, não é mais cheio

    for (int j = x->n; j >= i+1; j--)           // muda as posições dos filhos de x (jogando-os para o lado direito), abrindo espaço para inserir z como novo filho de x
    {                                   // old: for (int j = x->n+1; j > i+1; j--) --> dava erro
        x->filho[j+1] = x->filho[j];
    }
    x->filho[i+1] = z;                          // z vira novo filho de x

    for (int j = x->n-1; j >= i; j--)           // passa chaves para a direita, mesma lógica que o for de cima
    {                                   // old: for (int j = x->n; j > i; j--) --> dava erro
        x->chave[j+1] = x->chave[j];
    }

    x->chave[i] = y->chave[t-1];                // joga a chave mediana do antigo filho cheio (y) para cima (em x), na posição correta, fazendo a divisão entre filho y e filho z
    x->n++;                                     // ajusta numero de elementos de x (pai do nó cheio)
}

// parâmetro: x é o nó no qual tentaremos inserir k
void BtreeInsertNonFull(BtreeNode *x, int k)
{
    int i = x->n-1;

    if (x->isLeaf)                              // não tem filho? sim = entra
    {
        while (i >= 0 && k < x->chave[i])       // caso k seja menor que a última chave do nó, passamos a última chave para a direita
        {                                       // até achar a posição ideal de k
            x->chave[i+1] = x->chave[i];
            i--;
        }
        x->chave[i+1] = k;                      // insere k na posição correta
        x->n++;                                 // ajusta n do nó
    }
    else
    {
        while (i >= 0 && k < x->chave[i])       // loop para achar qual intervalo inserir k
        {
            i--;
        }
        i++;                                    // incrementa pq para acessar o filho pós chave[i], é sempre i+1

        if (x->filho[i]->n == 2*t-1)            // verifica se o nó filho está cheio
        {
            BtreeSplitChild(x, i);
            if (k > x->chave[i])                // achar posição ideal para inserir k
            {
                i++;
            }
        }
        BtreeInsertNonFull(x->filho[i], k);
    }
}

void BtreeInsert(Btree *T, int k)
{
    BtreeNode *r = T->root;                     // ponteiro para raiz

    if (r->n == 2*t-1)                          // verifica se o nó (raiz) está cheio
    {
        BtreeNode *s = allocateNode(t, false);  // cria novo nó
        s->filho[0] = r;                        // raiz se torna filha do novo nó

        T->root = s;                            // novo nó s é a nova raiz

        BtreeSplitChild(s, 0);
        BtreeInsertNonFull(s,k);
    }
    else
    {
        BtreeInsertNonFull(r, k);
    }
}
