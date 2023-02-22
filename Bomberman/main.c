#include "raylib.h"

//LINHAS E COLUNAS
#define NLINHAS 21
#define NCOLUNAS 11

//TELA
#define LARGURATELA (NLINHAS*50)+100
#define ALTURATELA (NCOLUNAS*50)+100
#define CORFUNDO DARKGRAY

//MAPA
#define LARGURAMAPA NLINHAS*50
#define ALTURAMAPA NCOLUNAS*50
#define CORMAPA GREEN

//BLOCOS
#define NBLOCOS ((NLINHAS-1)/2) * ((NCOLUNAS-1)/2) + 1
#define TAMANHOBLOCOS 50
#define CORBLOCOS GRAY

//BOMBERMAN
#define LARGURABOMBERMAN 40
#define ALTURABOMBERMAN 40
#define VELBOMBERMAN 2.0f
#define CORBOMBERMAN BROWN

//INIMIGOS
#define TAMANHOINIMIGO 20
#define VELINIMIGO 2.0f
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

int checaColisaoBombermanBlocos(Rectangle blocos[], Rectangle bombermanRet, int direcao)
{
    int l = 0, blockDirec[NDIRECOES] = {};
    while(l < NBLOCOS && blockDirec[direcao] == 0)
    {
        //Checando colisao com bloco a direita do Bomberman
        if( (bombermanRet.x + bombermanRet.width) >= blocos[l].x
           && bombermanRet.x < blocos[l].x
           && ( ( bombermanRet.y < ((blocos[l].y + TAMANHOBLOCOS)-1)
                 && bombermanRet.y > (blocos[l].y+1)
                 && (bombermanRet.y + bombermanRet.height) > (blocos[l].y+1) )
               || ( ( bombermanRet.y < (blocos[l].y+1)
                 && (bombermanRet.y + bombermanRet.height) > (blocos[l].y+1)
                 && (bombermanRet.y + bombermanRet.height) < ((blocos[l].y + TAMANHOBLOCOS)-1) ) ) ) )
        {
            blockDirec[dir] = 1;
        }

        //Checando colisao com bloco a esquerda do Bomberman
        if( bombermanRet.x <= (blocos[l].x + TAMANHOBLOCOS)
           && (bombermanRet.x + bombermanRet.width) > (blocos[l].x + TAMANHOBLOCOS)
           && ( ( bombermanRet.y < ((blocos[l].y + TAMANHOBLOCOS)-1)
                 && bombermanRet.y > (blocos[l].y+1)
                 && (bombermanRet.y + bombermanRet.height) > (blocos[l].y+1) )
               || ( ( bombermanRet.y < (blocos[l].y+1)
                 && (bombermanRet.y + bombermanRet.height) > (blocos[l].y+1)
                 && (bombermanRet.y + bombermanRet.height) < ((blocos[l].y + TAMANHOBLOCOS)-1) ) ) ) )
        {
            blockDirec[esq] = 1;
        }

        //Checando colisao com bloco acima do Bomberman
        if( bombermanRet.y <= (blocos[l].y + TAMANHOBLOCOS)
           && (bombermanRet.y + bombermanRet.height) > (blocos[l].y + TAMANHOBLOCOS)
           && ( ( bombermanRet.x < ((blocos[l].x + TAMANHOBLOCOS)-1)
                 && bombermanRet.x > (blocos[l].x+1)
                 && (bombermanRet.x + bombermanRet.width) > (blocos[l].x+1) )
               || ( ( bombermanRet.x < (blocos[l].x+1)
                 && (bombermanRet.x + bombermanRet.width) > (blocos[l].x+1)
                 && (bombermanRet.x + bombermanRet.width) < ((blocos[l].x + TAMANHOBLOCOS)-1) ) ) ) )
        {
            blockDirec[cima] = 1;
        }

        //Checando colisao com bloco abaixo do Bomberman
        if( (bombermanRet.y + bombermanRet.height) >= blocos[l].y
           && bombermanRet.y < blocos[l].y
           && ( ( bombermanRet.x < ((blocos[l].x + TAMANHOBLOCOS)-1)
                 && bombermanRet.x > (blocos[l].x+1)
                 && (bombermanRet.x + bombermanRet.width) > (blocos[l].x+1) )
               || ( ( bombermanRet.x < (blocos[l].x+1)
                 && (bombermanRet.x + bombermanRet.width) > (blocos[l].x+1)
                 && (bombermanRet.x + bombermanRet.width) < ((blocos[l].x + TAMANHOBLOCOS)-1) ) ) ) )
        {
            blockDirec[baixo] = 1;
        }
        l++;
    }
    //Retorna 1 caso a direcao escolhida esteja bloqueada por um bloco
    if(l < NBLOCOS)
    {
        return 1;
    }
    return 0;

}

