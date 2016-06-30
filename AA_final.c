/*
UFRRJ - 
DISCENTES:DESIREE ARAUJO e FERNANDA OLIVEIRA
DISCIPLINA: LABORATÓRIO DE ED1 - 2016.1
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define TAM 10000
#define FILHOESQ 0
#define FILHODIR 1
#define TABELA 256

typedef struct NO{
    char letra;     // Caractere que armazena uma letra da string a ser comprimida
    int frequencia; // Frequencia com que o caracter se repete
    //char lado;    Lado que o no e filho, podendo ser o filho direito ou esquerdo do no pai
    struct NO* proximo;     
    struct NO* filhoesq;    // No que indica qual e o filho esquerdo do no atual
    struct NO* filhodir;    // No que indica qual e o filho direito do no atual
}NO;

typedef struct{
    NO* cabeca; // NO principal da arvore
    int tam;    //N° de bits gerados pela compressao do arquivo
}LISTA_ENCADEADA;

typedef struct{
    unsigned char bit; 
}BIT;

typedef struct{
    NO* raiz;
    BIT texto[TAM][TAM];    
}ARVORE_BINARIA;


//------------------------------------------
// Variaveis da compactação
int byte = 8;
int contador_compac = 0; // contador
unsigned char aux = 0;
int desloc;
//---------------------------------------------

//Variaveis da descompactação
int desc = 1;
int contador_descomp = 0; //cont
int resto = 0; //sobra
int contadorByte = 0; //contabyte
unsigned char vet[8] = {0};
//------------------------------------------

//Variaveis globais
int tabelaASC[TABELA] = {0};
FILE *arqBinario;
FILE *arquivo;
FILE *f; //des
//------------------------------------------


//INICIALIZA A ESTRUTURA DA ARVORE
ARVORE_BINARIA* criar(){
    ARVORE_BINARIA* arvore = (ARVORE_BINARIA*)malloc(sizeof(ARVORE_BINARIA));
    if(arvore != NULL){
       arvore->raiz = NULL;
        return arvore;
    }
    return NULL;
}

//INICIALIZA A ESTRUTURA DO NO
NO* criar_no(unsigned char caracter, int freq){
    NO* novo = (NO*)malloc(sizeof(NO));
    if (novo != NULL)
    {
        novo->letra = caracter;
        novo->frequencia = freq;
        novo->proximo = NULL;
        novo->filhoesq = NULL;
        novo->filhodir = NULL;
        //printf("no criado\n");
        return novo;
    }
    return NULL;
}

//INICIALIZA A ESTRUTURA DA LISTA
LISTA_ENCADEADA* cria_lista(){
    LISTA_ENCADEADA *lista = (LISTA_ENCADEADA *) malloc(sizeof(LISTA_ENCADEADA));
    NO* aux = criar_no('\0', 0);
  
    if(lista != NULL){
        lista->cabeca = aux;
        lista->tam = 0;
        //printf("lista criada\n");
        return lista;
    }
    return NULL;
}

//LÊ UMA POSIÇÃO NA TABELA
void ler(int posicao){ 

    tabelaASC[posicao]++;
}
//ENTRA COM O ARQUIVO A SER COMPACTADO
void entra_com_arquivo(char const nome []){   
    FILE *arq = fopen(nome, "r");

    if(arq != NULL){
        char letra;

        while(!feof(arq)){
            fscanf(arq, "%c", &letra);

            //  if chegou ao fim do arquivo break;
            if(feof(arq))
                break;
            ler(letra);
        }

        fclose(arq);
    }else
        puts("ERRO!\n");
}

//VERIFICA SE A LISTA ESTA VAZIA
int vazia_lista(LISTA_ENCADEADA *lista){
    if(lista->cabeca->proximo == NULL)
        return 1;    
    else
        return 0;
}

//VERIFICA SE A ARVORE ESTA VAZIA
int vazia_arvore(ARVORE_BINARIA* arvore){
    if(arvore->raiz == NULL)
        return 1;
    else
        return 0;
}

//A FUNÇÃO VERIFICA O ARQUIVO ATE 8 BITS, QUANDO CHEGA NO NONO BIT ELA ENTRA NO ELSE E  ESCREVE NO ARQUIVO QUE JÁ LEU BYTE DA MENSAGEM E ASSIM VAI RECUSIVAMENTE ATÉ O TERMINO DO ARQUIVO
void contagemBits(unsigned char bit, int tamanho){
    desloc = byte - tamanho;

    if(desloc >= 0){
       if(bit == '1'){
            unsigned char bit = 1;
            bit = bit << desloc;
            aux = aux|bit;
            byte--;
        }else if(bit == '0'){
            byte--;
        }
    }
    else{   
        fwrite(&aux, sizeof(unsigned char), 1, arqBinario);
        aux = 0;
        byte = 8;
        contagemBits(bit,1);
    }
}

//PERCORRE UMA MATRIZ ONDE ESTAO ARMAZENADOS OS BITS QUE REPRESENTAM UM CARACTER E ESCREVE ESSE BITS NO ARQUIVO E AO FINAL ESCREVE UM CARACTER NULO
void codCaracter(ARVORE_BINARIA *arvore){
        
    unsigned char n = 9;

    if(arqBinario != NULL)
    {   
        int i,j;

        for(i=0;i<TAM;i++)
        {
            if(arvore->texto[i][0].bit != '\0')
                fwrite(&i,sizeof(unsigned char),1,arqBinario);
            
            for(j=0; j<TAM; j++)
            {
                if(arvore->texto[i][j].bit == '\0')
                    break;

                else if(arvore->texto[i][j].bit != '\0')
                    fwrite(&arvore->texto[i][j].bit,sizeof(unsigned char),1,arqBinario);    
            }

                   
        }

        
        fwrite(&n,sizeof(unsigned char),1,arqBinario);
    }
}
//FAZ A CONTAGEM DA TAXA DE COMPRESSÃO DO ARQUIVO
int taxaCompressao(){   
    int i, k = 0;
    float taxa = 0;

    for(i = 32; i < TABELA; i++){
        k = k + tabelaASC[i];
    }
    
    k = 8 * k;
    taxa =  (1 - ((float)contador_compac/k)) * 100.0;

    printf("Taxa de compressao: %f%%\n", taxa);
    return taxa;
}
//GERA O ARQUIVO BINARIO
void gerar_arq(ARVORE_BINARIA *arvore, char const nome[], char const destino[]){   
    arqBinario = fopen(destino, "wb");
    FILE *arq = fopen(nome, "r");
    char letra;
    int i;

    if(arqBinario != NULL){
        codCaracter(arvore);

        while(!feof(arq))
        {
            i = 0;
            fscanf(arq, "%c", &letra);
            
            while(arvore->texto[(int)letra][i].bit != '\0')
            {
                contagemBits(arvore->texto[(int)letra][i].bit, 1);
                i++;
                contador_compac++;
            }
        }
    }else
        puts("ERRO.");


        if(contador_compac % 8 != 0 || contador_compac == 8)
        {   
        //VERIFICA SE A QTD DE BITS É DIVISIL POR 8, SE NÃO FOR RESTAM BITS A SER GRAVADOS NO ARQUIVO
            unsigned char n = contador_compac%8;
            unsigned char restou = (8-n);
            unsigned char nulo = 9;

            fwrite(&aux, sizeof(unsigned char), 1, arqBinario);
            fwrite(&nulo,sizeof(unsigned char),1,arqBinario);

        //ESCREVE COMO CHAR QUANTOS BITS RESTARAM

            printf("restou %d\n",restou);
            fwrite(&restou, sizeof(unsigned char),1,arqBinario);
           
        }
    
    taxaCompressao();
    fclose(arq);
}

void inserir(LISTA_ENCADEADA *lista, NO *no)
{
    if(vazia_lista(lista)){
        lista->cabeca->proximo = no;
        lista->tam++;
    }

    else
    {
        NO* aux = lista->cabeca;

        while(aux->proximo != NULL && aux->proximo->frequencia < no->frequencia)
        {
            aux = aux->proximo;
        }

        no->proximo = aux->proximo;
        aux->proximo = no;
        lista->tam++;
    }
}

//
void lista(LISTA_ENCADEADA *lista){ 
    int i;

    for(i = 0; i < TABELA; i++){
        if(tabelaASC[i] == 0)
            continue;
        else
            inserir(lista, criar_no(i, tabelaASC[i]));
    }
}

void arvore_(NO* no){
    if(no != NULL){
        arvore_(no->filhoesq);
        arvore_(no->filhodir);
        printf("caracter %c - frequencia: %d \n", no->letra, no->frequencia);
    }

}
//CHAMA A FUNÇÃI ARVORE_ PARA IMPRIMIR OS DADOS
void arvore_aux(ARVORE_BINARIA *arvore){
    arvore_(arvore->raiz);
}

void imprimir_codigo(ARVORE_BINARIA *arvore){   
    int i,j;
    
    for(i = 0; i < TAM; i++)
    { 
        for(j = 0; j < TAM; j++)
        {   
            if(arvore->texto[i][j].bit == '\0')
                continue;
            else
            printf("codigo da letra %c e: %c\n", i, arvore->texto[i][j].bit);
        }
    }
}
void imprimir(LISTA_ENCADEADA * lista){

    NO* no = lista->cabeca->proximo;

    while(no != NULL){ 

        printf("Caracter %c - %d \n",no->letra, no->frequencia);
        no = no->proximo;

    }
}

void imprimir_tabela(){

    int i;

    for(i = 0; i < TABELA; i++){

        if(tabelaASC[i] == 0)
            continue;
        
        printf("Caracter %c: aparece %d vezes\n", i, tabelaASC[i]);
    }
}
void gerarCodigo(LISTA_ENCADEADA *lista)
{
    //PEGA OS DOIS PRIMEIROS NÓS, UTILIZANDO LISTA SIMPLESMENTE ENCADEADA,  PARA FORMAR A ÁRVORE 

    NO* no = lista->cabeca->proximo;
    NO* aux = no->proximo;

    int frequenc = no->frequencia + aux->frequencia;

    NO* novo = criar_no('\0', frequenc);
    novo->filhoesq = no;
    novo->filhodir= aux;

    lista->cabeca->proximo = aux->proximo;

    no->proximo = NULL;
    aux->proximo = NULL;

    inserir(lista,novo);
}

//PERCOREE A ARVORE FORMANDO UMA TABELA COM OS CARACTERES ENCONTRADOS NO ARQUIVO, GERANDO O CODIGO COMPACTADO
void codificacao(ARVORE_BINARIA *arvore, NO* raiz, BIT vetor[], int fim){ //gerar_codigo  
    if(raiz != NULL){
        if(raiz->filhoesq == NULL && raiz->filhodir == NULL){   
            /* AO ENCONTRAR O NÓ FOLHA, PEGA O CARACTER COMO INTEIRO
            E SETA AS COLUNAS COM OS VAORES DO VETOR AUXILIAR CRIADO PARA ARMAZENAR OS BITS */

            if(fim == -1){
                BIT CHAR; 
                CHAR.bit = '\0';
                BIT cod;
                cod.bit = '1';

                arvore->texto[(int)raiz->letra][0] = cod;
                arvore->texto[(int)raiz->letra][1] = CHAR;
            }

            else    
            {   
                int i;

                for(i = 0; i <= fim; i++)
                {
                    arvore->texto[(int)raiz->letra][i] = vetor[i];
                }

                BIT CHAR;
                CHAR.bit = '\0';
                arvore->texto[(int)raiz->letra][fim + 1] = CHAR;
            }
        }

        else
        {   
        // PERCORRE A ARVORE PELA ESQUERDA E ARMAZENA O BIT ENCONTRADO NO VETOR AUXILIAR. 
            if(raiz->filhoesq != NULL){ 
                fim++;
                BIT nBit;
                nBit.bit = '0';
                vetor[fim] = nBit;
                codificacao(arvore, raiz->filhoesq, vetor, fim);
                fim--;
            }

        // PERCORRE A ARVORE PELA DIREITA E ARMAZENA O BIT ENCONTRADO NO VETOR AUXILIAR. 
            if(raiz->filhodir != NULL)
            { 
                fim++;
                BIT nBit2;
                nBit2.bit = '1';
                vetor[fim] = nBit2;
                codificacao(arvore, raiz->filhodir, vetor, fim);
                fim--;
            }
        }
    }
}
void criar_codigo(ARVORE_BINARIA *arvore)
{
    BIT vet[TAM];
    codificacao(arvore, arvore->raiz, vet, -1);
}
//-------------------------------------------------------------------
//FUNÇÕES DE DESCOMPACTAÇÃO 
ARVORE_BINARIA * criar_arvore(){

    unsigned char i; 
    unsigned char caracter;
    ARVORE_BINARIA *arvore = (ARVORE_BINARIA*) malloc (sizeof(ARVORE_BINARIA));

    NO* no = criar_no('\0',0); 
    arvore->raiz = no;
    NO* aux = arvore->raiz; 

    if(arvore != NULL){
        caracter = fgetc(arqBinario);
        i = caracter;
        caracter = fgetc(arqBinario);

        if(arqBinario != NULL)
        {
            while(caracter != 9)
            {
                if(caracter == '0'){
                    if(aux->filhoesq == NULL){
                        NO* novo = (NO*)malloc(sizeof(NO));
                        aux->filhoesq = novo;
                        aux = novo;
                        caracter = fgetc(arqBinario);
                    }else{
                        aux = aux->filhoesq;
                        caracter = fgetc(arqBinario);
                    }
                }else if(caracter == '1'){
                    if(aux->filhodir == NULL){
                        NO *novo = (NO*)malloc(sizeof(NO));
                        aux->filhodir = novo;
                        aux = novo;
                        caracter = fgetc(arqBinario);
                    }else{
                        aux = aux->filhodir;
                        caracter = fgetc(arqBinario);
                    }
                }else{
                        aux->letra = i;
                        i = caracter;
                        caracter = fgetc(arqBinario);
                        aux= arvore->raiz;
                    }
            

                if(caracter == 9)
                {
                    aux->letra = i;
                }
            }
        }  
        return arvore;          
    }
    printf("ERRO\n");
   
    return NULL;
}

