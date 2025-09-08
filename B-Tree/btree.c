#include <stdio.h>
#include <stdlib.h>

#include "btree.h"

int t = ORDEM; // Ordem -> Vari�vel Global

BtreeNode *allocateNode(int t, bool leaf)
{
    BtreeNode *x = malloc(sizeof(BtreeNode));

    x->n = 0;
    x->isLeaf = leaf;

    x->chave = malloc((2*t-1) * sizeof(int));
    x->filho = malloc((2*t) * sizeof(BtreeNode*));

    return x;
}

// fun��o deve ter complexidade O(1) (de acordo com o livro do Cormen)
void BtreeCreate(Btree *T, int t)
{
    BtreeNode *x = allocateNode(t, true);

    T->t = t;
    T->root = x;
}

// par�metro: ponteiro para o n� x da �rvore, e uma chave k que deve ser procurada
BtreeNode *BtreeSearch(BtreeNode *x, int k)
{
    int i = 0;

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
    {
        x->filho[j+1] = x->filho[j];
    }
    x->filho[i+1] = z;                          // z vira novo filho de x

    for (int j = x->n-1; j >= i; j--)           // passa chaves para a direita, mesma l�gica que o for de cima
    {
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
        BtreeInsertNonFull(s, k);
    }
    else
    {
        BtreeInsertNonFull(r, k);
    }
}

// fun��o respons�vel pelo caso 1
void BtreeDeleteLeaf(BtreeNode *x, int i)
{
    for (int j = i+1; j < x->n; j++)            // ajusta posi��o das chaves
    {
        x->chave[j-1] = x->chave[j];
    }
    x->n--;                                     // removeu uma chave, ent�o atualiza n
}

void BtreeMerge(BtreeNode *x, int i)
{
    BtreeNode *y = x->filho[i];                 // filho a esquerda de k
    BtreeNode *z = x->filho[i+1];               // filho a direita de k

    y->chave[t-1] = x->chave[i];                // k desce de x para o meio (t-1) do filho da esquerda (y)

    for (int j=0; j < z->n; j++)                // passa chaves de z para y
        y->chave[t+j] = z->chave[j];

    if (!y->isLeaf)                             // "y tem filhos?"
    {
        for (int j=0; j <= z->n; j++)           // passa filhos de z para y
            y->filho[t+j] = z->filho[j];
    }

    y->n += z->n+1;                             // atualiza o n�mero de chaves de y (+1 porque k desceu de x)

    for (int j = i+1; j < x->n; j++)            // como k desceu para y, temos que arrumar as chaves de x movendo-as para a esquerda
        x->chave[j-1] = x->chave[j];

    for (int j = i+2; j <= x->n ; j++)          // como k desceu para y, temos que arrumar os filhos de x movendo-as para a esquerda
        x->filho[j-1] = x->filho[j];

    x->n--;                                     // atualiza n de x, porque k desceu para y
    free(z);                                    // deletamos z
}

// fun��o respos�vel pelo caso 2
void BtreeDeleteNonLeaf(BtreeNode *x, int i)
{
    int k = x->chave[i];                        // salva k, caso precise usar o merge no caso 2c

    BtreeNode *y = x->filho[i];                 // filho de x que precede k (esquerda)
    BtreeNode *z = x->filho[i+1];               // filho de x que sucede k (direita)

    if (y->n >= t)                              // caso 2a: "y tem pelo menos t chaves"
    {
        while (!y->isLeaf)                      // "y n�o � uma folha? y tem filho?"
        {                                       // isso serve para pegar o *maior* predecessor de k!
            y = y->filho[y->n];                 // sempre o maior do n�
        }

        int predecessor = y->chave[y->n-1];     // maior predecessor de k (f)
        x->chave[i] = predecessor;              // f � colocado no lugar de k
        BtreeDeletion(y, predecessor);          // removemos f do seu antigo lugar
    }
    else if (z->n >= t)                         // caso 2b: y tem apenas t-1 chaves && z tem pelo menos t chaves
    {
        while (!z->isLeaf)                      // "z n�o � uma folha? z tem filho?"
        {                                       // isso serve para pegar o *menor* sucessor de k!
            z = z->filho[0];                    // sempre o menor do n�
        }

        int sucessor = z->chave[0];             // menor sucessor de k (h)
        x->chave[i] = sucessor;                 // h � colocado no lugar de k
        BtreeDeletion(z, sucessor);             // removemos h do seu antigo lugar
    }
    else                                        // caso 2c: y e z tem apenas t-1 (m�nimo) de chaves
    {
        BtreeMerge(x, i);                       // unimos y e z, junto com k
        BtreeDeletion(y, k);                    // deletamos k nesse n� unificado
    }
}

void BtreeBorrowLeft(BtreeNode *x, int i)
{
    BtreeNode *y = x->filho[i];                 // da esquerda
    BtreeNode *z = x->filho[i-1];               // o mais a esquerda (i-1)

    for (int j = y->n-1; j >= 0; j--)           // passar as chaves do y para a direita, abrindo espa�o para receber a chave de x
        y->chave[j+1] = y->chave[j];

    if (!y->isLeaf)                             // "y tem filho?"
    {
        for (int j = y->n; j >= 0; j--)         // se sim, passa filhos para a direita, abrindo espa�o para receber o filho de z
            y->filho[j+1] = y->filho[j];
    }

    y->chave[0] = x->chave[i-1];                // passamos a chave emprestada de x para y

    if (!y->isLeaf)                             // "y tem filho?"
    {
        y->filho[0] = z->filho[z->n];           // se sim, y recebe o (�ltimo) filho de z
    }

    x->chave[i-1] = z->chave[z->n-1];           // finalmente, passamos a (�ltima) chave de z para x

    y->n++;                                     // ajustamos os n's
    z->n--;
}

