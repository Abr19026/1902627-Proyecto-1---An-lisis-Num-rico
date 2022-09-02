#include <stdio.h>
#include <stdlib.h>

typedef double numero_m;

struct matriz {
    numero_m *mat_ptr;
    unsigned int rows,cols;
};

/*  Inicializa una matriz con ceros dada cantidad de rows y cols
    Retorna 1 en error
*/
char init_matriz(struct matriz* matriz, unsigned int rows, unsigned int cols)
{
    matriz->mat_ptr = calloc(rows,cols * sizeof(numero_m));
    if(matriz->mat_ptr != NULL)
    {
        matriz->rows = rows;
        matriz->cols = cols;
        return 0;
    }
    return 1;
}

void destruir_matriz(struct matriz* matriz) {
    free(matriz->mat_ptr);
    matriz->mat_ptr = NULL;
    matriz->cols = 0;
    matriz->rows = 0;
}

#define pos(mat,row,col) ((mat)->mat_ptr[(row)*(mat)->cols + (col)])

void imprimir_matriz(struct matriz *mat)
{
    for(int i=0; i<mat->rows; i++)
    {
        for(int j=0; j<mat->cols; j++)
        {
            printf("%6.3lf ",pos(mat,i,j));
        }
        putchar('\n');
    }
}

/*0 <= r1,r2 < mat->rows*/
void intercambiar_row(struct matriz *mat, unsigned int r1, unsigned int r2)
{
    numero_m aux;
    for(int k=0; k<mat->cols;k++) {
        aux = pos(mat,r1,k);
        pos(mat,r1,k) = pos(mat,r2,k);
        pos(mat,r2,k) = aux;
    }
}

/*
intercambia la fila [nivel] con una inferior tal que
mat[nivel][nivel] != 0
Retorna 1 si no es posible hacerlo
Retorna 0 en Ã©xito
*/
char pivotear_montante(unsigned int nivel, struct matriz *mat)
{
    int inter;
    for(inter=nivel+1; inter<mat->rows;inter++ ) {
        if(pos(mat,inter,nivel) != 0) {
            intercambiar_row(mat,nivel,inter);
            return 0;
        }
    }
    return 1;
}

/*  Realiza mÃ©todo de montante o Bareiss en una matriz aumentada
    Solo usar si cols >= rows de la matriz
    Retorna el determinante
*/
numero_m montante(struct matriz *mat, unsigned int *num_pivoteos)
{
    unsigned int pivoteos = 0;
    numero_m piv_ant = 1;

    //Por cada row intento hacer montante
    for(unsigned int nivel = 0; nivel < mat->rows; nivel++)
    {
        //Hace intercambio de filas si piv act = 0
        if(pos(mat,nivel,nivel) == 0) {
            char lin_dep = pivotear_montante(nivel,mat);
            
            if(lin_dep) {
                *num_pivoteos = pivoteos;
                return 0;
            }
            else
                pivoteos++;
        }
        
        //Realiza mÃ©todo de montante o Bareiss en los rows restantes
        for(int row = 0; row < mat->rows; row++)
        {
            if(row == nivel) continue;
            for(int col = nivel + 1; col< mat->cols; col++)
            {
                pos(mat,row,col) = pos(mat,row,col) * pos(mat,nivel,nivel) - pos(mat,row,nivel) * pos(mat,nivel,col);
                pos(mat,row,col) /= piv_ant;
            }
        }
        //Actualiza piv_ant
        piv_ant = pos(mat,nivel,nivel);
    }

    *num_pivoteos = pivoteos;

    if(pivoteos % 2 == 1)
        piv_ant = -piv_ant;
    
    return piv_ant;
}

/* Crea una matriz aumentada en el destino
   Retorna 1 si hay error (m1 y m2 no tienen mismas rows)
   o si no hay memoria
*/
char aumentada(struct matriz* m1, struct matriz* m2, struct matriz* destino)
{
    //Guardias
    if(m1->rows != m2->rows) {
        return 1;
    }
    if(init_matriz(destino,m1->rows,m1->cols + m2->cols) == 1) {
        return 1;
    }

    //Copia matrices
    for(int i = 0;i<m1->rows; i++) {
        for(int j=0;j<m1->cols; j++) {
            pos(destino,i,j) = pos(m1,i,j);
        }
        for(int k=0; k<m2->cols; k++) {
            pos(destino,i,k + m1->cols) = pos(m2,i,k);
        }
    }
    return 0;
}

/*Crea una matriz identidad n*n, retorna 1 en error*/
char matriz_identidad(struct matriz* destino, unsigned int n) {
    if(init_matriz(destino,n,n) == 1) {
        return 1;
    }
    for(int i=0; i<n;i++) {
        pos(destino,i,i) = 1;
    }
    return 0;
}

/*  Retorna determinante de la matriz y pone su inversa en destino
    (Solo usar si la matriz de entrada es cuadrada)
    Si no hay inversa no crea la matriz (determinante 0)
*/
numero_m determ_inversa(struct matriz* matriz, struct matriz* destino_inv) {
    //Checa que sea matriz cuadrada
    if(matriz->cols != matriz->rows) {
        exit(1);
    }
    //Crea matriz identidad temporal
    struct matriz ident;
    if(matriz_identidad(&ident,matriz->rows) == 1) {
        printf("NO SE PUDO CREAR IDENTIDAD"); exit(1);
    }
    //Crea matriz aumentada con la inversa
    struct matriz aum;
    if(aumentada(matriz,&ident,&aum) == 1) {
        printf("NO SE PUDO CREAR AUMENTADA"); exit(1);
    }
    //Elimina matriz identidad temporal
    destruir_matriz(&ident);

    unsigned int pivoteos;
    //Aplica montante a aumentada (si determinante = 0 no hay inversa)
    numero_m determinante = montante(&aum,&pivoteos);
    
    //Crea inversa con adjunta y la pasa a destino (solo si determinante no es 0)
    if(determinante != 0) {
        numero_m semidet = determinante;
        if(pivoteos%2 == 1) semidet = -semidet;
        //Crea matriz de destino
        init_matriz(destino_inv,matriz->rows,matriz->cols);
        //La matriz de destino es la matriz adjunta / semidet;
        for(int i=0;i<matriz->rows;i++) {
            for(int j=0; j< matriz->cols; j++) {
                pos(destino_inv,i,j) = pos(&aum,i,j+matriz->cols) / semidet;
            }
        }
    }

    //Elimina aumentada
    destruir_matriz(&aum);

    return determinante;
}


const numero_m matriz_test[] = {
    1, 2, 1, 2,
    2, 4, 2, 5,
    1, 3, 8, 1,
    1, 2, 6, 1
};

int main()
{
    struct matriz operable;
    init_matriz(&operable,4,4);
    //Copia matriz de prueba
    for(int x=0;x<operable.rows*operable.cols;x++) {
        pos(&operable,x/operable.cols,x%operable.cols) = matriz_test[x];
    }

    struct matriz inver;
    numero_m determinante = determ_inversa(&operable,&inver);
    if(determinante != 0) {
        imprimir_matriz(&inver);
    }
    printf("\nDeterminante: %f", determinante);
    return 0;
}