#include <stdio.h>
#include <wchar.h>
#include <assert.h>
#include <locale.h>
#include <math.h>
#include <stdlib.h>


#define NUMERO_DE_CARTAS 14
#define VALOR_DO_REI 14

// Enumeração para representar os tipos de sequência
typedef enum
{
    NENHUMA = 0,         // uma sequência vazia deverá ficar com este valor;
    CONJUNTO = 1,        // Conjunto de uma até quatro cartas todas com o mesmo valor mas de naipes diferentes;
    SEQUENCIA = 2,       // Sequência de três ou mais cartas de valores consecutivos (não necessariamente todas do mesmo naipe);
    DUPLA_SEQUENCIA = 3, // Dupla sequência de três ou mais pares de valores consecutivos.
} TipoDeSequenciaEnum;

// Estrutura de dados para representar uma sequência de cartas
typedef struct
{
    wchar_t *cartas;                // aponta para um array de cartas
    int capacidade;                 // capacidade do array de cartas
    int tamanho;                    // tamanho atual do array de cartas (número de cartas que estão lá)
    TipoDeSequenciaEnum tipo;       // tipo de sequência
    wchar_t cartaMaior;             // carta maior da sequência
    int valorAbsolutoCartaMaior;    // valor absoluto da carta maior
    int valorDentroNaipeCartaMaior; // valor dentro do naipe da carta maior
} SequenciaDeCartas;

// inicializar sequência de cartas com capacidade inicial
void initSequenciaDeCartas(SequenciaDeCartas *sequencia, int capacidadeInicial)
{
    sequencia->cartas = (wchar_t *)calloc(capacidadeInicial, sizeof(wchar_t));  // alocar espaço para a capacidade inicial
    sequencia->capacidade = capacidadeInicial;
    sequencia->tamanho = 0;
}

// adicionar carta à sequenciaDeCartas
void adicionarCarta(SequenciaDeCartas *sequencia, wchar_t novaCarta)
{
    // se o tamanho for igual à capacidade, aumentar a capacidade
    if (sequencia->tamanho >= sequencia->capacidade)
    {
        sequencia->capacidade *= 2; // duplicar a capacidade para evitar ter de realocar muitas vezes
        sequencia->cartas = (wchar_t *)realloc(sequencia->cartas, sequencia->capacidade * sizeof(wchar_t)); // realocar espaço para a nova capacidade
    }
    sequencia->cartas[sequencia->tamanho] = novaCarta;
    sequencia->tamanho++;
}

// estrutura de dados para representar uma lista de sequências de cartas
typedef struct
{
    SequenciaDeCartas *sequencias; // aponta para um array de sequências de cartas
    int tamanho;                    // número de sequências na lista
    int capacidade;                 // capacidade do array da lista
} ListaDeSequenciasDeCartas;

// inicializar lista de sequências de cartas com capacidade inicial
void initListaDeSequenciasDeCartas(ListaDeSequenciasDeCartas *lista, int capacidadeInicial)
{
    lista->sequencias = (SequenciaDeCartas *)calloc(capacidadeInicial, sizeof(SequenciaDeCartas)); // alocar espaço para a capacidade inicial
    lista->capacidade = capacidadeInicial;
    lista->tamanho = 0;
}

// adicionar sequência à listaDeSequênciasDeCartas
void adicionarSequencia(ListaDeSequenciasDeCartas *lista, SequenciaDeCartas sequencia)
{
    // se o tamanho for igual à capacidade, aumentar a capacidade
    if (lista->tamanho >= lista->capacidade)
    {
        lista->capacidade *= 2; // duplicar a capacidade para evitar ter de realocar muitas vezes
        lista->sequencias = (SequenciaDeCartas *)realloc(lista->sequencias, lista->capacidade * sizeof(SequenciaDeCartas)); // realocar espaço para a nova capacidade
    }
    lista->sequencias[lista->tamanho] = sequencia;
    lista->tamanho++;
}

// Função para calcular o produto cartesiano e retornar uma matriz com os índices
// exemplo: calcularIndicesCartesianos({2, 3, 4}, 3, &totalCombinacoes)
// retorna uma matriz com os índices de todas as combinações possíveis
// totalCombinacoes = 2 * 3 * 4 = 24
// matrizDeIndices = [[0, 0, 0], [0, 0, 1], [0, 0, 2], [0, 0, 3], [0, 1, 0], [0, 1, 1], [0, 1, 2], [0, 1, 3], [0, 2, 0], [0, 2, 1], [0, 2, 2], [0, 2, 3], [1, 0, 0], [1, 0, 1], [1, 0, 2], [1, 0, 3], [1, 1, 0], [1, 1, 1], [1, 1, 2], [1, 1, 3], [1, 2, 0], [1, 2, 1], [1, 2, 2], [1, 2, 3]]


int **calcularIndicesCartesianos(int *tamanhosDosConjuntos, int totalDeConjuntos, int *totalDeCombinacoes)
{
    // Calcular o total de combinações
    int total = 1; // Iniciar com 1 para multiplicação
    for (int i = 0; i < totalDeConjuntos; i++)
    {
        total *= tamanhosDosConjuntos[i];
    }
    *totalDeCombinacoes = total; 

    // Alocar espaço para a matriz de índices
    // linhas = total de combinações
    int **matrizDeIndices = (int **)calloc(total, sizeof(int *));
    // colunas = total de conjuntos
    for (int i = 0; i < total; i++)
    {
        matrizDeIndices[i] = (int *)calloc(totalDeConjuntos, sizeof(int));
    }

    // para cada combinação que é uma linha da matriz
    for (int i = 0; i < total; i++)
    {
        // Vamos preencher cada coluna da linha
        int temp = i; // temp é o índice da combinação
        for (int j = 0; j < totalDeConjuntos; j++)
        {
            // Dividir temp pelo número de combinações restantes para obter o índice correto
            matrizDeIndices[i][j] = temp % tamanhosDosConjuntos[j];
            temp /= tamanhosDosConjuntos[j]; 
        }
    }

    return matrizDeIndices;
}

// unicode das cartas https://en.wikipedia.org/wiki/Playing_cards_in_Unicode
// Esta função pode ser implementada com base no valor do código Unicode da carta
// Função que determina o naipe da carta usando bitwise

