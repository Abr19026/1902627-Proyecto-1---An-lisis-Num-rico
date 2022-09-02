#include<gtk/gtk.h>
#include<ctype.h>
#include"matriz.h"

void matriz_grid(GtkWidget *grid ,int rows,int cols)
{
    GtkWidget *inp_text;
    for(int x=0; x<cols; x++) {
        for(int y=0;y<rows;y++) {
            inp_text = gtk_text_new();
            gtk_grid_attach (GTK_GRID (grid), inp_text, x, y, 1, 1);
        }
    }
}

char is_positiv_int(const char* texto)
{
    int i=0;
    while(texto[i] != '\0') {
        if(!isdigit(texto[i])) {
            return 0;
        }
        i++;
    }
    return 1;
}

char is_real_num(const char* texto)
{
    char signo = 0;
    char punto = 0;
    int i=0;
    while(texto[i] != '\0') {
        if(!isdigit(texto[i])) {
            if(texto[i] == '+' || texto[i] == '-') {
                if(!signo) signo = 1;
                else return 0;
            } else if(texto[i] == ',') {
                if(!punto) punto = 1;
                else return 0;
            } else {
                return 0;
            }
        }
        i++;
    }
    return 1;
}

/*
Si entrada del text box 0<x<7 entonces crea matriz de entrada (Retorna 0) 
Si no, coloca una etiqueta de error (1 si no es numero, 2 si es fuera de rango)
*/
char interpretar_text_int(GtkEditable *text_box,int* dim_matriz) {
    const char* contenidos = gtk_editable_get_text (text_box);
    char es_num = is_positiv_int(contenidos);

    if(!es_num)
    {
        //Crear label que diga (Entrada no es válida)
        return 1;
    }

    int numero = atoi(contenidos);
    if(numero < 1 || numero > 6) {
        //Crear label que diga (Entrada debe estar entre 0 y 7 (exclusivo))
        return 2;
    }
    *dim_matriz = numero;
    return 0;
}

/*
Recibe el TextBox, lo interpreta y crea un label de error o una matriz dependiendo validez
*/
char accion_textbox(GtkEditable *textbox,GtkWidget **inp_grid,GtkWidget **label,int *GridDims) {
    int tamano_grid;
    char accion = interpretar_text_int(textbox,&tamano_grid);
    if(accion == 0) {
        //Crea grid con tamaño numero*numero
        *GridDims = tamano_grid;
        *inp_grid = gtk_grid_new();
        matriz_grid(*inp_grid,tamano_grid,tamano_grid);
    } else if (accion == 1) {
        //Crear label que diga entrada no válida 
        *label = gtk_label_new("Entrada no válida");
    } else if(accion == 2) {
        //Crear label que diga entrada debe estar entre 
        *label = gtk_label_new("Entrada debe estar entre 1 y 6");
    }
    return accion;
}


typedef struct _entrada_boton_dim_callback {
    GtkWidget *inp_grid;
    GtkEditable *textbox;
    GtkWidget *warn_label;
    GtkWidget *contenedor;
    int estado;
    int grid_dims;
    struct matriz *mat;
} globales;

globales glob_stat;

void boton_dim_callback(globales *pars)
{
    pars->estado = accion_textbox(pars->textbox,&pars->inp_grid,&pars->warn_label,&pars->grid_dims);
    switch(pars->estado) {
        case 0:
            GtkWidget* anterior = gtk_grid_get_child_at(GTK_GRID(pars->contenedor),0,4);
            
            if(anterior != NULL )   gtk_grid_remove(GTK_GRID(pars->contenedor),anterior);

            anterior = gtk_grid_get_child_at(GTK_GRID(pars->contenedor),0,3);
            
            if(anterior != NULL )   gtk_grid_remove(GTK_GRID(pars->contenedor),anterior);

            gtk_grid_attach(GTK_GRID(pars->contenedor),pars->inp_grid,0,4,1,1);
        break;
        
        case 1:case 2:
            anterior = gtk_grid_get_child_at(GTK_GRID(pars->contenedor),0,3);
            
            if(anterior != NULL )   gtk_grid_remove(GTK_GRID(pars->contenedor),anterior);

            gtk_grid_attach(GTK_GRID(pars->contenedor),pars->warn_label,0,3,1,1);
        default:
    }
}

/*Retorna 1 si matriz es valida, 0 si no*/
char checar_matriz_val(GtkWidget* matriz_inps,int dim_matriz) {
    GtkWidget *inp_act;
    const char* contenidos;
    for(int i=0; i< dim_matriz;i++) {
        for(int j=0;j<dim_matriz;j++) {
            inp_act = gtk_grid_get_child_at(GTK_GRID(matriz_inps),i,j);
            contenidos = gtk_editable_get_text(GTK_EDITABLE(inp_act));
            if(!is_real_num(contenidos))
                return 0;
        }
    }
    return 1;
}

/*Dada una grid de inps con valores validos, crea matriz*/
struct matriz* crear_matriz_desde_inps(GtkWidget* matriz_inps, int dim_matriz) {
    struct matriz nueva;
    init_matriz(&nueva,dim_matriz,dim_matriz); 
    GtkWidget *inp_act;
    const char* contenidos;
    for(int i=0; i< dim_matriz;i++) {
        for(int j=0;j<dim_matriz;j++) {
            char* extra;
            inp_act = gtk_grid_get_child_at(GTK_GRID(matriz_inps),i,j);
            contenidos = gtk_editable_get_text(GTK_EDITABLE(inp_act));
            pos(&nueva,j,i) = strtod(contenidos,&extra);
        }
    }
    struct matriz* retorno = malloc(sizeof(struct matriz));
    *retorno = nueva;
    return retorno;
}

