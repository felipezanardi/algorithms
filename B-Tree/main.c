#include <stdio.h>
#include <stdlib.h>

#include "btree.h"

int main()
{
    //      INICIALIZAÇÃO
    Btree test;
    BtreeCreate(&test, ORDEM);

    printf("ARVORE B:\n");
    printf("- Ordem (t): %d\n", ORDEM);
    printf("- Maximo de chaves por no: %d\n", 2*ORDEM-1);
    printf("- Minimo de chaves por no (exceto raiz): %d\n\n", ORDEM-1);

    //      INSERÇÃO
    int d = 1;
    for (int i=0; i<2; i++)
    {
        if (i>0)
        {
            d*=10;
        }

        for (int j=0; j<9; j++)
        {
            BtreeInsert(&test, d*(j+1));
        }
    }
    BtreeInsert(&test, 11);

    printf("Arvore antes:\n");
    BtreePrint(&test);


    //      DELETE
    BtreeDelete(&test, 50);


    //      TESTES
    printf("Arvore depois:\n");
    BtreePrint(&test);

    BtreeNode *a = BtreeSearch(test.root, 3);

    if (a != NULL)
    {
        printf("Numero de elementos dentro do no: %d\n\n", a->n);

        printf("Elementos:\n");
        for (int i=0; i<a->n; i++)
        {
            printf("%d\n", a->chave[i]);
        }
    }
    else
    {
        printf("Elemento nao encontrado!\n");
    }

    return 0;
}