// PAUS = 4,
// OUROS = 3,
// COPAS = 2,
// ESPADAS = 1

int naipeDaCarta(wchar_t carta) {
    //verifica se o valor UTF recebido é uma carta
    if (carta < 0x1F0A1 || carta > 0x1F0DE) {
        return 0; 
    }
    int valor = ( (carta & 0x000F0) >> 4 ) - 10 ; 
    // shift de quatro bits para obter o valor do penúltimo digito e subtrair 10 para termos os valores de 1 a 4.
    return valor;
}

// Função que determina o valor da carta dentro do naipe
// Usando operação && (bitwise and) com a mascara 0x0000F (0000000000001111)
// se fizermos a operação bitwise and com 0x0000F obtemos o valor dentro do naipe
// exemplo:
// 0x1F0A0 & 0x0000F = 0
// explicação:
// 0x1F0A0 = 11110000010100000
// 0x0000F = 00000000000001111
// 11110000010100000 & 00000000000001111 = 00000000000000000
// exemplo:
// 0x1F0A1 & 0x0000F = 1
// explicação:
// 0x1F0A1 = 11110000010100001
// 0x0000F = 00000000000001111
// 11110000010100001 & 00000000000001111 = 00000000000000001
// exemplo:
// 0x1F0A2 & 0x0000F = 2
// explicação:
// 0x1F0A2 = 11110000010100010
// 0x0000F = 00000000000001111
// 11110000010100010 & 00000000000001111 = 00000000000000010
// ...

int valorDentroNaipe(wchar_t carta)
{
    if (carta < 0x1F0A1 || carta > 0x1F0DE)
    {
        return -1;
    }
    int valor = carta & 0x0000F;
    return valor;
}

// valor da carta * 10 + naipe, para ficar por ordem de grandeza das cartas
// exemplo:
// AS de espadas = 1 * 10 + 1 = 11
// AS DE copas = 1 * 10 + 2 = 12
// AS de ouros = 1 * 10 + 3 = 13
// AS de paus = 1 * 10 + 4 = 14
// 2 de espadas = 2 * 10 + 1 = 21
// 2 de copas = 2 * 10 + 2 = 22
// 2 de ouros = 2 * 10 + 3 = 23
// 2 de paus = 2 * 10 + 4 = 24
int valorAbsolutoCarta(wchar_t carta)
{

    int valor = valorDentroNaipe(carta);
    int naipe = naipeDaCarta(carta);
    return valor * 10 + naipe;
}

// Verifica se é um conjunto
// Conjunto de uma até quatro cartas todas com o mesmo valor mas de naipes diferentes;
int eConjunto(SequenciaDeCartas sequencia)
{
    // verifica se tem entre 1 e 4 cartas, caso não tenha retorna 0 (NAO É CONJUNTO)
    if (sequencia.tamanho < 1 || sequencia.tamanho > 4)
    {
        return 0;
    }

    // verifica se os valores dentro dos naipes são todos iguais, caso não sejam, retorna 0 (NAO É CONJUNTO)
    int valorDentroDoNaipe = valorDentroNaipe(sequencia.cartas[0]);
    for (int i = 1; i < sequencia.tamanho; i++)
    {
        // verifica se é o mesmo valor dentro do naipe
        if (valorDentroNaipe(sequencia.cartas[i]) != valorDentroDoNaipe)
        {
            return 0;
        }
    }

    // verifica se os naipes são todos diferentes aos dos outros, caso não sejam, retorna 0 (NAO É CONJUNTO)
    // ATENÇAO: talvez seja melhor fazer esta validação no loop anterior, para não ter de percorrer a lista duas vezes
    for (int i = 0; i < sequencia.tamanho; i++)
    {
        for (int j = i + 1; j < sequencia.tamanho; j++)
        {
            if (naipeDaCarta(sequencia.cartas[i]) == naipeDaCarta(sequencia.cartas[j]))
            {
                return 0;
            }
        }
    }

    // se nenhuma regra é infrigida, retornar 1 (É CONJUNTO)
    return 1;
}

// Verifica se é uma sequência
// Sequência de três ou mais cartas de valores consecutivos (não necessariamente todas do mesmo naipe);
int eSequencia(SequenciaDeCartas sequencia)
{
    // verifica se tem pelo menos 3 cartas, caso não tenha, retorna 0 (NAO É SEQUENCIA)
    if (sequencia.tamanho < 3)
    {
        return 0;
    }

    // verifica se os valores são consecutivos, caso não sejam, retorna 0 (NAO É SEQUENCIA)
    for (int i = 0; i < sequencia.tamanho - 1; i++)
    {
        if (valorDentroNaipe(sequencia.cartas[i + 1]) - valorDentroNaipe(sequencia.cartas[i]) != 1)
        {
            return 0;
        }
    }

    // se nenhuma regra é infigida, retornar 1 (É SEQUENCIA)
    return 1;
}

// Verifica se é uma dupla sequência
// Dupla sequência de três ou mais pares de valores consecutivos.
int eDuplaSequencia(SequenciaDeCartas sequencia)
{
    // verifica se tem pelo menos 6 cartas, caso não tenha, retorna 0 (NAO É DUPLA SEQUENCIA)
    if (sequencia.tamanho < 6)
    {
        return 0;
    }

    // verifica se as cartas estão aos pares, caso não estejam, retorna 0 (NAO É DUPLA SEQUENCIA)
    for (int i = 0; i < sequencia.tamanho - 1; i += 2)
    {
        // verifica se par é igual
        if (valorDentroNaipe(sequencia.cartas[i]) != valorDentroNaipe(sequencia.cartas[i + 1]))
        {
            return 0;
        }
    }

    // verifica se os valores são consecutivos, caso não sejam, retorna 0 (NAO É DUPLA SEQUENCIA)
    // ATENÇAO: talvez seja melhor fazer esta validação no loop anterior, para não ter de percorrer a lista duas vezes
    for (int i = 0; i < sequencia.tamanho - 2; i += 2)
    {
        if (valorDentroNaipe(sequencia.cartas[i + 2]) - valorDentroNaipe(sequencia.cartas[i]) != 1)
        {
            return 0;
        }
    }

    return 1;
}

