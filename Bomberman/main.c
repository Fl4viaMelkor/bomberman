#include <stdio.h>
#include <stdlib.h>
#include "raylib.h"


//LINHAS E COLUNAS
#define NLINHAS 27
#define NCOLUNAS 15

//BLOCOS
#define NBLOCOS ((NLINHAS-1)/2) * ((NCOLUNAS-1)/2) + 1
#define TAMANHOBLOCOS 40
#define CORBLOCOS GRAY

//TELA
#define LARGURATELA (NLINHAS*TAMANHOBLOCOS) + (TAMANHOBLOCOS*2)
#define ALTURATELA (NCOLUNAS*TAMANHOBLOCOS) + (TAMANHOBLOCOS*2)
#define CORFUNDO DARKGRAY

//MAPA
#define LARGURAMAPA NLINHAS*TAMANHOBLOCOS
#define ALTURAMAPA NCOLUNAS*TAMANHOBLOCOS
#define CORMAPA GREEN
#define CORMAPA2 VIOLET

//BOMBERMAN
#define LARGURABOMBERMAN 30
#define ALTURABOMBERMAN 30
#define VELBOMBERMAN 1.5f
#define CORBOMBERMAN BROWN

//INIMIGOS
#define TAMANHOINIMIGO 17
#define VELINIMIGO 1.5f
#define CORINIMIGO RED
#define NINIMIGOS 1
#define DANO 1

//PASSOS
#define MIN_PASSOS 20
#define MAX_PASSOS 100

//INSTRUCOES
#define CORINSTRUCAO WHITE

//DIRECOES
#define NDIRECOES 4

//FPS
#define FPS 60

//OUTROS
#define INVERTE -1
#define TRUE 1
#define FALSE 0
#define TABELASCORE 10




typedef struct
{
    int score;
} SCORE;


typedef struct
{
    Vector2 pos;
    Vector2 tamanho;
    int fase;
    int vidas;
    SCORE score;
} BOMBERMAN;



typedef struct
{
    Vector2 pos;
    Vector2 tamanho;
} MAPA;

typedef enum
{
    esq,
    dir,
    cima,
    baixo,
} DIRECAO;

