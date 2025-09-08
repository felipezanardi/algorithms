#include <stdio.h>
#include <stdlib.h>

#include "btree.h"

int t = ORDEM; // Ordem -> Variável Global

BtreeNode *allocateNode(int t, bool leaf)
{
    BtreeNode *x = malloc(sizeof(BtreeNode));

    x->n = 0;
    x->isLeaf = leaf;

    x->chave = malloc((2*t-1) * sizeof(int));
    x->filho = malloc((2*t) * sizeof(BtreeNode*));

    return x;
}

// função deve ter complexidade O(1) (de acordo com o livro do Cormen)
void BtreeCreate(Btree *T, int t)
{
    BtreeNode *x = allocateNode(t, true);

    T->t = t;
    T->root = x;
}

// parâmetro: ponteiro para o nó x da árvore, e uma chave k que deve ser procurada
BtreeNode *BtreeSearch(BtreeNode *x, int k)
{
    int i = 0;

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
    {
        x->filho[j+1] = x->filho[j];
    }
    x->filho[i+1] = z;                          // z vira novo filho de x

    for (int j = x->n-1; j >= i; j--)           // passa chaves para a direita, mesma lógica que o for de cima
    {
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
        BtreeInsertNonFull(s, k);
    }
    else
    {
        BtreeInsertNonFull(r, k);
    }
}

// função responsável pelo caso 1
void BtreeDeleteLeaf(BtreeNode *x, int i)
{
    for (int j = i+1; j < x->n; j++)            // ajusta posição das chaves
    {
        x->chave[j-1] = x->chave[j];
    }
    x->n--;                                     // removeu uma chave, então atualiza n
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

    y->n += z->n+1;                             // atualiza o número de chaves de y (+1 porque k desceu de x)

    for (int j = i+1; j < x->n; j++)            // como k desceu para y, temos que arrumar as chaves de x movendo-as para a esquerda
        x->chave[j-1] = x->chave[j];

    for (int j = i+2; j <= x->n ; j++)          // como k desceu para y, temos que arrumar os filhos de x movendo-as para a esquerda
        x->filho[j-1] = x->filho[j];

    x->n--;                                     // atualiza n de x, porque k desceu para y
    free(z);                                    // deletamos z
}

// função resposável pelo caso 2
void BtreeDeleteNonLeaf(BtreeNode *x, int i)
{
    int k = x->chave[i];                        // salva k, caso precise usar o merge no caso 2c

    BtreeNode *y = x->filho[i];                 // filho de x que precede k (esquerda)
    BtreeNode *z = x->filho[i+1];               // filho de x que sucede k (direita)

    if (y->n >= t)                              // caso 2a: "y tem pelo menos t chaves"
    {
        while (!y->isLeaf)                      // "y não é uma folha? y tem filho?"
        {                                       // isso serve para pegar o *maior* predecessor de k!
            y = y->filho[y->n];                 // sempre o maior do nó
        }

        int predecessor = y->chave[y->n-1];     // maior predecessor de k (f)
        x->chave[i] = predecessor;              // f é colocado no lugar de k
        BtreeDeletion(y, predecessor);          // removemos f do seu antigo lugar
    }
    else if (z->n >= t)                         // caso 2b: y tem apenas t-1 chaves && z tem pelo menos t chaves
    {
        while (!z->isLeaf)                      // "z não é uma folha? z tem filho?"
        {                                       // isso serve para pegar o *menor* sucessor de k!
            z = z->filho[0];                    // sempre o menor do nó
        }

        int sucessor = z->chave[0];             // menor sucessor de k (h)
        x->chave[i] = sucessor;                 // h é colocado no lugar de k
        BtreeDeletion(z, sucessor);             // removemos h do seu antigo lugar
    }
    else                                        // caso 2c: y e z tem apenas t-1 (mínimo) de chaves
    {
        BtreeMerge(x, i);                       // unimos y e z, junto com k
        BtreeDeletion(y, k);                    // deletamos k nesse nó unificado
    }
}