// Calcula tipo de sequência
TipoDeSequenciaEnum calculaTipoDeSequencia(SequenciaDeCartas sequencia)
{
    // ATENCAO: fizemos assim para dividir melhor o trabalho.
    // Podiamos ter feito tudo numa só função, mas assim é mais fácil de perceber e fazer debug
    // se fosse tudo numa função talvez desse para ser mais eficiente e evitar percorrer a lista várias vezes
    if (eConjunto(sequencia))
    {
        return CONJUNTO;
    }
    else if (eSequencia(sequencia))
    {
        return SEQUENCIA;
    }
    else if (eDuplaSequencia(sequencia))
    {
        return DUPLA_SEQUENCIA;
    }
    else
    {
        return NENHUMA;
    }
}

// ordenar lista de sequências
// ATENCAO : Poderiamos utilizar o qsort 
void ordenarSequenciasPelaCartaMaior(int quantidadeDeSequencias, SequenciaDeCartas *sequencias)
{
    for (int i = 0; i < quantidadeDeSequencias - 1; i++)
    {
        for (int j = 0; j < quantidadeDeSequencias - i - 1; j++)
        {
            if (sequencias[j].valorAbsolutoCartaMaior > sequencias[j + 1].valorAbsolutoCartaMaior)
            {
                SequenciaDeCartas temp = sequencias[j];
                sequencias[j] = sequencias[j + 1];
                sequencias[j + 1] = temp;
            }
        }
    }
}

    // ordenar as cartas pelo valor absoluto
    // ATENCAO que isto é uma bubble sort, não é o mais eficiente, precisava de ser melhorado
void ordenarPeloValorAbsoluto(SequenciaDeCartas *sequencia)
{
    for (int i = 0; i < sequencia->tamanho - 1; i++)
    {
        for (int j = 0; j < sequencia->tamanho - i - 1; j++)
        {
            if (valorAbsolutoCarta(sequencia->cartas[j]) > valorAbsolutoCarta(sequencia->cartas[j + 1]))
            {
                wchar_t temp = sequencia->cartas[j];
                sequencia->cartas[j] = sequencia->cartas[j + 1];
                sequencia->cartas[j + 1] = temp;
            }
        }
    }
}
    // ordenar as cartas pelo valor dentro do naipe
    // ATENCAO que isto é uma bubble sort, não é o mais eficiente, precisava de ser melhorado
void ordenarPeloValorDentroNaipe(SequenciaDeCartas *sequencia)
{
    
    for (int i = 0; i < sequencia->tamanho - 1; i++)
    {
        for (int j = 0; j < sequencia->tamanho - i - 1; j++)
        {
            if (valorDentroNaipe(sequencia->cartas[j]) > valorDentroNaipe(sequencia->cartas[j + 1]))
            {
                wchar_t temp = sequencia->cartas[j];
                sequencia->cartas[j] = sequencia->cartas[j + 1];
                sequencia->cartas[j + 1] = temp;
            }
        }
    }
}

// Implementação da função avaliarSequencia
void avaliarSequencia(SequenciaDeCartas *sequencia)
{
   
    ordenarPeloValorAbsoluto(sequencia); // ordenar  as cartas
    sequencia->tipo = calculaTipoDeSequencia(*sequencia); 
    sequencia->cartaMaior = sequencia->cartas[sequencia->tamanho - 1];
    sequencia->valorAbsolutoCartaMaior = valorAbsolutoCarta(sequencia->cartaMaior);
    sequencia->valorDentroNaipeCartaMaior = valorDentroNaipe(sequencia->cartaMaior);
}

// Função que converte uma string de cartas Unicode numa sequência
SequenciaDeCartas stringParaSequenciaCartas(const wchar_t *stringCartas)
{

    // contar cartas
    int tamanho = 0;
    while (stringCartas[tamanho] != L'\n')
    {
        tamanho++;
    }

    SequenciaDeCartas sequencia;
    initSequenciaDeCartas(&sequencia, tamanho);

    int i = 0;

    // insere as cartas no array pela ordem do input
    while (stringCartas[i] != L'\n')
    {
        if (stringCartas[i] >= 0x1F0A1 && stringCartas[i] <= 0x1F0DE)
        {
            adicionarCarta(&sequencia, stringCartas[i]);
        }
        i++;
    }

    // ordena as cartas
    ordenarPeloValorAbsoluto(&sequencia);

    // avalia a sequência
    avaliarSequencia(&sequencia);

    return sequencia;
}

// verificar se são do mesmo tipo
int saoDoMesmoTipo(int quantidadeDeSequencias, SequenciaDeCartas sequencias[])
{
    // verifica se as sequencias são do mesmo tipo
    for (int i = 0; i < quantidadeDeSequencias - 1; i++)
    {
        if (sequencias[i].tipo != sequencias[i + 1].tipo || sequencias[i].tamanho != sequencias[i + 1].tamanho)
        {
            return 0;
        }
    }
    return 1;
}

int guiaoAMooshak(FILE *fp)
{ // fp é um apontador para ficheiro (que pode ser um ficheiro ou stdin dependendo do que for passado)
  // usado para ler o input de um ficheiro

  // ler a quantidade de linhas que vamos ter
    int linhasInput;
    assert(fwscanf(fp, L"%d\n", &linhasInput) == 1);

    wchar_t linha[1000];

    if (linhasInput < 1)
    {
        wprintf(L"ERRO: O número de linhas é inválido\n");
    }

    // para cada linha lida
    for (int i = 0; i < linhasInput; i++)
    {
        if (fgetws(linha, 1000, fp) == NULL)
        {
            break;
        }

        SequenciaDeCartas sequencia = stringParaSequenciaCartas(linha);

        switch (sequencia.tipo) // Impressão do output do Guião A
        {
        case NENHUMA:
            wprintf(L"Nada!");
            break;
        case CONJUNTO:
            wprintf(L"conjunto com %d cartas onde a carta mais alta é %lc", sequencia.tamanho, sequencia.cartaMaior);
            break;
        case SEQUENCIA:
            wprintf(L"sequência com %d cartas onde a carta mais alta é %lc", sequencia.tamanho, sequencia.cartaMaior);
            break;
        case DUPLA_SEQUENCIA:
            wprintf(L"dupla sequência com %d cartas onde a carta mais alta é %lc", sequencia.tamanho / 2, sequencia.cartaMaior);
            break;
        default:
            break;
        }
        wprintf(L"\n");
    }
    return 0;
}

