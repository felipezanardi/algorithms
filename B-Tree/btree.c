#include <stdlib.h>

#include "btree.h"

int t = ORDEM; // Ordem -> Vari�vel Global

BtreeNode *allocateNode(int t, bool leaf)
{
    BtreeNode *x = malloc(sizeof(BtreeNode));

    x->n = 0;
    x->isLeaf = leaf;

    x->chave = malloc((2*t-1) * sizeof(int));
    x->filho = malloc((2*t) * sizeof(BtreeNode));

    return x;
}

// fun��o deve ter complexidade O(1) (de acordo com o livro do Cormen)
void BtreeCreate(Btree *T, int t)
{
    BtreeNode *x = allocateNode(t, true);

    T->t = t;
    T->root = x;
}

// par�metro: ponteiro para o n� de raiz x da �rvore, e uma chave k que deve ser procurada
BtreeNode *BtreeSearch(BtreeNode *x, int k)
{
    int i=0;

    while (i < x->n && k > x->chave[i])         // achar a posi��o correta
        i++;

    if (i < x->n && k == x->chave[i])           // oq eu procuro � igual a essa chave?
        return x;                               // retorna o n� em que a chave se encontra (o endere�o)

    else if (x->isLeaf)                         // se chegarmos em uma folha e nenhuma chave foi encontrada, voltamos sem nada
        return NULL;

    else
        return BtreeSearch(x->filho[i], k);     // avan�ar ainda mais na busca
}

// par�metro: x � um n� n�o cheio, e i um indice que indica um filho cheio de x (x->filho[i])
void BtreeSplitChild(BtreeNode *x, int i)
{
    BtreeNode *y = x->filho[i];                 // y = filho cheio
    BtreeNode *z = allocateNode(t, y->isLeaf);  // z = novo n�

    z->n = t-1;

    for (int j = 0; j < t-1; j++)               // passa chaves do filho cheio para o novo n� (apenas chaves > chave mediana)
    {
        z->chave[j] = y->chave[j+t];
    }

    if (!y->isLeaf)                             // filho cheio n�o � folha? (tem filho?) sim = entra no loop
    {
        for (int j = 0; j < t; j++)             // passa os filhos do filho cheio para o novo n� z (apenas os filhos p�s a chave mediana)
        {
            z->filho[j] = y->filho[j+t];
        }
    }
    y->n = t-1;                                 // filho cheio tem seu n ajustado, ou seja, n�o � mais cheio

    for (int j = x->n; j >= i+1; j--)           // muda as posi��es dos filhos de x (jogando-os para o lado direito), abrindo espa�o para inserir z como novo filho de x
    {                                   // old: for (int j = x->n+1; j > i+1; j--) --> dava erro
        x->filho[j+1] = x->filho[j];
    }
    x->filho[i+1] = z;                          // z vira novo filho de x

    for (int j = x->n-1; j >= i; j--)           // passa chaves para a direita, mesma l�gica que o for de cima
    {                                   // old: for (int j = x->n; j > i; j--) --> dava erro
        x->chave[j+1] = x->chave[j];
    }

    x->chave[i] = y->chave[t-1];                // joga a chave mediana do antigo filho cheio (y) para cima (em x), na posi��o correta, fazendo a divis�o entre filho y e filho z
    x->n++;                                     // ajusta numero de elementos de x (pai do n� cheio)
}

// par�metro: x � o n� no qual tentaremos inserir k
void BtreeInsertNonFull(BtreeNode *x, int k)
{
    int i = x->n-1;

    if (x->isLeaf)                              // n�o tem filho? sim = entra
    {
        while (i >= 0 && k < x->chave[i])       // caso k seja menor que a �ltima chave do n�, passamos a �ltima chave para a direita
        {                                       // at� achar a posi��o ideal de k
            x->chave[i+1] = x->chave[i];
            i--;
        }
        x->chave[i+1] = k;                      // insere k na posi��o correta
        x->n++;                                 // ajusta n do n�
    }
    else
    {
        while (i >= 0 && k < x->chave[i])       // loop para achar qual intervalo inserir k
        {
            i--;
        }
        i++;                                    // incrementa pq para acessar o filho p�s chave[i], � sempre i+1

        if (x->filho[i]->n == 2*t-1)            // verifica se o n� filho est� cheio
        {
            BtreeSplitChild(x, i);
            if (k > x->chave[i])                // achar posi��o ideal para inserir k
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

    if (r->n == 2*t-1)                          // verifica se o n� (raiz) est� cheio
    {
        BtreeNode *s = allocateNode(t, false);  // cria novo n�
        s->filho[0] = r;                        // raiz se torna filha do novo n�

        T->root = s;                            // novo n� s � a nova raiz

        BtreeSplitChild(s, 0);
        BtreeInsertNonFull(s,k);
    }
    else
    {
        BtreeInsertNonFull(r, k);
    }
}