//DESCOMPACTAÇÃO
void bytes(unsigned char caracter, int i, ARVORE_BINARIA *arvore){   
    if(contador_descomp <= 7)
    {
        if(caracter > 1){   
            vet[i] = (int) caracter % 2;
            caracter = caracter >> desc;
            contador_descomp++;
            bytes(caracter, contador_descomp, arvore);
        }else{   
            //printf("%d\n", caracter);
            int j; 
            contador_descomp = 0;

            vet[i] = 1;
            int k; //i

            
            for(j = 7; j >=0; j--)
            {
                fprintf(arquivo, "%d",vet[j]);
            }

                
                for(k = 0; k < 8; k++)
                {
                    vet[k] = 0;
                }
        }
    }
}
void descompactar(ARVORE_BINARIA *arvore){   
    unsigned char letra;

    // CONTA A QUANTIDADE DE BITS COMPACTADOS DO ARQUIVO

    while(!feof(arqBinario))
    {
        letra = fgetc(arqBinario);
       
        if(letra == 9){
            resto = fgetc(arqBinario);
            
        }
        else if(letra != 255 && letra != 9){
            contadorByte += 1;
        }
    }

   
    rewind(arqBinario); 

    // VERIFICA SE SOBROU ALGUM BIT
    if(((contadorByte * 8) - resto) % 8 == 0) 
    {   
        while(!feof(arqBinario))
        {   
            fseek(arqBinario, -1*(2+contadorByte), SEEK_END);      
            letra = fgetc(arqBinario);

            while(letra != 9)
            {
                bytes(letra, 0, arvore);
                letra = fgetc(arqBinario);
            }

            if(letra == 9)
                break;
        }
    }

    else{
        int control = 1;
        int i = 0;
        unsigned char vet[8];

        while(!feof(arqBinario))
        {
            fseek(arqBinario, -1*(2+contadorByte), SEEK_END);      
            letra = fgetc(arqBinario);

            while(letra != 9 && control < contadorByte)
            {
                bytes(letra, 0, arvore);
                letra = fgetc(arqBinario);
                control++;
            }

            if(control == contadorByte)
            {
                unsigned char caracter = letra;
                unsigned char teste = 128;
                unsigned char sobrou;
                int j;

                for(j = 7; j>= 0; j--)
                {
                    sobrou = (caracter & teste);

                    if(sobrou > 0)
                    {
                        vet[j] = sobrou/sobrou;
                        //printf("%d\n",vetor[j]);
                        fprintf(arquivo, "%d",vet[j]);
                    }

                    else
                    {
                        vet[j] = sobrou;
                        //printf("%d\n",vetor[j]);
                        fprintf(arquivo, "%d",vet[j]);
                    }

                    teste = teste >> 1;

                    if((resto) == j)
                    {
                        i = 1;
                        break;
                    }

                }
            }

            if(i == 1)
                break;
        }

    }

    fclose(arquivo);
}