// imprime sequencia de cartas separadas por espaços menos o último
void imprimeSequencia(SequenciaDeCartas sequencia)
{
    for (int i = 0; i < sequencia.tamanho; i++)
    {
        wprintf(L"%lc", sequencia.cartas[i]);
        // imprime espaço se não for a última carta
        if (i < sequencia.tamanho - 1)
        {
            wprintf(L" ");
        }
    }
    wprintf(L"\n");
}

// Impressão do output do Guião B
void imprimirRespostasGuiaoB(int numeroDoTeste, int quantidadeDeSequencias, SequenciaDeCartas listaDeSequencias[])

{
    wprintf(L"Teste %d\n", numeroDoTeste);
    int mesmoTipo = saoDoMesmoTipo(quantidadeDeSequencias, listaDeSequencias);
    if (!mesmoTipo)
    {
        wprintf(L"Combinações não iguais!\n");
        return;
    }

    // para todas as avaliações
    for (int i = 0; i < quantidadeDeSequencias; i++)
    {
        imprimeSequencia(listaDeSequencias[i]);
    }
}

int guiaoBMooshak(FILE *fp)
{
    // ler a quantidade de testes que vamos ter
    int quantidadeDeTestes;
    assert(fwscanf(fp, L"%d\n", &quantidadeDeTestes) == 1);

    // para cada teste
    for (int i = 0; i < quantidadeDeTestes; i++)
    {
        // ler a quantidade de sequências que vamos ter
        int quantidadeDeSequenciasDoTeste;
        assert(fwscanf(fp, L"%d\n", &quantidadeDeSequenciasDoTeste) == 1);

        // lista de sequências
        SequenciaDeCartas listaDeSequencias[quantidadeDeSequenciasDoTeste];

        // ler as linhas e preencher o array de avaliações
        for (int j = 0; j < quantidadeDeSequenciasDoTeste; j++)
        {
            // ler a sequência
            wchar_t linha[1000];
            assert(fgetws(linha, 1000, fp) != NULL);

            // criar sequência a partir da linha
            SequenciaDeCartas sequencia = stringParaSequenciaCartas(linha);
            // colocar a sequência já avaliada na lista
            listaDeSequencias[j] = sequencia;
        }

        // ordenar as sequências pela carta maior
        ordenarSequenciasPelaCartaMaior(quantidadeDeSequenciasDoTeste, listaDeSequencias);

        imprimirRespostasGuiaoB(i + 1, quantidadeDeSequenciasDoTeste, listaDeSequencias);
    }

    return 0;
}

// retira as cartas jogadas da mão do jogador
void removerJogada(SequenciaDeCartas *maoDoJogador, SequenciaDeCartas jogada)
{
    // para cada carta da jogada
    for (int i = 0; i < jogada.tamanho; i++)
    {
        // para cada carta da mão do jogador
        for (int j = 0; j < maoDoJogador->tamanho; j++)
        {
            // se a carta da jogada for igual a carta da mão do jogador
            if (jogada.cartas[i] == maoDoJogador->cartas[j])
            {
                // remover a carta da mão do jogador
                for (int k = j; k < maoDoJogador->tamanho - 1; k++)
                {
                    maoDoJogador->cartas[k] = maoDoJogador->cartas[k + 1];
                }
                maoDoJogador->tamanho--;
                break;
            }
        }
    }
}

// calcula os últimos números de PASSOS jogados seguidos, nas últimas jogadas
int numeroDePassos(int numeroDeJogadas, SequenciaDeCartas jogadas[])
{
    int numeroDePassos = 0;
    for (int i = 1; i <= numeroDeJogadas; i++)
    {
        if (jogadas[numeroDeJogadas - i].tamanho == 0)
        {
            numeroDePassos++;
        }
        else
        {
            return numeroDePassos;
        }
    }
    return numeroDePassos;
}

// calcula o número de reis da última jogada
int calculaReisDaUltimaJogada(SequenciaDeCartas ultimaJogada)
{
    // se não for conjunto não é uma excessão de uma jogada apenas de reis
    if (ultimaJogada.tipo != CONJUNTO)
    {
        return 0;
    }

    // se a primeira carta não for rei retornar 0
    if (valorDentroNaipe(ultimaJogada.cartas[0]) != VALOR_DO_REI)
    {
        return 0;
    }
    // como é conjunto, o número de reis é igual ao tamanho 
    return ultimaJogada.tamanho;
}

// retirar da mão do jogador as cartas que foram jogadas até agora
void removerTodasAsJogadasAnteriores(SequenciaDeCartas *maoDoJogador, int numeroDeJogadas, SequenciaDeCartas jogadas[])
{
    for (int i = 0; i < numeroDeJogadas; i++)
    {
        // assume-se que todas as jogadas anteriores ja foram validadas
        // os testes o dirão se é verdade :)
        removerJogada(maoDoJogador, jogadas[i]);
    }
}