typedef struct
{
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

int verficaPosInimigoBlocos(Rectangle blocos[], int posx_al, int posy_al)
{
    int l;
    for(l = 0; l < NBLOCOS; l++)
    {
        if( (posx_al > blocos[l].x && posx_al < (blocos[l].x + TAMANHOBLOCOS))
                || ((posx_al + TAMANHOINIMIGO) > blocos[l].x && (posx_al + TAMANHOINIMIGO) < (blocos[l].x + TAMANHOBLOCOS))
                || (posy_al > blocos[l].y && posy_al < (blocos[l].y + TAMANHOBLOCOS))
                || ((posy_al + TAMANHOINIMIGO) > blocos[l].y && (posy_al + TAMANHOINIMIGO) < (blocos[l].y + TAMANHOBLOCOS)) )
        {
            return 1;
        }
    }
    return 0;
}

void geraInimigos(INIMIGO listaInimigos[], int ninimigos, MAPA mapa, Rectangle blocos[])
{
    int l, direcao_al, posx_al, posy_al;
    for(l = 0; l < ninimigos; l++)
    {
        direcao_al = GetRandomValue(0, NDIRECOES-1);
        listaInimigos[l].dir = direcao_al;
        geraVelInimigo(&listaInimigos[l]);
        posx_al = GetRandomValue(TAMANHOINIMIGO + mapa.pos.x, (GetScreenWidth() - mapa.pos.x - TAMANHOINIMIGO));
        posy_al = GetRandomValue(TAMANHOINIMIGO + mapa.pos.y, (GetScreenHeight() - mapa.pos.y - TAMANHOINIMIGO));
        while(verficaPosInimigoBlocos(blocos, posx_al, posy_al))
        {
            posx_al = GetRandomValue(TAMANHOINIMIGO + mapa.pos.x, (GetScreenWidth() - mapa.pos.x - TAMANHOINIMIGO));
            posy_al = GetRandomValue(TAMANHOINIMIGO + mapa.pos.y, (GetScreenHeight() - mapa.pos.y - TAMANHOINIMIGO));
        }
        listaInimigos[l].pos.x = posx_al;
        listaInimigos[l].pos.y = posy_al;
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


int checkColisaoPlayerInimigo1(INIMIGO listaInimigos[],int numeroInmigos, Rectangle bombermanRet,BOMBERMAN *bomberman)
{
    int i;
    for(i=0; i<numeroInmigos; i++)
    {
        if(CheckCollisionCircleRec(listaInimigos[numeroInmigos].pos,TAMANHOINIMIGO,bombermanRet))
        {
            bomberman->vidas=-DANO;
        }
    }
}

///Outro jeito
    int checkColisaoPlayerInimigo(INIMIGO listaInimigos[],int numeroInmigos,BOMBERMAN *bomberman)
    {
        int i;
        for(i=0; i<numeroInmigos; i++)
        {
            if(listaInimigos[numeroInmigos].pos.x==bomberman->pos.x && listaInimigos[numeroInmigos].pos.y==bomberman->pos.y)
            {
                bomberman->vidas=-DANO;
                printf("COlidu");
            }
        }
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
                       && bombermanRet.y > (blocos[l].y)
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
                       && bombermanRet.y > (blocos[l].y)
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
                       && bombermanRet.x > (blocos[l].x)
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
                       && bombermanRet.x > (blocos[l].x)
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

int verificaLinhaColunaValidas(Rectangle blocos[], INIMIGO inimigo, int direcao)
{
    int l=0, blockDirec[NDIRECOES] = {};
    while( l < NBLOCOS &&  blockDirec[direcao] == 0)
    {
        //Checando se tem algum bloco na mesma linha do Inimigo
        if( ( ( (inimigo.pos.y - TAMANHOINIMIGO) < ((blocos[l].y + TAMANHOBLOCOS)-1)
                && (inimigo.pos.y - TAMANHOINIMIGO) > (blocos[l].y) )
                || ( ( (inimigo.pos.y + TAMANHOINIMIGO) > (blocos[l].y+1)
                       && (inimigo.pos.y + TAMANHOINIMIGO) < ((blocos[l].y + TAMANHOBLOCOS)-1) ) ) ) )
        {
            blockDirec[dir] = 1;
            blockDirec[esq] = 1;
        }

        //Checando colisao com bloco a esquerda do Bomberman
        if( ( ( (inimigo.pos.x - TAMANHOINIMIGO) < ((blocos[l].x + TAMANHOBLOCOS)-1)
                && (inimigo.pos.x - TAMANHOINIMIGO) > (blocos[l].x) )
                || ( ( (inimigo.pos.x + TAMANHOINIMIGO) > (blocos[l].x+1)
                       && (inimigo.pos.x + TAMANHOINIMIGO) < ((blocos[l].x + TAMANHOBLOCOS)-1) ) ) ) )
        {
            blockDirec[cima] = 1;
            blockDirec[baixo] = 1;
        }
        l++;
    }
    //Retorna 1 caso a direcao escolhida nao possa ser utilizada naquela linha/coluna
    if(l < NBLOCOS)
    {
        return 1;
    }
    return 0;
}

int checaColisaoInimigoBlocos(Rectangle blocos[], INIMIGO inimigo, int direcao)
{
    int l = 0, blockDirec[NDIRECOES] = {};
    while(l < NBLOCOS && blockDirec[direcao] == 0)
    {
        //Checando colisao com bloco a direita do Bomberman
        if( (inimigo.pos.x + TAMANHOINIMIGO) >= blocos[l].x
                && (inimigo.pos.x - TAMANHOINIMIGO) < blocos[l].x
                && ( ( (inimigo.pos.y - TAMANHOINIMIGO) < ((blocos[l].y + TAMANHOBLOCOS)-1)
                       && (inimigo.pos.y - TAMANHOINIMIGO) > (blocos[l].y) )
                     || ( ( (inimigo.pos.y + TAMANHOINIMIGO) > (blocos[l].y+1)
                            && (inimigo.pos.y + TAMANHOINIMIGO) < ((blocos[l].y + TAMANHOBLOCOS)-1) ) ) ) )
        {
            blockDirec[dir] = 1;
        }

        //Checando colisao com bloco a esquerda do Bomberman
        if( (inimigo.pos.x - TAMANHOINIMIGO) <= (blocos[l].x + TAMANHOBLOCOS)
                && (inimigo.pos.x  + TAMANHOINIMIGO) > (blocos[l].x + TAMANHOBLOCOS)
                && ( ( (inimigo.pos.y - TAMANHOINIMIGO) < ((blocos[l].y + TAMANHOBLOCOS)-1)
                       && (inimigo.pos.y - TAMANHOINIMIGO) > (blocos[l].y) )
                     || ( ( (inimigo.pos.y + TAMANHOINIMIGO) > (blocos[l].y+1)
                            && (inimigo.pos.y + TAMANHOINIMIGO) < ((blocos[l].y + TAMANHOBLOCOS)-1) ) ) ) )
        {
            blockDirec[esq] = 1;
        }

        //Checando colisao com bloco acima do Bomberman
        if( (inimigo.pos.y - TAMANHOINIMIGO) <= (blocos[l].y + TAMANHOBLOCOS)
                && (inimigo.pos.y + TAMANHOINIMIGO) > (blocos[l].y + TAMANHOBLOCOS)
                && ( ( (inimigo.pos.x - TAMANHOINIMIGO) < ((blocos[l].x + TAMANHOBLOCOS)-1)
                       && (inimigo.pos.x - TAMANHOINIMIGO) > (blocos[l].x) )
                     || ( ( (inimigo.pos.x + TAMANHOINIMIGO) > (blocos[l].x+1)
                            && (inimigo.pos.x + TAMANHOINIMIGO) < ((blocos[l].x + TAMANHOBLOCOS)-1) ) ) ) )
        {
            blockDirec[cima] = 1;
        }

        //Checando colisao com bloco abaixo do Bomberman
        if( (inimigo.pos.y  + TAMANHOINIMIGO) >= blocos[l].y
                && (inimigo.pos.y - TAMANHOINIMIGO) < blocos[l].y
                && ( ( (inimigo.pos.x - TAMANHOINIMIGO) < ((blocos[l].x + TAMANHOBLOCOS)-1)
                       && (inimigo.pos.x - TAMANHOINIMIGO) > (blocos[l].x) )
                     || ( ( (inimigo.pos.x + TAMANHOINIMIGO) > (blocos[l].x+1)
                            && (inimigo.pos.x + TAMANHOINIMIGO) < ((blocos[l].x + TAMANHOBLOCOS)-1) ) ) ) )
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

//---pontuação
    void SalvaPontucao(SCORE tabela[])
    {
        int quant = 1;
        FILE *arq = fopen("score.txt","wb");
        if(arq)
        {
            fwrite(tabela,sizeof(SCORE),quant,arq);
        }
        else{"Não foi possível abrir o arquivo";}
        fclose(arq);
    }

    void LerdoArquivo(SCORE tabela[])
    {
        FILE *arq = fopen("score.txt","rb");
        if(arq)
        {
            fread(tabela, sizeof(SCORE), TABELASCORE, arq);
            fclose(arq);

        }
        else
        {
            printf("Erro ao abrir score");
        }
    }


//}
//---------------------

void movimentaInimigos(INIMIGO listaInimigos[], int ninimigos, MAPA mapa, Rectangle blocos[])
{
    int l, direcao_al, blockDirec[NDIRECOES] = {};
    for(l = 0; l < ninimigos; l++)
    {
        //Checando colisao na borda da direita
        if( listaInimigos[l].pos.x >= (GetScreenWidth() - mapa.pos.x - TAMANHOINIMIGO) || checaColisaoInimigoBlocos(blocos, listaInimigos[l], dir) )
        {
            blockDirec[dir] = 1;
        }

        //Checando colisao na borda da esquerda
        if( listaInimigos[l].pos.x <= (TAMANHOINIMIGO + mapa.pos.x) || checaColisaoInimigoBlocos(blocos, listaInimigos[l], esq) )
        {
            blockDirec[esq] = 1;
        }

        //Checando colisao na borda superior
        if( listaInimigos[l].pos.y <= (TAMANHOINIMIGO + mapa.pos.y) || checaColisaoInimigoBlocos(blocos, listaInimigos[l], cima) )
        {
            blockDirec[cima] = 1;
        }

        //Andar para baixo
        if( listaInimigos[l].pos.y >= (GetScreenHeight() - mapa.pos.y - TAMANHOINIMIGO) || checaColisaoInimigoBlocos(blocos, listaInimigos[l], baixo) )
        {
            blockDirec[baixo] = 1;
        }

        //Gera nova direcao aleatoria caso tenha colidido com uma parede ou o numero de passos permitidos tenha se esgotado
        if( blockDirec[listaInimigos[l].dir] == 1 || listaInimigos[l].passos >= listaInimigos[l].passosMax )
        {
            direcao_al = GetRandomValue(0, NDIRECOES-1);
            //verifica se a direcao esta bloqueada ou nao esta em uma linha e/ou coluna valida
            while( blockDirec[direcao_al] == 1 || verificaLinhaColunaValidas(blocos, listaInimigos[l], direcao_al) == 1 )
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
    SetTargetFPS(FPS);

    //--------------------------------------------------------------------------------------

    //define o MAPA e seus dados
    MAPA mapa =
    {
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
    int vida=3,fase=1;
    BOMBERMAN bomberman =
    {
        {
            (GetScreenWidth()/2) - (LARGURABOMBERMAN/2) + TAMANHOBLOCOS,
            (GetScreenHeight()/2) - (ALTURABOMBERMAN/2),
        },
        { LARGURABOMBERMAN, ALTURABOMBERMAN },
        fase, //fase inicial
        vida, //vida inicial
        0
    };

    //define a estrutura retangulo do BOMBERMAN para testar a colisão
    Rectangle bombermanRet = { bomberman.pos.x, bomberman.pos.y, bomberman.tamanho.x, bomberman.tamanho.y };

    //--------------------------------------------------------------------------------------

    //inicializa o vetor de INIMIGOS
    int l;
    INIMIGO listaInimigos[NINIMIGOS];

    //gera os INIMIGOS
    geraInimigos(listaInimigos, NINIMIGOS, mapa, blocos);

    //Score
    SCORE tabeladescore[TABELASCORE+1];

    //--------------------------------------------------------------------------------------

    //--------------------------------------------------------------------------------------

    // Loop de Jogo (continua enquanto o usuario nao clicar "ESC" e enquanto o bomberman nao morrer para um inimigo
    //--------------------------------------------------------------------------------------

    while (!WindowShouldClose())
    {

        // Atualização da tela do jogo
        //----------------------------------------------------------------------------------

        //movimenta o Bomberman a partir do teclado
        movimentaBomberman(&bomberman, mapa, blocos, bombermanRet);

        //Atualiza a posicao da estrutura retangulo do Bomberman (bombermanRet)
        bombermanRet.x = bomberman.pos.x;
        bombermanRet.y = bomberman.pos.y;

        //Atualiza a posicao dos inimigos
        movimentaInimigos(listaInimigos, NINIMIGOS, mapa, blocos);

        //Checa colisão
        checkColisaoPlayerInimigo1(listaInimigos,NINIMIGOS,bombermanRet,&bomberman);
        printf("VIDA: %d",bomberman.vidas);
        if(bomberman.vidas=0)
        {
            char op;
            DrawText("Deseja continuar?", 10,10,20, PURPLE);
            scanf("%c",&op);
            switch(op)
            {
                case 1:
                    LerdoArquivo(tabeladescore);
                    //OrganizaTabela
                    SalvaPontucao(tabeladescore);
                    break;

            }
        }

        //----------------------------------------------------------------------------------

        // Desenho do jogo
        //----------------------------------------------------------------------------------

        BeginDrawing();

        //Desenha fundo da tela (borda do mapa)
        ClearBackground(CORFUNDO);

        //Desenha o mapa
        //DrawRectangleGradientH(mapa.pos.x, mapa.pos.y, LARGURAMAPA,ALTURAMAPA, MAROON, GOLD);
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