GtkWidget* matriz_a_grid(struct matriz* mat)
{
    GtkWidget* nuevo_grid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(nuevo_grid),10);
    GtkWidget* label;
    for(int i=0;i<mat->rows;i++) {
        for(int j=0;j<mat->cols;j++) {
            gchar *texto = g_strdup_printf("%4.3f",pos(mat,i,j));
            label = gtk_label_new(texto);
            g_free(texto);
            gtk_grid_attach(GTK_GRID(nuevo_grid),label,j,i,1,1);
        }
    }
    return nuevo_grid;
}

void generar_matriz_callback(globales *pars) {
    if(pars->mat != NULL) {
        destruir_matriz(pars->mat); 
        free(pars->mat);
        pars->mat = NULL;
    }

    if(!checar_matriz_val(pars->inp_grid,pars->grid_dims)) {
        //Crea label de que no es válida
        
        pars->warn_label = gtk_label_new("La matríz no es válida");

        GtkWidget* anterior = gtk_grid_get_child_at(GTK_GRID(pars->contenedor),0,6);

        if(anterior != NULL )   gtk_grid_remove(GTK_GRID(pars->contenedor),anterior);

        gtk_grid_attach(GTK_GRID(pars->contenedor),pars->warn_label,0,6,1,1);
    } else {
        //Quita mensaje de error
        GtkWidget* anterior = gtk_grid_get_child_at(GTK_GRID(pars->contenedor),0,6);
        if(anterior != NULL )   gtk_grid_remove(GTK_GRID(pars->contenedor),anterior);
        pars->mat = crear_matriz_desde_inps(pars->inp_grid,pars->grid_dims);

        //Quita resultado anterior
        anterior = gtk_grid_get_child_at(GTK_GRID(pars->contenedor),1,0);
        if(anterior != NULL )   gtk_grid_remove(GTK_GRID(pars->contenedor),anterior);

        anterior = gtk_grid_get_child_at(GTK_GRID(pars->contenedor),1,1);
        if(anterior != NULL )   gtk_grid_remove(GTK_GRID(pars->contenedor),anterior);
        //Calcula determinante e inversa
        struct matriz inversa;
        numero_m det = determ_inversa(pars->mat,&inversa);
        
        //Muestra nuevo resultado
        char* texto = g_strdup_printf("Determinante: %f",det);
        GtkWidget* label_det = gtk_label_new(texto);
        gtk_grid_attach(GTK_GRID(pars->contenedor),label_det,1,1,3,3);
        g_free(texto);
        printf("aaaaa");
        if(det != 0) {
            GtkWidget* label_grid = matriz_a_grid(&inversa);
            gtk_grid_attach(GTK_GRID(pars->contenedor),label_grid,1,0,3,3);
            destruir_matriz(&inversa);
        }
        
    }
}

static void
activate (GtkApplication *app,
          gpointer        user_data)
{
    GtkWidget *window;
    GtkWidget *grid; //Contenedor
    GtkWidget *button;  //Botón de lectura de dimensión
    GtkWidget *inst_label;   //Instrucciones
    GtkWidget *texto;   //Entrada de datos
    GtkWidget *calcular_bot;

    window = gtk_application_window_new (app);
    gtk_window_set_title (GTK_WINDOW (window), "Calculadora de inversa");

    grid = gtk_grid_new();
    gtk_window_set_child (GTK_WINDOW (window), grid);
    gtk_grid_set_column_spacing(GTK_GRID(grid),30);
    gtk_grid_set_row_spacing(GTK_GRID(grid),20);

    inst_label = gtk_label_new("Introducir dimensión de la matriz a invertir");
    texto = gtk_entry_new();
    //gtk_entry_set_has_frame(GTK_ENTRY(texto),TRUE);
    button = gtk_button_new_with_label("Crear matríz");
    calcular_bot = gtk_button_new_with_label("Calcular inversa");
    
    glob_stat.textbox = GTK_EDITABLE(texto);
    glob_stat.contenedor = grid;

    g_signal_connect_swapped(button,"clicked",G_CALLBACK(boton_dim_callback),&glob_stat);
    g_signal_connect_swapped(calcular_bot,"clicked",G_CALLBACK(generar_matriz_callback),&glob_stat);

    gtk_grid_attach(GTK_GRID(grid),inst_label,0,0,1,1);
    gtk_grid_attach(GTK_GRID(grid),texto,0,1,1,1);
    gtk_grid_attach(GTK_GRID(grid),button,0,2,1,1);
    gtk_grid_attach(GTK_GRID(grid),calcular_bot,0,5,1,1);
    gtk_widget_show (window);
}

int lado_matriz;

int
main (int    argc,
      char **argv)
{
  GtkApplication *app;
  int status;

  app = gtk_application_new ("org.gtk.example", G_APPLICATION_FLAGS_NONE);
  g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);
  status = g_application_run (G_APPLICATION (app), argc, argv);
  g_object_unref (app);

  return status;
}