#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Definição do tamanho máximo do histórico de jogadores
#define MAX_JOGADORES 100

typedef struct {
    char nome[50]; // Nome do jogador
    int vitorias;  // Quantidade de vitórias do jogador
} Jogador;

typedef struct {
    int ebomba;    // Indica se a célula tem bomba (1 para sim, 0 para não)
    int aberta;    // Indica se a célula está aberta (1 para sim, 0 para não)
    int bvizinha;  // Quantidade de bombas nas células vizinhas
} celula;

// Variáveis globais
celula jogo[10][10]; // Matriz do tabuleiro
int tam = 10;        // Tamanho do tabuleiro (dinâmico dependendo da dificuldade)

// Array para armazenar o histórico de jogadores
Jogador historico[MAX_JOGADORES];
int totalJogadores = 0; // Contador de jogadores no histórico

// Função para buscar ou adicionar jogador no histórico
int buscarOuAdicionarJogador(const char *nome) {
    int i;
    // Procura o jogador no histórico
    for (i = 0; i < totalJogadores; i++) {
        if (strcmp(historico[i].nome, nome) == 0) {
            return i; // Retorna o índice do jogador se encontrado
        }
    }

    // Adiciona o jogador no histórico se não encontrado
    strcpy(historico[totalJogadores].nome, nome);
    historico[totalJogadores].vitorias = 0; // Inicia com 0 vitórias
    return totalJogadores++; // Incrementa o total de jogadores e retorna o índice
}

// Inicializa o tabuleiro do jogo com valores padrão
void iniciar() {
    int l, c;
    for (l = 0; l < tam; l++) {
        for (c = 0; c < tam; c++) {
            jogo[l][c].ebomba = 0;    // Sem bomba
            jogo[l][c].bvizinha = 0; // Nenhuma bomba vizinha
            jogo[l][c].aberta = 0;   // Célula fechada
        }
    }
}

// Sorteia as posições das bombas no tabuleiro
void sorteio(int n) {
    int i;
    srand(time(NULL)); // Garante que o sorteio seja aleatório
    for (i = 1; i <= n; i++) {
        int l = rand() % tam; // Sorteia uma linha
        int c = rand() % tam; // Sorteia uma coluna
        if (jogo[l][c].ebomba == 0) { // Verifica se a célula já tem bomba
            jogo[l][c].ebomba = 1;   // Insere a bomba
        } else {
            i--; // Se já tiver bomba, refaz o sorteio
        }
    }
}

// Verifica se as coordenadas estão dentro dos limites do tabuleiro
int coordenadaT(int l, int c) {
    return (l >= 0 && l < tam && c >= 0 && c < tam);
}

// Conta o número de bombas nas células vizinhas de uma posição
int bombasv(int l, int c) {
    int qtd = 0;
    // Verifica as células vizinhas (cima, baixo, esquerda, direita)
    if (coordenadaT(l - 1, c) && jogo[l - 1][c].ebomba) qtd++;
    if (coordenadaT(l + 1, c) && jogo[l + 1][c].ebomba) qtd++;
    if (coordenadaT(l, c + 1) && jogo[l][c + 1].ebomba) qtd++;
    if (coordenadaT(l, c - 1) && jogo[l][c - 1].ebomba) qtd++;
    return qtd; // Retorna a quantidade de bombas vizinhas
}

// Calcula e armazena o número de bombas vizinhas para cada célula
void cbombas() {
    int l, c;
    for (l = 0; l < tam; l++) {
        for (c = 0; c < tam; c++) {
            jogo[l][c].bvizinha = bombasv(l, c); // Preenche o campo `bvizinha`
        }
    }
}

// Imprime o tabuleiro atual na tela
void imprimir() {
    int l, c;
    printf("\n\n\t   ");
    // Imprime os índices das colunas
    for (l = 0; l < tam; l++) {
        printf("  %d ", l);
    }
    printf("\n \t   -------------------------\n");
    for (l = 0; l < tam; l++) {
        printf("\t%d  |", l); // Imprime o índice da linha
        for (c = 0; c < tam; c++) {
            if (jogo[l][c].aberta) { // Se a célula estiver aberta
                if (jogo[l][c].ebomba) {
                    printf(" * "); // Mostra bomba
                } else {
                    printf(" %d ", jogo[l][c].bvizinha); // Mostra número de bombas vizinhas
                }
            } else {
                printf("   "); // Célula fechada
            }
            printf("|");
        }
        printf("\n");
    }
    printf("\t   -------------------------\n");
}

