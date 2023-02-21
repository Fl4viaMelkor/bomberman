#include "raylib.h"

//TELA
#define LARGURATELA 1000
#define ALTURATELA 650
#define CORFUNDO DARKGRAY

//MAPA
#define LARGURAMAPA 900
#define ALTURAMAPA 550
#define CORMAPA GREEN

//BOMBERMAN
#define LARGURABOMBERMAN 50
#define ALTURABOMBERMAN 50
#define VELBOMBERMAN 2.5f
#define CORBOMBERMAN BROWN

//INIMIGOS
#define TAMANHOINIMIGO 25
#define VELINIMIGO 2.5f
#define CORINIMIGO RED
#define NINIMIGOS 3

//PASSOS
#define MIN_PASSOS 10
#define MAX_PASSOS 100

//INSTRUCOES
#define CORINSTRUCAO WHITE

//DIRECOES
#define NDIRECOES 4

//OUTROS
#define INVERTE -1

typedef struct {
    Vector2 pos;
    Vector2 tamanho;
} BOMBERMAN;

typedef struct{
    Vector2 pos;
    Vector2 tamanho;
} MAPA;

typedef enum {
    esq,
    dir,
    cima,
    baixo,
} DIRECAO;

typedef struct {
    Vector2 pos;
    DIRECAO dir;
    Vector2 vel;
    int passos;
    int passosMax;
    short status; //vivo ou morto
} INIMIGO;

void geraVelInimigo(INIMIGO *inimigo)
{
    if(inimigo->dir == esq)
    {
        inimigo->vel.x = VELINIMIGO * INVERTE;
        inimigo->vel.y = 0;
    }
    else if(inimigo->dir == dir)
    {
        inimigo->vel.x = VELINIMIGO;
        inimigo->vel.y = 0;
    }
    else if(inimigo->dir == cima)
    {
        inimigo->vel.x = 0;
        inimigo->vel.y = VELINIMIGO * INVERTE;
    }
    else
    {
        inimigo->vel.x = 0;
        inimigo->vel.y = VELINIMIGO;
    }
}

void geraInimigos(INIMIGO listaInimigos[], int ninimigos, MAPA mapa)
{
    int l, direcao_al;
    for(l = 0; l < ninimigos; l++)
    {
        direcao_al = GetRandomValue(0, NDIRECOES-1);
        listaInimigos[l].dir = direcao_al;
        geraVelInimigo(&listaInimigos[l]);
        listaInimigos[l].pos.x = GetRandomValue(TAMANHOINIMIGO + mapa.pos.x, (GetScreenWidth() - mapa.pos.x - TAMANHOINIMIGO));
        listaInimigos[l].pos.y = GetRandomValue(TAMANHOINIMIGO + mapa.pos.y, (GetScreenHeight() - mapa.pos.y - TAMANHOINIMIGO));
        listaInimigos[l].passos = 0;
        listaInimigos[l].passosMax = GetRandomValue(MIN_PASSOS, MAX_PASSOS);
    }
}

int checaColisaoBombermanInimigos(INIMIGO listaInimigos[], int ninimigos, Rectangle bombermanRet)
{
    int l = 0;
    while(l < ninimigos && !CheckCollisionCircleRec(listaInimigos[l].pos, TAMANHOINIMIGO, bombermanRet))
    {
        l++;
    }
    if(l < ninimigos)
    {
        return 1;
    }
    return 0;

}

void movimentaBomberman(BOMBERMAN *bomberman, MAPA mapa)
{
    //Andar para a direita
    if( (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) && (bomberman->pos.x < (GetScreenWidth() - mapa.pos.x - bomberman->tamanho.x)) )
    {
        bomberman->pos.x += VELBOMBERMAN;
    }

    //Andar para a esquerda
    if( (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) && (bomberman->pos.x > mapa.pos.x) )
    {
        bomberman->pos.x -= VELBOMBERMAN;
    }

    //Andar para cima
    if( (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP)) && (bomberman->pos.y > mapa.pos.y) )
    {
        bomberman->pos.y -= VELBOMBERMAN;
    }

    //Andar para baixo
    if( (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN)) && (bomberman->pos.y < (GetScreenHeight() - mapa.pos.y - bomberman->tamanho.y)) )
    {
        bomberman->pos.y += VELBOMBERMAN;
    }
}