void movimentaBomberman(BOMBERMAN *bomberman, MAPA mapa, Rectangle blocos[], Rectangle bombermanRet)
{
    //Andar para a direita
    if( (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) && (bomberman->pos.x < (GetScreenWidth() - mapa.pos.x - bomberman->tamanho.x)) && !checaColisaoBombermanBlocos(blocos, bombermanRet, dir) )
    {
        bomberman->pos.x += VELBOMBERMAN;
    }

    //Andar para a esquerda
    if( (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) && (bomberman->pos.x > mapa.pos.x) && !checaColisaoBombermanBlocos(blocos,bombermanRet, esq) )
    {
        bomberman->pos.x -= VELBOMBERMAN;
    }

    //Andar para cima
    if( (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP)) && (bomberman->pos.y > mapa.pos.y)  && !checaColisaoBombermanBlocos(blocos,bombermanRet, cima) )
    {
        bomberman->pos.y -= VELBOMBERMAN;
    }

    //Andar para baixo
    if( (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN)) && (bomberman->pos.y < (GetScreenHeight() - mapa.pos.y - bomberman->tamanho.y)) && !checaColisaoBombermanBlocos(blocos,bombermanRet, baixo) )
    {
        bomberman->pos.y += VELBOMBERMAN;
    }
}

void movimentaInimigos(INIMIGO listaInimigos[], int ninimigos, MAPA mapa)
{
    int l, direcao_al, blockDirec[NDIRECOES] = {};
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

void geraBlocos(Rectangle blocos[NBLOCOS], MAPA mapa)
{
    int l=0, linha = TAMANHOBLOCOS, coluna = TAMANHOBLOCOS;
    while( l < NBLOCOS && mapa.pos.y + coluna < (GetScreenHeight() - mapa.pos.y - TAMANHOBLOCOS) )
    {
        blocos[l].height = TAMANHOBLOCOS;
        blocos[l].width = TAMANHOBLOCOS;
        blocos[l].x = mapa.pos.x + linha;
        blocos[l].y = mapa.pos.y + coluna;
        linha += TAMANHOBLOCOS*2;
        if( mapa.pos.x + linha >= (GetScreenWidth() - mapa.pos.x - TAMANHOBLOCOS) )
        {
            linha = TAMANHOBLOCOS;
            coluna += TAMANHOBLOCOS*2;
        }
        l++;
    }
}

void desenhaBlocos(Rectangle blocos[NBLOCOS])
{
    int l;
    for(l = 0; l < NBLOCOS; l++)
    {
        DrawRectangle(blocos[l].x, blocos[l].y, blocos[l].width, blocos[l].height, CORBLOCOS);
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
                ((GetScreenHeight() - ALTURAMAPA)/2)
            },
            { LARGURAMAPA, ALTURAMAPA }
        };

        //--------------------------------------------------------------------------------------

        //define os blocos nao quebraveis do mapa
        Rectangle blocos[NBLOCOS];
        geraBlocos(blocos, mapa);

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
                movimentaBomberman(&bomberman, mapa, blocos, bombermanRet);

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

                    //Desenha os blocos
                    desenhaBlocos(blocos);

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
