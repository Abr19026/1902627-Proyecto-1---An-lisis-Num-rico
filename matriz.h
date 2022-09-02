typedef double numero_m;

struct matriz {
    numero_m *mat_ptr;
    unsigned int rows,cols;
};

char init_matriz(struct matriz* matriz, unsigned int rows, unsigned int cols);

void destruir_matriz(struct matriz* matriz);

#define pos(mat,row,col) ((mat)->mat_ptr[(row)*(mat)->cols + (col)])

void imprimir_matriz(struct matriz *mat);

void intercambiar_row(struct matriz *mat, unsigned int r1, unsigned int r2);

char pivotear_montante(unsigned int nivel, struct matriz *mat);

numero_m montante(struct matriz *mat, unsigned int *num_pivoteos);

char aumentada(struct matriz* m1, struct matriz* m2, struct matriz* destino);

char matriz_identidad(struct matriz* destino, unsigned int n);

numero_m determ_inversa(struct matriz* matriz, struct matriz* destino_inv);