void movimentaInimigos(INIMIGO listaInimigos[], int ninimigos, MAPA mapa)
{
    int l, c=0, direcao_al, blockDirec[NDIRECOES] = {};
    for(l = 0; l < ninimigos; l++)
    {
        //Checando colisao na borda da direita
        if( listaInimigos[l].pos.x >= (GetScreenWidth() - mapa.pos.x - TAMANHOINIMIGO) )
        {
            blockDirec[dir] = 1;
        }

        //Checando colisao na borda da esquerda
        if( listaInimigos[l].pos.x <= (TAMANHOINIMIGO + mapa.pos.x) )
        {
            blockDirec[esq] = 1;
        }

        //Checando colisao na borda superior
        if( listaInimigos[l].pos.y <= (TAMANHOINIMIGO + mapa.pos.y) )
        {
            blockDirec[cima] = 1;
        }

        //Andar para baixo
        if( listaInimigos[l].pos.y >= (GetScreenHeight() - mapa.pos.y - TAMANHOINIMIGO) )
        {
            blockDirec[baixo] = 1;
        }

        //Gera nova direcao aleatoria caso tenha colidido com uma parede ou o numero de passos permitidos tenha se esgotado
        if( blockDirec[listaInimigos[l].dir] == 1 || listaInimigos[l].passos >= listaInimigos[l].passosMax )
        {
            direcao_al = GetRandomValue(0, NDIRECOES-1);
            while(blockDirec[direcao_al] == 1)
            {
                direcao_al = GetRandomValue(0, NDIRECOES-1);
            }
            listaInimigos[l].dir = direcao_al;
            geraVelInimigo(&listaInimigos[l]);
            if(listaInimigos[l].passos >= listaInimigos[l].passosMax)
            {
                listaInimigos[l].passos=0;
                listaInimigos[l].passosMax = GetRandomValue(MIN_PASSOS, MAX_PASSOS);
            }
        }

        //Movimenta o inimigo
        listaInimigos[l].pos.x += listaInimigos[l].vel.x;
        listaInimigos[l].pos.y += listaInimigos[l].vel.y;
        listaInimigos[l].passos++;

    }
}

int main(void)
{

    // Inicialização
    //--------------------------------------------------------------------------------------

        InitWindow(LARGURATELA, ALTURATELA, "Bomberman");
        SetTargetFPS(60);

        //--------------------------------------------------------------------------------------

        //define o MAPA e seus dados
        MAPA mapa = {
            {
                ((GetScreenWidth() - LARGURAMAPA)/2),
                ((GetScreenHeight() -1 - ALTURAMAPA)/2)
            },
            { LARGURAMAPA, ALTURAMAPA+2 }
        };

        //--------------------------------------------------------------------------------------

        //define o BOMBERMAN e seus dados
        BOMBERMAN bomberman = {
            {
                (GetScreenWidth()/2) - (LARGURABOMBERMAN/2),
                (GetScreenHeight()/2) - (ALTURABOMBERMAN/2),
            },
            { LARGURABOMBERMAN, ALTURABOMBERMAN }
        };

        //define a estrutura retangulo do BOMBERMAN para testar a colisão
        Rectangle bombermanRet = { bomberman.pos.x, bomberman.pos.y, bomberman.tamanho.x, bomberman.tamanho.y };

        //--------------------------------------------------------------------------------------

        //inicializa o vetor de INIMIGOS
        int l;
        INIMIGO listaInimigos[NINIMIGOS];

        //gera os INIMIGOS
        geraInimigos(listaInimigos, NINIMIGOS, mapa);

        //--------------------------------------------------------------------------------------

    //--------------------------------------------------------------------------------------

    // Loop de Jogo (continua enquanto o usuario nao clicar "ESC" e enquanto o bomberman nao morrer para um inimigo
    //--------------------------------------------------------------------------------------

        while (!WindowShouldClose() && !checaColisaoBombermanInimigos(listaInimigos, NINIMIGOS, bombermanRet))
        {

            // Atualização da tela do jogo
            //----------------------------------------------------------------------------------

                //movimenta o Bomberman a partir do teclado
                movimentaBomberman(&bomberman, mapa);

                //Atualiza a posicao da estrutura retangulo do Bomberman (bombermanRet)
                bombermanRet.x = bomberman.pos.x;
                bombermanRet.y = bomberman.pos.y;

                //Atualiza a posicao dos inimigos
                movimentaInimigos(listaInimigos, NINIMIGOS, mapa);

            //----------------------------------------------------------------------------------

            // Desenho do jogo
            //----------------------------------------------------------------------------------

                BeginDrawing();

                    //Desenha fundo da tela (borda do mapa)
                    ClearBackground(CORFUNDO);

                    //Desenha o mapa
                    DrawRectangleV(mapa.pos, mapa.tamanho, CORMAPA);

                    //Desenha o texto da parte superior da tela
                    DrawText("Mova o quadrado com as setas do seu teclado e fuja do inimigo", 10, 10, 20, CORINSTRUCAO);

                    //Desenha o bomberman
                    DrawRectangleV(bomberman.pos, bomberman.tamanho, CORBOMBERMAN);

                    //Desenha os inimigos
                    for(l = 0; l < NINIMIGOS; l++)
                    {
                        DrawCircleV(listaInimigos[l].pos, TAMANHOINIMIGO, CORINIMIGO);
                    }

                EndDrawing();

            //----------------------------------------------------------------------------------

        }
    //--------------------------------------------------------------------------------------

    return 0;
}