void BtreeBorrowRight(BtreeNode *x, int i)
{
    BtreeNode *y = x->filho[i];                 // da esquerda
    BtreeNode *z = x->filho[i+1];               // da direita

    y->chave[y->n] = x->chave[i];               // y recebe chave na posi��o i de x, ser� a �ltima chave de y

    if (!y->isLeaf)                             // "y tem filho?"
    {
        y->filho[y->n+1] = z->filho[0];         // se sim, y recebe o (primeiro) filho de z
    }

    x->chave[i] = z->chave[0];                  // x recebe (na posi��o onde estava a antiga chave) a (primeira) chave de z

    for (int j = 1; j < z->n; j++)              // ajusta chaves de z movendo-as para a esquerda
        z->chave[j-1] = z->chave[j];

    if (!y->isLeaf)                             // "y tem filho?"
    {
        for (int j = 1; j <= z->n; j++)         // se sim, quer dizer que z passou um filho pra y,
            z->filho[j-1] = z->filho[j];        // portanto, z ajusta seus filhos movendo-os para a esquerda
    }

    y->n++;                                     // ajustamos os n's
    z->n--;
}

// fun��o respons�vel pelos casos 3a e 3b
void BtreeFill(BtreeNode *x, int i)
{
    if (i > 0 && x->filho[i-1]->n >= t)         // caso 3a: "o irm�o a esquerda de y tem pelo menos t-1 chaves?"
    {
        BtreeBorrowLeft(x, i);                  // se sim, pegamos emprestado dele
    }
    else if (i < x->n && x->filho[i+1]->n >= t) // caso 3a: "o irm�o a direita de y tem pelo menos t-1 chaves?"
    {
        BtreeBorrowRight(x, i);                 // se sim, pegamos emprestado dele
    }
    else                                        // caso 3b: ambos irm�os possuem apenas t-1 chaves
    {
        if (i < x->n)                           // o merge usa o filho da esquerda (filho[i]) para unir com o irm�o da direita (filho[i+1]),
            BtreeMerge(x, i);                   // por�m, se i == x->n, o irm�o da direita n�o existe, por isso n�o daria para usar merge(x,i)

        else
            BtreeMerge(x, i-1);                 // i-1 unir filho[i-1] com filho[i], j� que filho[i] � o �ltimo filho
    }
}

// fun��o respons�vel pela remo��o geral
void BtreeDeletion(BtreeNode *x, int k)         // sup�e que x tem pelo menos t chaves
{
    int i = 0;

    while (i < x->n && k > x->chave[i])         // achar posi��o correta
        i++;

    if (i < x->n && k == x->chave[i])
    {
        if (x->isLeaf)                          // caso 1: k est� no n� x e x � uma folha
        {
            BtreeDeleteLeaf(x, i);
        }
        else                                    // caso 2: k est� no n� x e x � um n� interno
        {
            BtreeDeleteNonLeaf(x, i);
        }
    }
    else                                        // caso 3: chave n�o est� nesse n� x, e x n�o � uma folha
    {
        if (x->isLeaf)                          // � folha? se for, como n�o achamos a chave no n�, ent�o n�o existe k na �rvore
        {
            printf("A chave %d nao existe na arvore!\n", k);
            return;
        }

        int flag = (i == x->n);                 // flag: true (1) = k � maior que todas chaves do n�

        if (x->filho[i]->n < t)                 // o filho, em que k pode estar, tem pelo menos t chaves? pq assim, k poderia ser removido
        {                                       // n�o = preenche, sim = pula // n�o � melhor (x->filho[i]->n == t-1)?*
            BtreeFill(x, i);                    // caso 3a e 3b, manuten��o preventiva
        }

        if (flag && i > x->n)                   // "k � maior que todas chaves de x?" e "merge foi executado?" pq se foi, n de x foi decrementado!
        {                                       // por isso, pra corrigir esse decremento utilizamos essa verifica��o
            BtreeDeletion(x->filho[i-1], k);    // i-1 pq como k � maior que todas chaves, ele avan�a para o filho mais a direita, por�m,
        }                                       // utilizamos o merge, e por isso, agora o �ltimo filho est� em i-1
        else
        {
            BtreeDeletion(x->filho[i], k);      // continua a buscando k para deletar
        }
    }
}

void BtreeDelete(Btree *T, int k)               // apenas chama fun��o de remo��o
{
    BtreeNode *r = T->root;
    BtreeDeletion(r, k);
}

// fun��es para imprimir �rvore
void BtreePrintNode(BtreeNode *x, int level)
{
    if (x != NULL)
    {
        int i;

        for (int j = 0; j < level; j++)
        {
            printf("  ");
        }

        printf("Nivel %d: [", level);
        for (i = 0; i < x->n; i++)
        {
            printf("%d", x->chave[i]);
            if (i < x->n - 1) printf(", ");
        }
        printf("] (%s)\n", x->isLeaf ? "folha" : "interno");

        if (!x->isLeaf)
        {
            for (i = 0; i <= x->n; i++)
            {
                if (x->filho[i] != NULL)
                {
                    BtreePrintNode(x->filho[i], level + 1);
                }
            }
        }
    }
}

void BtreePrint(Btree *T)
{
    if (T == NULL || T->root == NULL)
    {
        printf("Arvore vazia!\n");
        return;
    }

    BtreePrintNode(T->root, 0);

    printf("\n\n");
}