void BtreeBorrowLeft(BtreeNode *x, int i)
{
    BtreeNode *y = x->filho[i];                 // da esquerda
    BtreeNode *z = x->filho[i-1];               // o mais a esquerda (i-1)

    for (int j = y->n-1; j >= 0; j--)           // passar as chaves do y para a direita, abrindo espaço para receber a chave de x
        y->chave[j+1] = y->chave[j];

    if (!y->isLeaf)                             // "y tem filho?"
    {
        for (int j = y->n; j >= 0; j--)         // se sim, passa filhos para a direita, abrindo espaço para receber o filho de z
            y->filho[j+1] = y->filho[j];
    }

    y->chave[0] = x->chave[i-1];                // passamos a chave emprestada de x para y

    if (!y->isLeaf)                             // "y tem filho?"
    {
        y->filho[0] = z->filho[z->n];           // se sim, y recebe o (último) filho de z
    }

    x->chave[i-1] = z->chave[z->n-1];           // finalmente, passamos a (última) chave de z para x

    y->n++;                                     // ajustamos os n's
    z->n--;
}

void BtreeBorrowRight(BtreeNode *x, int i)
{
    BtreeNode *y = x->filho[i];                 // da esquerda
    BtreeNode *z = x->filho[i+1];               // da direita

    y->chave[y->n] = x->chave[i];               // y recebe chave na posição i de x, será a última chave de y

    if (!y->isLeaf)                             // "y tem filho?"
    {
        y->filho[y->n+1] = z->filho[0];         // se sim, y recebe o (primeiro) filho de z
    }

    x->chave[i] = z->chave[0];                  // x recebe (na posição onde estava a antiga chave) a (primeira) chave de z

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

// função responsável pelos casos 3a e 3b
void BtreeFill(BtreeNode *x, int i)
{
    if (i > 0 && x->filho[i-1]->n >= t)         // caso 3a: "o irmão a esquerda de y tem pelo menos t-1 chaves?"
    {
        BtreeBorrowLeft(x, i);                  // se sim, pegamos emprestado dele
    }
    else if (i < x->n && x->filho[i+1]->n >= t) // caso 3a: "o irmão a direita de y tem pelo menos t-1 chaves?"
    {
        BtreeBorrowRight(x, i);                 // se sim, pegamos emprestado dele
    }
    else                                        // caso 3b: ambos irmãos possuem apenas t-1 chaves
    {
        if (i < x->n)                           // o merge usa o filho da esquerda (filho[i]) para unir com o irmão da direita (filho[i+1]),
            BtreeMerge(x, i);                   // porém, se i == x->n, o irmão da direita não existe, por isso não daria para usar merge(x,i)

        else
            BtreeMerge(x, i-1);                 // i-1 unir filho[i-1] com filho[i], já que filho[i] é o último filho
    }
}

// função responsável pela remoção geral
void BtreeDeletion(BtreeNode *x, int k)         // supõe que x tem pelo menos t chaves
{
    int i = 0;

    while (i < x->n && k > x->chave[i])         // achar posição correta
        i++;

    if (i < x->n && k == x->chave[i])
    {
        if (x->isLeaf)                          // caso 1: k está no nó x e x é uma folha
        {
            BtreeDeleteLeaf(x, i);
        }
        else                                    // caso 2: k está no nó x e x é um nó interno
        {
            BtreeDeleteNonLeaf(x, i);
        }
    }
    else                                        // caso 3: chave não está nesse nó x, e x não é uma folha
    {
        if (x->isLeaf)                          // é folha? se for, como não achamos a chave no nó, então não existe k na árvore
        {
            printf("A chave %d nao existe na arvore!\n", k);
            return;
        }

        int flag = (i == x->n);                 // flag: true (1) = k é maior que todas chaves do nó

        if (x->filho[i]->n < t)                 // o filho, em que k pode estar, tem pelo menos t chaves? pq assim, k poderia ser removido
        {                                       // não = preenche, sim = pula // não é melhor (x->filho[i]->n == t-1)?*
            BtreeFill(x, i);                    // caso 3a e 3b, manutenção preventiva
        }

        if (flag && i > x->n)                   // "k é maior que todas chaves de x?" e "merge foi executado?" pq se foi, n de x foi decrementado!
        {                                       // por isso, pra corrigir esse decremento utilizamos essa verificação
            BtreeDeletion(x->filho[i-1], k);    // i-1 pq como k é maior que todas chaves, ele avança para o filho mais a direita, porém,
        }                                       // utilizamos o merge, e por isso, agora o último filho está em i-1
        else
        {
            BtreeDeletion(x->filho[i], k);      // continua a buscando k para deletar
        }
    }
}

void BtreeDelete(Btree *T, int k)               // apenas chama função de remoção
{
    BtreeNode *r = T->root;
    BtreeDeletion(r, k);
}

// funções para imprimir árvore
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
