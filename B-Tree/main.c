#include <stdio.h>
#include <stdlib.h>

#include "btree.h"

int main()
{
    //      INICIALIZAÇÃO
    Btree test;
    BtreeCreate(&test, 3);


    //      INSERÇÃO
    BtreeInsert(&test, 3);

    BtreeInsert(&test, 10);

    BtreeInsert(&test, 2);

    BtreeInsert(&test, 4);

    BtreeInsert(&test, 6);

    BtreeInsert(&test, 30);

    BtreeInsert(&test, 40);

    BtreeInsert(&test, 50);

    BtreeInsert(&test, 60);

    //      TESTES
    BtreeNode *a = BtreeSearch(test.root, 60);

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
