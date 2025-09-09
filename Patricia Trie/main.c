#include <stdio.h>
#include <stdlib.h>

#include "patricia.h"

int main()
{
    Node *arvore;

    inicializa(&arvore);

    for (int i = 1; i < 12; i++)
        insere(&arvore, i);

    if (busca(arvore, 3) != NULL)
        printf("Chave encontrada!\n");
}