void percorrer_arvore(ARVORE_BINARIA *arvore){

    NO* no = arvore->raiz;
    unsigned char letra;
    arquivo = fopen("binario.txt","r"); 

    if(arquivo != NULL){
        fscanf(arquivo, "%c", &letra);
        while(!feof(arquivo)){
            if(letra == '0')  
                no = no->filhoesq;
            else if(letra == '1')
                no = no->filhodir;     
            if (no->filhoesq == NULL && no->filhodir == NULL){
                fprintf(f, "%c", no->letra);
                no = arvore->raiz;
            }
        
        fscanf(arquivo, "%c", &letra);
        }
    }else
        printf("ERRO!\n");

    fclose(arquivo);

}

//RETORNA O NUMERO DE NOS DA LISTA
int numero_nos(NO* no){
    int n = 0;
    NO* aux = no->proximo;

   while(aux != NULL){
        n++;
   }
    return n;
}

//funcao de comparacao de frequencias que sera usada pela qsort
int comparar_freq(const void *ptr1, const void *ptr2){
    
    NO **ptrdados1 = (NO **)ptr1;
    NO**ptrdados2 = (NO **)ptr2;
    int freq1 = (*ptrdados1)->frequencia;
    int freq2 = (*ptrdados2)->frequencia;
    if (freq1 < freq2)
        return 1;
    else if (freq1 > freq2)
        return -1;
    else 
        return 0;
    
}