// remover
void removerJogadasAnterioresePredentidas(SequenciaDeCartas *maoDoJogador, int numeroDeJogadas, SequenciaDeCartas jogadas[], SequenciaDeCartas sequenciaDaJogadaPretendida)
{

    removerTodasAsJogadasAnteriores(maoDoJogador, numeroDeJogadas, jogadas);

    if (sequenciaDaJogadaPretendida.tipo == NENHUMA)
    {
        // se a sequência pretendida for nenhuma, então não há nada a fazer
        return;
    }

    if (numeroDeJogadas == 0)
    {
        // se formos o primeiro a jogar podemos jogar qualquer coisa
        removerJogada(maoDoJogador, sequenciaDaJogadaPretendida);
        return;
    }

    int numeroDePassosSeguidos = numeroDePassos(numeroDeJogadas, jogadas);
    if (numeroDePassosSeguidos >= 3)
    {
        // se o número de passos seguidos for maior ou igual a 3, podemos jogar qualquer sequência válida
        removerJogada(maoDoJogador, sequenciaDaJogadaPretendida);
        return;
    }

    // verificar se a jogada pretendida é válida em relação à última jogada
    SequenciaDeCartas ultimaJogada = jogadas[numeroDeJogadas - numeroDePassosSeguidos - 1];
    int quantidadeDeReis = calculaReisDaUltimaJogada(ultimaJogada);

    // Um rei (K) pode ser seguido de um conjunto de quatro cartas iguais;
    if (quantidadeDeReis == 1 && sequenciaDaJogadaPretendida.tipo == CONJUNTO && sequenciaDaJogadaPretendida.tamanho == 4)
    {
        removerJogada(maoDoJogador, sequenciaDaJogadaPretendida);
        return;
    }

    // Um rei (K) pode ser seguido de uma sequência de três pares;
    if (quantidadeDeReis == 1 && sequenciaDaJogadaPretendida.tipo == DUPLA_SEQUENCIA && sequenciaDaJogadaPretendida.tamanho == 6)
    {   
        removerJogada(maoDoJogador, sequenciaDaJogadaPretendida);
        return;
    }

    // Um par de reis pode ser seguido de uma sequência de quatro pares;
    if (quantidadeDeReis == 2 && sequenciaDaJogadaPretendida.tipo == DUPLA_SEQUENCIA && sequenciaDaJogadaPretendida.tamanho == 8)
    {
        removerJogada(maoDoJogador, sequenciaDaJogadaPretendida);
        return;
    }

    // Um trio de reis pode ser seguido por uma sequência de cinco pares.
    if (quantidadeDeReis == 3 && sequenciaDaJogadaPretendida.tipo == DUPLA_SEQUENCIA && sequenciaDaJogadaPretendida.tamanho == 10)
    {
        removerJogada(maoDoJogador, sequenciaDaJogadaPretendida);
        return;
    }

    if (quantidadeDeReis == 4)
    {
        return;
    }

    // A jogada pretendida é válida se for do mesmo tipo e tamanho da jogada anterior e se a carta maior tiver um maior valor absoluto
    if (sequenciaDaJogadaPretendida.tipo == ultimaJogada.tipo &&
        sequenciaDaJogadaPretendida.tamanho == ultimaJogada.tamanho &&
        sequenciaDaJogadaPretendida.valorAbsolutoCartaMaior > ultimaJogada.valorAbsolutoCartaMaior)
    {
        removerJogada(maoDoJogador, sequenciaDaJogadaPretendida);
    }
}

// imprime output Guiao C
void imprimirRespostasGuiaoC(int numeroDoTeste, SequenciaDeCartas resultado)
{
    wprintf(L"Teste %d\n", numeroDoTeste);
    imprimeSequencia(resultado);
}

int guiaoCMooshak(FILE *fp)
{

    // ler a quantidade de testes que vamos ter
    int quantidadeDeTestes;
    assert(fwscanf(fp, L"%d\n", &quantidadeDeTestes) == 1);

    // para cada teste
    for (int i = 0; i < quantidadeDeTestes; i++)
    {
        // ler o número de jogadas
        int numeroDeJogadas;
        assert(fwscanf(fp, L"%d\n", &numeroDeJogadas) == 1);

        // le linha com mão do jogador
        wchar_t linhaComMaoDoJogador[1000];
        assert(fgetws(linhaComMaoDoJogador, 1000, fp) != NULL);

        SequenciaDeCartas maoDoJogador = stringParaSequenciaCartas(linhaComMaoDoJogador);

        // ler jogadas
        SequenciaDeCartas jogadas[numeroDeJogadas];
        for (int j = 0; j < numeroDeJogadas; j++)
        {
            // ler a jogada
            wchar_t linhaDaJogada[1000];
            assert(fgetws(linhaDaJogada, 1000, fp) != NULL);
            SequenciaDeCartas sequenciaDaJogada;

            // se a linha é igual a "PASSOU"
            if (wcscmp(linhaDaJogada, L"PASSOU\n") == 0)
            {
                // sequência fica vazia
                sequenciaDaJogada.tamanho = 0;
            }
            else
            {
                // criar sequência a partir da linha
                sequenciaDaJogada = stringParaSequenciaCartas(linhaDaJogada);
            }
            // colocar a avaliação na lista
            jogadas[j] = sequenciaDaJogada;
        }

        // ler jogada pretendida
        wchar_t linhaDaJogadaPretendida[1000];
        assert(fgetws(linhaDaJogadaPretendida, 1000, fp) != NULL);
        SequenciaDeCartas sequenciaDaJogadaPretendida = stringParaSequenciaCartas(linhaDaJogadaPretendida);

        removerJogadasAnterioresePredentidas(&maoDoJogador, numeroDeJogadas, jogadas, sequenciaDaJogadaPretendida);

        imprimirRespostasGuiaoC(i + 1, maoDoJogador);
    }

    return 0;
}

void adicionaCombinacoesDeTamanhoPretendido(ListaDeSequenciasDeCartas *conjuntosPossiveis, SequenciaDeCartas sequenciaDeCartasIguas, int tamanhoPretendido)
{
    // se o tamanho da sequência for igual ao tamanho pretendido, adiciona a sequência
    if (sequenciaDeCartasIguas.tamanho == tamanhoPretendido)
    {
        adicionarSequencia(conjuntosPossiveis, sequenciaDeCartasIguas);
        return;
    }

    // se o tamanho da sequência for maior que o tamanho pretendido, divide a sequência em sequências de tamanho pretendido
    if (sequenciaDeCartasIguas.tamanho > tamanhoPretendido)
    {
        for (int i = 0; i < sequenciaDeCartasIguas.tamanho - tamanhoPretendido + 1; i++)
        {
            SequenciaDeCartas sequenciaDeCartasIguaisNova;
            initSequenciaDeCartas(&sequenciaDeCartasIguaisNova, tamanhoPretendido);

            for (int j = i; j < i + tamanhoPretendido; j++)
            {
                adicionarCarta(&sequenciaDeCartasIguaisNova, sequenciaDeCartasIguas.cartas[j]);
            }
            adicionarSequencia(conjuntosPossiveis, sequenciaDeCartasIguaisNova);
        }
    }
}

