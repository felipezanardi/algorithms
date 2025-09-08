#include <stdio.h>
#include <stdlib.h>

#include "dst.h"

int main()
{
    Node *arvore = NULL;

    for (int i=0; i<12; i++)
        insere(&arvore, i);

    imprimir(arvore);
    printf("\n");

    remover(&arvore, 7);
    remover(&arvore, 4);

    imprimir(arvore);
    printf("\n");

    return 0;
}