//cria vetor com elementos da lista e ordena usando qsort
void ordena(NO* no, NO **vetor){
    int i=0;
    NO* aux = no->proximo;  

    while(aux != NULL){
        vetor[i] = aux;
        i++;
    }
    
    qsort(vetor, i, sizeof(NO*), comparar_freq);

}

int main(int argc, char const* argv[]){

    if(argc == 3){

        double tempo = 0, tempo1 = 0, resul;
        printf("testando compactação...\n");
        
        tempo = (double)clock()/CLOCKS_PER_SEC;

        entra_com_arquivo(argv[1]);
        //printf("here\n");
        tempo1 = (double)clock()/CLOCKS_PER_SEC;
        resul = tempo1 - tempo;
        printf("Tempo em segundos: %f\n", resul);

        LISTA_ENCADEADA* list = cria_lista();
        ARVORE_BINARIA* arvore = criar();
       // printf("teste\n");

        lista(list);
        //  printf("teste\n");
        printf("IPRIMIR LISTA\n");
        imprimir(list);
        printf("---------------------------------------------\n");
        
        printf("TABELA\n");
        imprimir_tabela();
        printf("---------------------------------------------\n");

        while(list->tam > 1)
        {
            gerarCodigo(list);
            list->tam = list->tam - 2;
        }

        arvore->raiz = list->cabeca->proximo;
        //arvore_aux(arvore);
        printf("---------------------------------------------\n");
        criar_codigo(arvore);
        gerar_arq(arvore,argv[1],argv[2]);
        fclose(arqBinario);
    }
   else if(argc == 2)
    {
        printf("testando Descompactação... \n");

        arqBinario = fopen(argv[1], "rb");
        arquivo = fopen("binario.txt","w+r");
        f= fopen("descompactado.txt", "w"); 


        ARVORE_BINARIA *arvore = criar_arvore();
        descompactar(arvore);
        percorrer_arvore(arvore);
        remove("binario.txt");
        //imprime_arvore(arvore);
        printf("descompactado\n");
    }
    else
        puts("ERRO!");
        
    //printf("NUMERO NOS\n");
    //numero_nos(arvore->raiz);
    //comparar_freq(arvore->raiz, )
        return 0;
       
}