void combinaRecursivo(SequenciaDeCartas sequenciaACombinar, SequenciaDeCartas *sequeciaCombinada, int inicio, int fim, int indice, int r, ListaDeSequenciasDeCartas *acumulador)
{
    // Quando a combinação estiver completa, adiciona ao acumulador
    if (indice == r)
    {
        SequenciaDeCartas novaSequencia;
        initSequenciaDeCartas(&novaSequencia, r);
        for (int i = 0; i < r; i++)
        {
            adicionarCarta(&novaSequencia, sequeciaCombinada->cartas[i]);
        }
        ordenarPeloValorAbsoluto(&novaSequencia);
        avaliarSequencia(&novaSequencia);
        adicionarSequencia(acumulador, novaSequencia);
        return;
    }
    // subsitui indice por todos os elementos possiveis para formar combinações, começando em 'inicio'
    for (int i = inicio; i <= fim && fim - i + 1 >= r - indice; i++)
    {
        sequeciaCombinada->cartas[indice] = sequenciaACombinar.cartas[i];
        combinaRecursivo(sequenciaACombinar, sequeciaCombinada, i + 1, fim, indice + 1, r, acumulador);
    }
}

void preencheConjuntosPossiveisComCombinacoes(ListaDeSequenciasDeCartas *conjuntosPossiveis, SequenciaDeCartas conjuntoDaMao, int tamanhoDaCombinacao, int valorMinimo)
{

    ListaDeSequenciasDeCartas acumulador = {0};
    initListaDeSequenciasDeCartas(&acumulador, 10);
    SequenciaDeCartas sequeciaCombinada = {0};
    initSequenciaDeCartas(&sequeciaCombinada, 4);
    combinaRecursivo(conjuntoDaMao, &sequeciaCombinada, 0, conjuntoDaMao.tamanho - 1, 0, tamanhoDaCombinacao, &acumulador);

    for (int i = 0; i < acumulador.tamanho; i++)
    {

        if (acumulador.sequencias[i].valorAbsolutoCartaMaior > valorMinimo)
        {
            adicionarSequencia(conjuntosPossiveis, acumulador.sequencias[i]);
        }
    }
}

// função que alinha a mão do jogador por conjuntos de cartas iguais e devolve um array de sequencias de cartas
// exemplo 
// A A A 3 3  4 4 4  5 5 5 5 6 6 6 6 J Q Q Q 
// Fica alinhado assim
// 0 -> A A A
// 1 -> 
// 2 -> 3 3
// 3 -> 4 4 4
// 4 -> 5 5 5 5
// 5 -> 6 6 6 6
// 6 -> 
// 7 ->
// 8 ->
// 9 ->
// 10 -> 
// 11 -> J
// 12 -> Q Q Q
// 13 -> 
// Este alinhamento é util para depois fazer combinações de conjuntos ou sequências
SequenciaDeCartas*  alinhaCartasPorConjuntos(SequenciaDeCartas maoDoJogador)
{
    SequenciaDeCartas* cartasAlinhadas = malloc(NUMERO_DE_CARTAS * sizeof(SequenciaDeCartas));
     for (int i = 0; i < NUMERO_DE_CARTAS; i++)
    {
        initSequenciaDeCartas(&cartasAlinhadas[i], 2); // no maximo são 4, vamos colocar duas se passar de duas a capaciadade é aumentada para o dobro
    }

    for (int i = 0; i < maoDoJogador.tamanho; i++)
    {
        int indice = valorDentroNaipe(maoDoJogador.cartas[i]) - 1;

        adicionarCarta(&cartasAlinhadas[indice], maoDoJogador.cartas[i]);
    }
    for (int i = 0; i < NUMERO_DE_CARTAS; i++)
    {
        avaliarSequencia(&cartasAlinhadas[i]);
    }

    return cartasAlinhadas;
}

void adicionarTodosOsConjuntosPossiveis( ListaDeSequenciasDeCartas *listaDeSequencias, SequenciaDeCartas maoDoJogador, int tamanhoDaJogada, int valorMinimoDaCartaMaior)
{

    SequenciaDeCartas* cartasAlinhadas = alinhaCartasPorConjuntos(maoDoJogador);

    for (int i = 0; i < NUMERO_DE_CARTAS; i++)
    {
        ordenarPeloValorAbsoluto(&cartasAlinhadas[i]);
        avaliarSequencia(&cartasAlinhadas[i]);
        if (cartasAlinhadas[i].tamanho >= tamanhoDaJogada)
        {
            preencheConjuntosPossiveisComCombinacoes(listaDeSequencias, cartasAlinhadas[i], tamanhoDaJogada, valorMinimoDaCartaMaior);
        }
    }
}

void preencheSequenciasPossiveisComProdutoCartesiano(ListaDeSequenciasDeCartas *sequenciasPossiveis, ListaDeSequenciasDeCartas sequenciaDeTamanhoPretendido,  int tamanhoDasSequenciasASerCriadas, int valorMinimoDaCartaMaior)
{
    // se sequenciaDeTamanhoPretendido tem uma com tamanho zero é porque tem uma falha e não dá para fazer sequências
    for (int i = 0; i < sequenciaDeTamanhoPretendido.tamanho; i++)
    {
        if (sequenciaDeTamanhoPretendido.sequencias[i].tamanho == 0)
        {
            return;
        }
    }

    // calcular os indices do produto cartesiano com a função que fizemos
    int n = sequenciaDeTamanhoPretendido.tamanho;
    int tamanhos[n];
    for (int i = 0; i < n; i++)
    {
        tamanhos[i] = sequenciaDeTamanhoPretendido.sequencias[i].tamanho;
    }
    int totalDeCombinacoes;
    int **cartesianIndices = calcularIndicesCartesianos(tamanhos, n, &totalDeCombinacoes);

    // pegar nos indices cartesianos e construir as sequências
    for (int i = 0; i < totalDeCombinacoes; i++)
    {
        SequenciaDeCartas novaSequencia = {0};
        initSequenciaDeCartas(&novaSequencia, tamanhoDasSequenciasASerCriadas);
        for (int j = 0; j < n; j++)
        {
            adicionarCarta(&novaSequencia, sequenciaDeTamanhoPretendido.sequencias[j].cartas[cartesianIndices[i][j]]);
        }
        avaliarSequencia(&novaSequencia);
        if ( novaSequencia.valorAbsolutoCartaMaior > valorMinimoDaCartaMaior)
        {
            adicionarSequencia(sequenciasPossiveis, novaSequencia);
        }
    }
}