// Abre uma célula e, se não tiver bombas vizinhas, abre as vizinhas recursivamente
void abrirc(int l, int c) {
    if (coordenadaT(l, c) && jogo[l][c].aberta == 0) { // Verifica se a célula é válida e está fechada
        jogo[l][c].aberta = 1; // Abre a célula
        if (jogo[l][c].bvizinha == 0) { // Se não tiver bombas vizinhas
            // Abre as células adjacentes
            abrirc(l - 1, c);
            abrirc(l + 1, c);
            abrirc(l, c - 1);
            abrirc(l, c + 1);
        }
    }
}

// Verifica se o jogador ganhou (todas as células sem bombas foram abertas)
int ganhou() {
    int quantidade = 0, l, c;
    for (l = 0; l < tam; l++) {
        for (c = 0; c < tam; c++) {
            if (jogo[l][c].aberta == 0 && jogo[l][c].ebomba == 0) {
                quantidade++; // Conta células fechadas sem bombas
            }
        }
    }
    return quantidade; // Retorna a quantidade restante
}

// Exibe o histórico de jogadores e suas estatísticas
void exibirEstatisticas() {
    int i;
    printf("\n=== Estatísticas de jogadores ===\n");
    for (i = 0; i < totalJogadores; i++) {
        printf("%s: %d vitórias\n", historico[i].nome, historico[i].vitorias);
    }
    printf("=================================\n");
}

// Controla o fluxo de uma partida
void comecar(int jogadorIndex) {
    int linha, coluna;
    do {
        imprimir(); // Exibe o tabuleiro

        // Solicita coordenadas ao jogador
        do {
            printf("Escolha as coordenadas de linha e coluna: ");
            scanf("%d%d", &linha, &coluna);

            if (!coordenadaT(linha, coluna)) {
                printf("\nInválido! Por favor coloque uma coordenada correta.\n");
            }
        } while (!coordenadaT(linha, coluna));

        abrirc(linha, coluna); // Abre a célula escolhida
    } while (ganhou() != 0 && jogo[linha][coluna].ebomba == 0); // Continua até ganhar ou encontrar uma bomba

    if (jogo[linha][coluna].ebomba) {
        printf("\n\tOh não! Você perdeu :( Tente novamente\n");
    } else {
        printf("\n\tPARABÉNS! Você ganhou :D\n");
        historico[jogadorIndex].vitorias++; // Incrementa as vitórias do jogador
    }

    exibirEstatisticas(); // Mostra as estatísticas após o jogo
}

// Função principal que gerencia o jogo
int main() {
    int opcao, nivel, jogadorIndex;
    char nome[50];

    do {
        printf("\nDigite seu nome: ");
        scanf("%s", nome);

        jogadorIndex = buscarOuAdicionarJogador(nome); // Busca ou registra o jogador

        do {
            printf("\n\t\t\tCAMPO MINADO\n");
            printf("\nEscolha a dificuldade (1, 2, 3 ou 4): ");
            scanf("%d", &nivel);

            if (nivel == 1) {
                tam = 4;
                iniciar();
                sorteio(3);
            } else if (nivel == 2) {
                tam = 5;
                iniciar();
                sorteio(4);
            } else if (nivel == 3) {
                tam = 7;
                iniciar();
                sorteio(6);
            } else if (nivel == 4) {
                tam = 9;
                iniciar();
                sorteio(10);
            } else {
                printf("\nOpção inválida!\n");
            }
        } while (nivel < 1 || nivel > 4);

        cbombas(); // Calcula bombas vizinhas
        comecar(jogadorIndex); // Inicia o jogo

        printf("\nDeseja reiniciar o jogo?\n1. Sim\n2. Não\n");
        scanf("%d", &opcao);

    } while (opcao == 1);

    printf("\n\tObrigado por jogar! Até a próxima.\n");
    return 0;
}