void preencheSequenciasDuplasPossiveisComProdutoCartesiano(ListaDeSequenciasDeCartas *sequenciasPossiveis, ListaDeSequenciasDeCartas sequenciasDeTamanhoPretendido[], int tamanhoDasSequenciasASerCriadas, int valorMinimoDaCartaMaior)
{

    // se sequenciaDeTamanhoPretendido tem uma com tamanho zero é porque tem uma falha e não dá para fazer sequências
    for (int i = 0; i < tamanhoDasSequenciasASerCriadas; i++)
    {
        if (sequenciasDeTamanhoPretendido[i].tamanho == 0)
        {
            return;
        }
    }

    // calcular os indices do produto cartesiano com a função que fizemos
    int n = tamanhoDasSequenciasASerCriadas;
    int tamanhos[n];
    for (int i = 0; i < n; i++)
    {
        tamanhos[i] = sequenciasDeTamanhoPretendido[i].tamanho;
    }
    int totalDeCombinacoes;
    int **cartesianIndices = calcularIndicesCartesianos(tamanhos, n, &totalDeCombinacoes);

    // pegar nos indices cartesianos e construir as sequências
    for (int i = 0; i < totalDeCombinacoes; i++)
    {
        SequenciaDeCartas novaSequencia = {0};
        initSequenciaDeCartas(&novaSequencia, tamanhoDasSequenciasASerCriadas * 2);
        for (int j = 0; j < n; j++)
        {
            adicionarCarta(&novaSequencia, sequenciasDeTamanhoPretendido[j].sequencias[cartesianIndices[i][j]].cartas[0]);
            adicionarCarta(&novaSequencia, sequenciasDeTamanhoPretendido[j].sequencias[cartesianIndices[i][j]].cartas[1]);
        }
        avaliarSequencia(&novaSequencia);
        if (novaSequencia.valorAbsolutoCartaMaior > valorMinimoDaCartaMaior)
        {
            adicionarSequencia(sequenciasPossiveis, novaSequencia);
        }
    }
}

int compararSequenciasDeCartasDaPrimeiraParaUltima(const void *a, const void *b)
{
    const SequenciaDeCartas *seq1 = (const SequenciaDeCartas *)a;
    const SequenciaDeCartas *seq2 = (const SequenciaDeCartas *)b;

   // Se tiverem tamahos diferentes a maior é a maior
    if (seq1->tamanho != seq2->tamanho) {
        return seq1->tamanho - seq2->tamanho;
    }

    // Se tiverem o mesmo tamanho
    int tamanhoDasSequencias = seq1->tamanho;

    for (int i = 0; i < tamanhoDasSequencias; i++)
    {
        int valorAbsolutoDaCarta1 = valorAbsolutoCarta(seq1->cartas[i]);
        int valorAbsolutoDaCarta2 = valorAbsolutoCarta(seq2->cartas[i]);
        if (valorAbsolutoDaCarta1 != valorAbsolutoDaCarta2)
        {
            return valorAbsolutoDaCarta1 - valorAbsolutoDaCarta2;
        }
    }

    // se forem todas iguais a sequencia é igual
    return 0;
}

void adicionarTodasAsSequenciasPossiveis(ListaDeSequenciasDeCartas *sequenciasPossiveis, SequenciaDeCartas maoDoJogador, int tamanhoDasSequenciasASerCriadas, int valorMinimoDaCartaMaior)
{


    // alinhar cartas por conjuntos
    SequenciaDeCartas* cartasAlinhadas = alinhaCartasPorConjuntos(maoDoJogador);


    // percorrer desde o inicio até ao fim menos o tamanho da sequência a ser criada
    for (int i = 0; i < NUMERO_DE_CARTAS - tamanhoDasSequenciasASerCriadas+1; i++)
    {
        // Criar uma lista de conjuntos consecutivos do tamanho da sequência pretendida
        ListaDeSequenciasDeCartas sequenciaDeTamanhoPretendido = {0};
        initListaDeSequenciasDeCartas(&sequenciaDeTamanhoPretendido, tamanhoDasSequenciasASerCriadas);
        for (int j = 0; j < tamanhoDasSequenciasASerCriadas; j++)
        {
            adicionarSequencia(&sequenciaDeTamanhoPretendido, cartasAlinhadas[i + j]);
        }
        // preencher as sequências possiveis
        preencheSequenciasPossiveisComProdutoCartesiano(sequenciasPossiveis, sequenciaDeTamanhoPretendido, tamanhoDasSequenciasASerCriadas, valorMinimoDaCartaMaior);
    }

}

void adicionarTodasAsSequenciasDuplasPossiveis(ListaDeSequenciasDeCartas *sequenciasDuplasPossiveis, SequenciaDeCartas maoDoJogador, int tamanho, int valorMinimoDaCartaMaior)
{

    int tamanhoDUPLADasSequenciasASerCriadas = tamanho / 2;

    SequenciaDeCartas* cartasAlinhadas = alinhaCartasPorConjuntos(maoDoJogador);

    ListaDeSequenciasDeCartas combinacaoDeDuplasAlinhadas[NUMERO_DE_CARTAS] = {0};
    for (int i = 0; i < NUMERO_DE_CARTAS; i++)
    {
        initListaDeSequenciasDeCartas(&combinacaoDeDuplasAlinhadas[i], 10);
    }

    for (int i = 0; i < NUMERO_DE_CARTAS; i++)
    {
        preencheConjuntosPossiveisComCombinacoes(&combinacaoDeDuplasAlinhadas[i], cartasAlinhadas[i], 2, 0);
    }


    for (int i = 0; i < NUMERO_DE_CARTAS - tamanhoDUPLADasSequenciasASerCriadas +1; i++)
    {
        ListaDeSequenciasDeCartas sequenciasDeTamanhoPretendido[tamanhoDUPLADasSequenciasASerCriadas];
        for (int j = 0; j < tamanhoDUPLADasSequenciasASerCriadas; j++)
        {
            sequenciasDeTamanhoPretendido[j] = combinacaoDeDuplasAlinhadas[i + j];
        }
        preencheSequenciasDuplasPossiveisComProdutoCartesiano(sequenciasDuplasPossiveis, sequenciasDeTamanhoPretendido, tamanhoDUPLADasSequenciasASerCriadas, valorMinimoDaCartaMaior);
    }

}

ListaDeSequenciasDeCartas calculaSequenciasPossiveis(SequenciaDeCartas ultimaSequenciaJogada, SequenciaDeCartas maoDoJogador)
{
    ListaDeSequenciasDeCartas sequenciasPossiveis;
    initListaDeSequenciasDeCartas(&sequenciasPossiveis, 50);

    // Um conjunto é seguido de um conjunto do mesmo tamanho e maior valor;
    if (ultimaSequenciaJogada.tipo == CONJUNTO)
    {
        adicionarTodosOsConjuntosPossiveis(&sequenciasPossiveis, maoDoJogador, ultimaSequenciaJogada.tamanho, ultimaSequenciaJogada.valorAbsolutoCartaMaior);
    }
    // Uma sequência é seguida de uma sequência do mesmo tamanho e maior valor;
    if (ultimaSequenciaJogada.tipo == SEQUENCIA)
    {
        adicionarTodasAsSequenciasPossiveis(&sequenciasPossiveis, maoDoJogador, ultimaSequenciaJogada.tamanho, ultimaSequenciaJogada.valorAbsolutoCartaMaior);
    }
    // Uma sequência dupla é seguida de uma sequência dupla do mesmo tamanho e maior valor;
    if (ultimaSequenciaJogada.tipo == DUPLA_SEQUENCIA)
    {
        adicionarTodasAsSequenciasDuplasPossiveis(&sequenciasPossiveis, maoDoJogador, ultimaSequenciaJogada.tamanho, ultimaSequenciaJogada.valorAbsolutoCartaMaior);
    }

    int numeroDeReis = calculaReisDaUltimaJogada(ultimaSequenciaJogada);
    if (numeroDeReis == 1)
    {
        // Um rei (K) pode ser seguido de um conjunto de quatro cartas iguais;
        adicionarTodosOsConjuntosPossiveis(&sequenciasPossiveis, maoDoJogador, 4, 0);
        // Um rei (K) pode ser seguido de uma sequência de três pares;
        adicionarTodasAsSequenciasDuplasPossiveis(&sequenciasPossiveis, maoDoJogador, 6, 0);
    }
    
    if (numeroDeReis == 2)
    {
        // Um par de reis pode ser seguido de uma sequência de quatro pares;
        adicionarTodasAsSequenciasDuplasPossiveis(&sequenciasPossiveis, maoDoJogador, 8, 0);
      
    }

    if (numeroDeReis == 3)
    {
        // Um trio de reis pode ser seguido por uma sequência de cinco pares.
        adicionarTodasAsSequenciasDuplasPossiveis(&sequenciasPossiveis, maoDoJogador, 10, 0);
    }

    if (ultimaSequenciaJogada.tipo == NENHUMA)
    {
        // CALCULAR TODAS E JUNTAR ?
        return sequenciasPossiveis;
    }

    // ordenar conforme os outputs pedidos
    qsort(sequenciasPossiveis.sequencias, sequenciasPossiveis.tamanho, sizeof(SequenciaDeCartas), compararSequenciasDeCartasDaPrimeiraParaUltima);
    
    return sequenciasPossiveis;
}

int guiaoDMooshak(FILE *fp)
{
    // ler a quantidade de testes que vamos ter
    int quantidadeDeTestes;
    assert(fwscanf(fp, L"%d\n", &quantidadeDeTestes) == 1);

    // para cada teste
    for (int i = 0; i < quantidadeDeTestes; i++)
    {

        // ler última sequência jogada
        wchar_t linhaDaUltimaSequencia[1000];
        assert(fgetws(linhaDaUltimaSequencia, 1000, fp) != NULL);
        SequenciaDeCartas ultimaSequenciaJogada = stringParaSequenciaCartas(linhaDaUltimaSequencia);

        // ler mão do jogador
        wchar_t linhaComMaoDoJogador[1000];
        assert(fgetws(linhaComMaoDoJogador, 1000, fp) != NULL);
        SequenciaDeCartas maoDoJogador = stringParaSequenciaCartas(linhaComMaoDoJogador);

        ListaDeSequenciasDeCartas sequenciasPossiveis;

        sequenciasPossiveis = calculaSequenciasPossiveis(ultimaSequenciaJogada, maoDoJogador);

        wprintf(L"Teste %d\n", i + 1);
        // para cada sequência possivel
        if (sequenciasPossiveis.tamanho == 0)
        {
            wprintf(L"PASSO\n");

        }
        else
          {
        for (int j = 0; j < sequenciasPossiveis.tamanho; j++)
        {
            imprimeSequencia(sequenciasPossiveis.sequencias[j]);
        }
      }
    }

    return 0;
}

// Compilar

// main
// argc - número de argumentos
// argv - argumentos
// argv[0] - nome do programa (por conveção mesmo que não seja passado o nome do programa é colocado aqui pelo sistema operativo)
// argv[1] - argumento 1 (nome do ficheiro de input) quando fazemos debug no vscode, pasamos input.txt como argumento para o programa
int main(int argc, char *argv[])
{

    // se não forem passados argumentos vai ler do stdin
    FILE *origemDoInput = stdin;

    if (argc > 1)
    {
        // usamos isto para poder fazer debug na nossa máquina
        // se forem passados argumentos. vai abrir do ficheiro com nome que vem como parametro (input.txt configurado no launch.json do vscode)
        origemDoInput = fopen(argv[1], "r");
        setlocale(LC_ALL, ""); // so funciona com este locale na nossa máquina
    }
    else
    {
        // se não forem passados argumentos vai ler do stdin
        setlocale(LC_ALL, "C.UTF-8"); // est é o locale que funciona no mooshak
    }

    // int result = guiaoAMooshak(origemDoInput);
    int result = guiaoBMooshak(origemDoInput);
    // int result = guiaoCMooshak(origemDoInput);
    // int result = guiaoDMooshak(origemDoInput);

    // fechar o ficheiro se não foi aberto
    if (origemDoInput != stdin)
    {
        fclose(origemDoInput);
    }

    return result;
}