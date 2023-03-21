#include "raylib.h"
#include <stdio.h>

//LINHAS E COLUNAS
#define NLINHAS 27
#define NCOLUNAS 15

//BLOCOS
#define NBLOCOS NLINHAS * NCOLUNAS //((NLINHAS-1)/2) * ((NCOLUNAS-1)/2) + 1
#define TAMANHOBLOCOS 40
#define CORBLOCOS GRAY
#define NFORMAS_EXPLOSAO 7

//TELA
#define LARGURATELA GetMonitorWidth(0)
#define ALTURATELA GetMonitorHeight(0)
#define CORFUNDO DARKGRAY

//MAPA
#define LARGURAMAPA NLINHAS*TAMANHOBLOCOS
#define ALTURAMAPA NCOLUNAS*TAMANHOBLOCOS
#define CORMAPA (Color){ 1, 149, 1, 255 }

//BOMBERMAN
#define LARGURABOMBERMAN 26
#define ALTURABOMBERMAN 38
#define VELBOMBERMAN 1.5f
#define CORBOMBERMAN BROWN
#define NOME 20
#define VIDAS 3

//INIMIGOS
#define TAMANHOINIMIGO 19
#define VELINIMIGO 1.0f
#define CORINIMIGO RED
#define NINIMIGOS 10
#define DIST_MIN_BOMBERMAN_INI 300
#define DIST_MIN_PERSEG TAMANHOBLOCOS*6

//PASSOS
#define MIN_PASSOS 20
#define MAX_PASSOS 100

//BOMBAS
#define NBOMBAS 3
#define POSIN_BOMBAS INVERTE
#define TAMANHOBOMBA 17
#define CORBOMBA BLACK
#define ALCANCE 1
#define FRAMES_EXPLOSAO 20
#define COREXPLOSAO ORANGE

//MUROS
#define MIN_MUROS 10
#define MAX_MUROS 100 //LIMITE: 314
#define NMUROS GetRandomValue(MIN_MUROS, MAX_MUROS)
#define CORMUROS BLUE
#define DIST_MIN_BOMBERMAN_MUROS 50

//PORTAL
#define CORPORTAL ORANGE

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
#define DANO 1




typedef struct
{
    int score;
} SCORE;


typedef struct
{

    Rectangle ret;
    int morto;
    Texture2D textura;
    Vector2 pos;
    Vector2 tamanho;
    int fase;
    int vidas;
    SCORE score;
    char nome[NOME];

} BOMBERMAN;


typedef struct{
    Rectangle ret;
    Color cor;
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
    int morto;

} INIMIGO;

typedef enum
{
    transponivel,
    intransponivel,
    quebravel,
    explosao
} STATUSBLOCO;

typedef struct {
    Rectangle ret;
    STATUSBLOCO status;
    int blockDir[NDIRECOES];
    int bomba;
    int portal;
    int contadorExplosao;
    Texture2D texturaPortal;
} BLOCO;

typedef enum
{
    inerte,
    ativada,
    detonada
} STATUSBOMBA;

typedef struct{
    Vector2 pos;
    STATUSBOMBA status;
} BOMBA;


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



int verficaPosInimigoBlocos(BLOCO blocos[], int posx_al, int posy_al)
{
    int l;
    for(l = 0; l < NBLOCOS; l++)
    {
        if( blocos[l].status != transponivel
           && CheckCollisionRecs((Rectangle) { posx_al - TAMANHOINIMIGO - 1, posy_al - TAMANHOINIMIGO - 1, TAMANHOINIMIGO*2 + 2, TAMANHOINIMIGO*2 + 2 }, blocos[l].ret) )
        {
            return 1;
        }
    }
    return 0;
}

void geraInimigos(INIMIGO listaInimigos[], int ninimigos, MAPA mapa, BLOCO blocos[], BOMBERMAN bomberman)
{
    int l, direcao_al, posx_al, posy_al;
    for(l = 0; l < ninimigos; l++)
    {
        direcao_al = GetRandomValue(0, NDIRECOES-1);
        listaInimigos[l].dir = direcao_al;
        geraVelInimigo(&listaInimigos[l]);

        posx_al = GetRandomValue(TAMANHOINIMIGO + mapa.ret.x, (GetScreenWidth() - mapa.ret.x - TAMANHOINIMIGO));
        posy_al = GetRandomValue(TAMANHOINIMIGO + mapa.ret.y, (GetScreenHeight() - mapa.ret.y - TAMANHOINIMIGO));

        while( verficaPosInimigoBlocos(blocos, posx_al, posy_al)
              || CheckCollisionCircleRec( (Vector2) {posx_al, posy_al}, TAMANHOINIMIGO,
                                          (Rectangle) { bomberman.ret.x + (bomberman.ret.width/2) - (DIST_MIN_BOMBERMAN_INI/2),
                                                        bomberman.ret.y + (bomberman.ret.height/2) - (DIST_MIN_BOMBERMAN_INI/2),
                                                        DIST_MIN_BOMBERMAN_INI, DIST_MIN_BOMBERMAN_INI } ) )
        {
            posx_al = GetRandomValue(TAMANHOINIMIGO + mapa.ret.x, (GetScreenWidth() - mapa.ret.x - TAMANHOINIMIGO));
            posy_al = GetRandomValue(TAMANHOINIMIGO + mapa.ret.y, (GetScreenHeight() - mapa.ret.y - TAMANHOINIMIGO));
        }

        listaInimigos[l].pos.x = posx_al;
        listaInimigos[l].pos.y = posy_al;
        listaInimigos[l].passos = 0;
        listaInimigos[l].passosMax = GetRandomValue(MIN_PASSOS, MAX_PASSOS);
        listaInimigos[l].morto = 0;
    }
}

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

void checkColisaoPlayerInimigo1(INIMIGO listaInimigos[],int numeroInmigos, Rectangle bombermanRet,BOMBERMAN *bomberman)
{
    int i;
    for(i=0; i<numeroInmigos; i++)
    {
        if(CheckCollisionCircleRec(listaInimigos[i].pos, TAMANHOINIMIGO, bombermanRet))
        {
            bomberman->vidas = bomberman->vidas - DANO;
            if(bomberman->vidas>0)
            {
                bomberman->pos.x =(GetScreenWidth()/2) - (LARGURABOMBERMAN/2) + TAMANHOBLOCOS;
                bomberman->pos.y = (GetScreenHeight()/2) - (ALTURABOMBERMAN/2);
            }
        }
    }
}


int checaColisaoBombermanBlocos(BLOCO blocos[], BOMBERMAN bomberman, int direcao)
{
    int l = 0, blockDirec[NDIRECOES] = {};
    while(l < NBLOCOS && blockDirec[direcao] == 0)
    {
        if( blocos[l].status == intransponivel || blocos[l].status == quebravel )
        {
            //Checando colisao com bloco a direita do Bomberman
            if( CheckCollisionRecs((Rectangle) {bomberman.ret.x + VELBOMBERMAN, bomberman.ret.y, bomberman.ret.width, bomberman.ret.height}, blocos[l].ret) )
            {
                blockDirec[dir] = 1;
            }

            //Checando colisao com bloco a esquerda do Bomberman
            if( CheckCollisionRecs((Rectangle) {bomberman.ret.x - VELBOMBERMAN, bomberman.ret.y, bomberman.ret.width, bomberman.ret.height}, blocos[l].ret) )
            {
                blockDirec[esq] = 1;
            }

            //Checando colisao com bloco acima do Bomberman
            if( CheckCollisionRecs((Rectangle) {bomberman.ret.x, bomberman.ret.y - VELBOMBERMAN, bomberman.ret.width, bomberman.ret.height}, blocos[l].ret) )
            {
                blockDirec[cima] = 1;
            }

            //Checando colisao com bloco abaixo do Bomberman
            if( CheckCollisionRecs((Rectangle) {bomberman.ret.x, bomberman.ret.y + VELBOMBERMAN, bomberman.ret.width, bomberman.ret.height}, blocos[l].ret) )
            {
                blockDirec[baixo] = 1;
            }
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

/*int checaColisaoBombermanBombas(BOMBERMAN bomberman, BOMBA listaBombas[], int nbombas, int direcao)
{
    int l = 0, valX = 0, valY=0;
    if(direcao == esq)
    {
        valX = INVERTE*VELINIMIGO;
    }
    else if(direcao == dir)
    {
        valX = VELINIMIGO;
    }
    else if(direcao == cima)
    {
        valY = INVERTE*VELINIMIGO;
    }
    else if(direcao == baixo)
    {
        valY = VELINIMIGO;
    }

    while(l < nbombas && CheckCollisionCircleRec(listaBombas[l].pos, TAMANHOBOMBA, bomberman.ret))
    {
        l++;
    }
    if(l >= nbombas)
    {
        printf("bbb");
        l=0;
        while(l < nbombas && !CheckCollisionCircleRec(listaBombas[l].pos, TAMANHOBOMBA, (Rectangle) { bomberman.ret.x + valX, bomberman.ret.y + valY, bomberman.ret.width, bomberman.ret.height }))
        {
            l++;
        }
        if(l < nbombas)
        {
            printf("aaa");
            return 1;
        }
    }
    return 0;
}*/

void movimentaBomberman(BOMBERMAN *bomberman, MAPA mapa, BLOCO blocos[], BOMBA listaBombas[], int nbombas)
{
    //Andar para a direita
    if( (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT))
       && (bomberman->ret.x < (GetScreenWidth() - mapa.ret.x - bomberman->ret.width - VELBOMBERMAN))
       && !checaColisaoBombermanBlocos(blocos, *bomberman, dir)
       /*&& !checaColisaoBombermanBombas(*bomberman, listaBombas, nbombas, dir)*/ )
    {
        bomberman->ret.x += VELBOMBERMAN;
    }

    //Andar para a esquerda
    if( (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT))
       && (bomberman->ret.x > mapa.ret.x + VELBOMBERMAN)
       && !checaColisaoBombermanBlocos(blocos, *bomberman, esq)
       /*&& !checaColisaoBombermanBombas(*bomberman, listaBombas, nbombas, esq)*/ )
    {
        bomberman->ret.x -= VELBOMBERMAN;
    }

    //Andar para cima
    if( (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP))
       && (bomberman->ret.y > mapa.ret.y + VELBOMBERMAN)
       && !checaColisaoBombermanBlocos(blocos, *bomberman, cima)
       /*&& !checaColisaoBombermanBombas(*bomberman, listaBombas, nbombas, cima)*/ )
    {
        bomberman->ret.y -= VELBOMBERMAN;
    }

    //Andar para baixo
    if( (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN))
       && (bomberman->ret.y < (GetScreenHeight() - mapa.ret.y - bomberman->ret.height - VELBOMBERMAN))
       && !checaColisaoBombermanBlocos(blocos, *bomberman, baixo)
       /*&& !checaColisaoBombermanBombas(*bomberman, listaBombas, nbombas, baixo)*/ )
    {
        bomberman->ret.y += VELBOMBERMAN;
    }
}

int verificaLinhaColunaValidas(BLOCO blocos[], INIMIGO inimigo, int direcao)
{
    int l=0, c, blockDirec[NDIRECOES] = {}, contador=0;
    while( l < NBLOCOS && blockDirec[direcao] == 0)
    {
        if(CheckCollisionCircleRec(inimigo.pos, TAMANHOINIMIGO, blocos[l].ret))
        {
            for(c = 0; c < NDIRECOES; c++)
            {
                if(blocos[l].blockDir[c] == 1)
                {
                    blockDirec[c] = 1;
                    //printf("%4d = %d", l, c);
                }
            }
        }
        l++;
    }
    contador=0;
    for(c=0; c<NDIRECOES; c++)
    {
        if(blockDirec[c]==1)
        {
            contador++;
        }
    }
    if(contador >= NDIRECOES)
    {
        //printf("bloqueado");
        for(c=0; c<NBLOCOS; c++)
        {
            if(CheckCollisionCircleRec(inimigo.pos, TAMANHOINIMIGO, blocos[c].ret))
            {
                //printf("\n c=%4d\nposinx=%4f\nposretx=%4f", c, inimigo.pos.x, blocos[c].ret.x);
            }
        }
    }

    //Retorna 1 caso a direcao escolhida nao possa ser utilizada naquela linha/coluna
    if(l < NBLOCOS)
    {
        return 1;
    }
    return 0;
}

/*int checaColisaoInimigoBlocos(BLOCO blocos[], INIMIGO inimigo, int direcao)
{
    int l = 0, blockDirec[NDIRECOES] = {};
    while(l < NBLOCOS && blockDirec[direcao] == 0)
    {
        if(blocos[l].intransponivel == 1)
        {
            //Checando colisao com bloco a direita do Bomberman
            if( (inimigo.pos.x + TAMANHOINIMIGO)+1 >= blocos[l].ret.x
               && (inimigo.pos.x - TAMANHOINIMIGO)-1 < blocos[l].ret.x
               && ( ( (inimigo.pos.y - TAMANHOINIMIGO) < ((blocos[l].ret.y + TAMANHOBLOCOS)-1)
                     && (inimigo.pos.y - TAMANHOINIMIGO) > (blocos[l].ret.y) )
                   || ( ( (inimigo.pos.y + TAMANHOINIMIGO) > (blocos[l].ret.y+1)
                     && (inimigo.pos.y + TAMANHOINIMIGO) < ((blocos[l].ret.y + TAMANHOBLOCOS)-1) ) ) ) )
            {
                blockDirec[dir] = 1;
            }

            //Checando colisao com bloco a esquerda do Bomberman
            if( (inimigo.pos.x - TAMANHOINIMIGO)-1 <= (blocos[l].ret.x + TAMANHOBLOCOS)
               && (inimigo.pos.x  + TAMANHOINIMIGO)+1 > (blocos[l].ret.x + TAMANHOBLOCOS)
               && ( ( (inimigo.pos.y - TAMANHOINIMIGO) < ((blocos[l].ret.y + TAMANHOBLOCOS)-1)
                     && (inimigo.pos.y - TAMANHOINIMIGO) > (blocos[l].ret.y) )
                   || ( ( (inimigo.pos.y + TAMANHOINIMIGO) > (blocos[l].ret.y+1)
                     && (inimigo.pos.y + TAMANHOINIMIGO) < ((blocos[l].ret.y + TAMANHOBLOCOS)-1) ) ) ) )
            {
                blockDirec[esq] = 1;
            }

            //Checando colisao com bloco acima do Bomberman
            if( (inimigo.pos.y - TAMANHOINIMIGO)-1 <= (blocos[l].ret.y + TAMANHOBLOCOS)
               && (inimigo.pos.y + TAMANHOINIMIGO)+1 > (blocos[l].ret.y + TAMANHOBLOCOS)
               && ( ( (inimigo.pos.x - TAMANHOINIMIGO) < ((blocos[l].ret.x + TAMANHOBLOCOS)-1)
                     && (inimigo.pos.x - TAMANHOINIMIGO) > (blocos[l].ret.x) )
                   || ( ( (inimigo.pos.x + TAMANHOINIMIGO) > (blocos[l].ret.x+1)
                     && (inimigo.pos.x + TAMANHOINIMIGO) < ((blocos[l].ret.x + TAMANHOBLOCOS)-1) ) ) ) )
            {
                blockDirec[cima] = 1;
            }

            //Checando colisao com bloco abaixo do Bomberman
            if( (inimigo.pos.y  + TAMANHOINIMIGO)-1 >= blocos[l].ret.y
               && (inimigo.pos.y - TAMANHOINIMIGO)+1 < blocos[l].ret.y
               && ( ( (inimigo.pos.x - TAMANHOINIMIGO) < ((blocos[l].ret.x + TAMANHOBLOCOS)-1)
                     && (inimigo.pos.x - TAMANHOINIMIGO) > (blocos[l].ret.x) )
                   || ( ( (inimigo.pos.x + TAMANHOINIMIGO) > (blocos[l].ret.x+1)
                     && (inimigo.pos.x + TAMANHOINIMIGO) < ((blocos[l].ret.x + TAMANHOBLOCOS)-1) ) ) ) )
            {
                blockDirec[baixo] = 1;
            }
        }
        l++;
    }
    //Retorna 1 caso a direcao escolhida esteja bloqueada por um bloco
    if(l < NBLOCOS)
    {
        return 1;
    }
    return 0;
}*/

int checaColisaoInimigoBombas(BOMBA bombas[], int nbombas, INIMIGO inimigo, int direcao)
{
    int l = 0;
    while(l < nbombas)
    {
        if(bombas[l].status == ativada)
        {
            //printf("bomba");
            //Checando colisao com bomba a direita do inimigo
            if( direcao == dir && CheckCollisionCircles((Vector2) { inimigo.pos.x + VELINIMIGO, inimigo.pos.y }, TAMANHOINIMIGO, bombas[l].pos, TAMANHOBOMBA) )
            {
                return 1;
            }

            //Checando colisao com bomba a esquerda do inimigo
            if( direcao == esq && CheckCollisionCircles((Vector2) { inimigo.pos.x - VELINIMIGO, inimigo.pos.y }, TAMANHOINIMIGO, bombas[l].pos, TAMANHOBOMBA) )
            {
                return 1;
            }

            //Checando colisao com bomba acima do inimigo
            if( direcao == cima && CheckCollisionCircles((Vector2) { inimigo.pos.x, inimigo.pos.y - VELINIMIGO }, TAMANHOINIMIGO, bombas[l].pos, TAMANHOBOMBA) )
            {
                return 1;
            }

            //Checando colisao com bomba abaixo do inimigo
            if( direcao == baixo && CheckCollisionCircles((Vector2) { inimigo.pos.x, inimigo.pos.y + VELINIMIGO }, TAMANHOINIMIGO, bombas[l].pos, TAMANHOBOMBA) )
            {
                return 1;
            }
        }
        l++;
    }

    return 0;

}

int checaColisaoInimigoMuros(BLOCO blocos[], INIMIGO inimigo, int direcao)
{
    int l = 0;
    while(l < NBLOCOS)
    {
        if(blocos[l].status == quebravel)
        {
            //Checando colisao com bomba a direita do inimigo
            if( direcao == dir && CheckCollisionCircleRec((Vector2) { inimigo.pos.x + VELINIMIGO, inimigo.pos.y }, TAMANHOINIMIGO, blocos[l].ret) )
            {
                return 1;
            }

            //Checando colisao com bomba a esquerda do inimigo
            if( direcao == esq && CheckCollisionCircleRec((Vector2) { inimigo.pos.x - VELINIMIGO, inimigo.pos.y }, TAMANHOINIMIGO, blocos[l].ret) )
            {
                return 1;
            }

            //Checando colisao com bomba acima do inimigo
            if( direcao == cima && CheckCollisionCircleRec((Vector2) { inimigo.pos.x, inimigo.pos.y - VELINIMIGO }, TAMANHOINIMIGO, blocos[l].ret) )
            {
                return 1;
            }

            //Checando colisao com bomba abaixo do inimigo
            if( direcao == baixo && CheckCollisionCircleRec((Vector2) { inimigo.pos.x, inimigo.pos.y + VELINIMIGO }, TAMANHOINIMIGO, blocos[l].ret) )
            {
                return 1;
            }
        }
        l++;
    }

    return 0;

}

int persegueBomberman(INIMIGO inimigo, BOMBERMAN bomberman, BLOCO blocos[])
{
    //ve se o inimigo esta perto do bomberman
    if(CheckCollisionCircleRec( (Vector2) {inimigo.pos.x,inimigo.pos.y}, TAMANHOINIMIGO,
                                          (Rectangle) { bomberman.ret.x + (bomberman.ret.width/2) - (DIST_MIN_PERSEG/2),
                                                        bomberman.ret.y + (bomberman.ret.height/2) - (DIST_MIN_PERSEG/2),
                                                        DIST_MIN_PERSEG, DIST_MIN_PERSEG } ))
    {
        //verifica se o bomberman e o inimigo estao na mesma linha vertical
        if( (inimigo.pos.x - TAMANHOINIMIGO >= bomberman.ret.x && inimigo.pos.x - TAMANHOINIMIGO <= bomberman.ret.x + bomberman.ret.height)
           || (inimigo.pos.x + TAMANHOINIMIGO >= bomberman.ret.x && inimigo.pos.x + TAMANHOINIMIGO <= bomberman.ret.x + bomberman.ret.height) )
        {
            if(inimigo.pos.y - TAMANHOINIMIGO >= bomberman.ret.y )
            {
                return cima;
            }
            else{
                return baixo;
            }
        }
        else if( (inimigo.pos.y - TAMANHOINIMIGO >= bomberman.ret.y && inimigo.pos.y - TAMANHOINIMIGO <= bomberman.ret.y + bomberman.ret.width)
           || (inimigo.pos.y + TAMANHOINIMIGO >= bomberman.ret.y && inimigo.pos.y + TAMANHOINIMIGO <= bomberman.ret.y + bomberman.ret.width) )
        {
            if(inimigo.pos.x - TAMANHOINIMIGO >= bomberman.ret.x )
            {
                return esq;
            }
            else{
                return dir;
            }
        }
    }
    return inimigo.dir;
}

int contaDesblock(int blockDirec[])
{
    int l, contador=0;
    for(l = 0; l < NDIRECOES; l++)
    {
        if(blockDirec[l] == 0)
        {
            contador++;
        }
    }
    return contador;
}

int fogeBombas(INIMIGO inimigo, BLOCO blocos[], int direcao)
{
    int l=0, c, blockDirec[NDIRECOES] = {};
    while( l < NBLOCOS && blockDirec[direcao] == 0)
    {
        if(CheckCollisionCircleRec(inimigo.pos, TAMANHOINIMIGO, blocos[l].ret))
        {
            //alcance direita
            c=l+1;
            while( c >= 0 && c < NBLOCOS && c <= l+ALCANCE && c%NLINHAS != 0 && blocos[c].status != intransponivel )
            {
                if(CheckCollisionCircleRec((Vector2) {inimigo.pos.x + VELINIMIGO, inimigo.pos.y} , TAMANHOINIMIGO, blocos[c].ret))
                {
                    blockDirec[dir] = 1;
                }
                else if(CheckCollisionCircleRec((Vector2) {inimigo.pos.x - VELINIMIGO, inimigo.pos.y} , TAMANHOINIMIGO, blocos[c].ret))
                {
                    blockDirec[esq] = 1;
                }
                else if(CheckCollisionCircleRec((Vector2) {inimigo.pos.x, inimigo.pos.y - VELINIMIGO} , TAMANHOINIMIGO, blocos[c].ret))
                {
                    blockDirec[cima] = 1;
                }
                else if(CheckCollisionCircleRec((Vector2) {inimigo.pos.x, inimigo.pos.y + VELINIMIGO} , TAMANHOINIMIGO, blocos[c].ret))
                {
                    blockDirec[baixo] = 1;
                }
                c++;
            }

            //alcance esquerda
            c=l-1;
            while( c >= 0 && c < NBLOCOS && c >= l-ALCANCE && c%NLINHAS != NLINHAS-1 && blocos[c].status != intransponivel )
            {
                if(CheckCollisionCircleRec((Vector2) {inimigo.pos.x + VELINIMIGO, inimigo.pos.y} , TAMANHOINIMIGO, blocos[c].ret))
                {
                    blockDirec[dir] = 1;
                }
                else if(CheckCollisionCircleRec((Vector2) {inimigo.pos.x - VELINIMIGO, inimigo.pos.y} , TAMANHOINIMIGO, blocos[c].ret))
                {
                    blockDirec[esq] = 1;
                }
                else if(CheckCollisionCircleRec((Vector2) {inimigo.pos.x, inimigo.pos.y - VELINIMIGO} , TAMANHOINIMIGO, blocos[c].ret))
                {
                    blockDirec[cima] = 1;
                }
                else if(CheckCollisionCircleRec((Vector2) {inimigo.pos.x, inimigo.pos.y + VELINIMIGO} , TAMANHOINIMIGO, blocos[c].ret))
                {
                    blockDirec[baixo] = 1;
                }
                c--;
            }

            //alcance baixo
            c=l+NLINHAS;
            while( c >= 0 && c < NBLOCOS && c <= l+(ALCANCE*NLINHAS) && blocos[c].status != intransponivel )
            {
                if(CheckCollisionCircleRec((Vector2) {inimigo.pos.x + VELINIMIGO, inimigo.pos.y} , TAMANHOINIMIGO, blocos[c].ret))
                {
                    blockDirec[dir] = 1;
                }
                else if(CheckCollisionCircleRec((Vector2) {inimigo.pos.x - VELINIMIGO, inimigo.pos.y} , TAMANHOINIMIGO, blocos[c].ret))
                {
                    blockDirec[esq] = 1;
                }
                else if(CheckCollisionCircleRec((Vector2) {inimigo.pos.x, inimigo.pos.y - VELINIMIGO} , TAMANHOINIMIGO, blocos[c].ret))
                {
                    blockDirec[cima] = 1;
                }
                else if(CheckCollisionCircleRec((Vector2) {inimigo.pos.x, inimigo.pos.y + VELINIMIGO} , TAMANHOINIMIGO, blocos[c].ret))
                {
                    blockDirec[baixo] = 1;
                }
                c += NLINHAS;
            }

            //alcance cima
            c=l-NLINHAS;
            while( c >= 0 && c < NBLOCOS && c >= l-(ALCANCE*NLINHAS) && blocos[c].status != intransponivel )
            {
                if(CheckCollisionCircleRec((Vector2) {inimigo.pos.x + VELINIMIGO, inimigo.pos.y} , TAMANHOINIMIGO, blocos[c].ret))
                {
                    blockDirec[dir] = 1;
                }
                else if(CheckCollisionCircleRec((Vector2) {inimigo.pos.x - VELINIMIGO, inimigo.pos.y} , TAMANHOINIMIGO, blocos[c].ret))
                {
                    blockDirec[esq] = 1;
                }
                else if(CheckCollisionCircleRec((Vector2) {inimigo.pos.x, inimigo.pos.y - VELINIMIGO} , TAMANHOINIMIGO, blocos[c].ret))
                {
                    blockDirec[cima] = 1;
                }
                else if(CheckCollisionCircleRec((Vector2) {inimigo.pos.x, inimigo.pos.y + VELINIMIGO} , TAMANHOINIMIGO, blocos[c].ret))
                {
                    blockDirec[baixo] = 1;
                }
                c -= NLINHAS;
            }
        }
        l++;
    }

    //Retorna 1 caso tenha uma bomba na direcao escolhida
    if(l < NBLOCOS)
    {
        return 1;
    }
    return 0;
}

void movimentaInimigos(INIMIGO listaInimigos[], int ninimigos, MAPA mapa, BLOCO blocos[], BOMBA bombas[], int nbombas, BOMBERMAN bomberman)
{
    int l, c, direcao_al, blockDirec[NDIRECOES] = {};
    for(l = 0; l < ninimigos; l++)
    {
        if(listaInimigos[l].morto == 0)
        {
            //Checando colisao na borda da direita
            if( (listaInimigos[l].pos.x + TAMANHOINIMIGO)+1 >= (GetScreenWidth() - mapa.ret.x)
               || verificaLinhaColunaValidas(blocos, listaInimigos[l], dir)
               || checaColisaoInimigoBombas(bombas, nbombas, listaInimigos[l], dir)
               || checaColisaoInimigoMuros(blocos, listaInimigos[l], dir) )
            {
                blockDirec[dir] = 1;
            }

            //Checando colisao na borda da esquerda
            if( (listaInimigos[l].pos.x - TAMANHOINIMIGO)-1 <= mapa.ret.x
               || verificaLinhaColunaValidas(blocos, listaInimigos[l], esq)
               || checaColisaoInimigoBombas(bombas, nbombas, listaInimigos[l], esq)
               || checaColisaoInimigoMuros(blocos, listaInimigos[l], esq) )
            {
                blockDirec[esq] = 1;
            }

            //Checando colisao na borda superior
            if( (listaInimigos[l].pos.y - TAMANHOINIMIGO)-1 <= mapa.ret.y
               || verificaLinhaColunaValidas(blocos, listaInimigos[l], cima)
               || checaColisaoInimigoBombas(bombas, nbombas, listaInimigos[l], cima)
               || checaColisaoInimigoMuros(blocos, listaInimigos[l], cima) )
            {
                blockDirec[cima] = 1;
            }

            //Andar para baixo
            if( (listaInimigos[l].pos.y + TAMANHOINIMIGO)+1 >= (GetScreenHeight() - mapa.ret.y)
               || verificaLinhaColunaValidas(blocos, listaInimigos[l], baixo)
               || checaColisaoInimigoBombas(bombas, nbombas, listaInimigos[l], baixo)
               || checaColisaoInimigoMuros(blocos, listaInimigos[l], baixo) )
            {
                blockDirec[baixo] = 1;
            }

            //gera uma nova direcao para perseguir o bomberman
            direcao_al = persegueBomberman(listaInimigos[l], bomberman, blocos);

            if(direcao_al != listaInimigos[l].dir && blockDirec[direcao_al] == 0
               && !fogeBombas(listaInimigos[l], blocos, direcao_al))
            {
                listaInimigos[l].dir = direcao_al;
                geraVelInimigo(&listaInimigos[l]);
            }

            //Gera nova direcao aleatoria caso tenha colidido com uma parede ou o numero de passos permitidos tenha se esgotado
            else if( blockDirec[listaInimigos[l].dir] == 1 || contaDesblock(blockDirec) >= 3 /*|| listaInimigos[l].passos >= listaInimigos[l].passosMax*/ )
            {
                //gera uma nova direcao aleatoria
                direcao_al = GetRandomValue(0, NDIRECOES-1);

                //verifica se a direcao esta bloqueada ou nao
                while( blockDirec[direcao_al] == 1)
                {
                    direcao_al = GetRandomValue(0, NDIRECOES-1);
                }

                //atualiza inimigo
                listaInimigos[l].dir = direcao_al;
                geraVelInimigo(&listaInimigos[l]);
                listaInimigos[l].passos=0;
                listaInimigos[l].passosMax = GetRandomValue(MIN_PASSOS, MAX_PASSOS);
            }

            //Movimenta o inimigo
            listaInimigos[l].pos.x += listaInimigos[l].vel.x;
            listaInimigos[l].pos.y += listaInimigos[l].vel.y;
            listaInimigos[l].passos++;

            //Zera posicoes bloqueadas
            for(c = 0; c < NDIRECOES; c++)
            {
                blockDirec[c] = 0;
            }
        }
    }
}

void geraBlocos(BLOCO blocos[], MAPA mapa)
{
    int l=0, c, linha = 0, coluna = 0;
    while( l < NBLOCOS && mapa.ret.y + coluna < (GetScreenHeight() - mapa.ret.y) )
    {
        blocos[l].ret.height = TAMANHOBLOCOS;
        blocos[l].ret.width = TAMANHOBLOCOS;
        blocos[l].ret.x = mapa.ret.x + linha;
        blocos[l].ret.y = mapa.ret.y + coluna;

        blocos[l].status = transponivel;
        blocos[l].bomba = INVERTE;

        blocos[l].portal = 0;
        //blocos[l].texturaPortal = LoadTexture("");


        for(c=0; c < NDIRECOES; c++)
        {
            blocos[l].blockDir[c] = 0;
        }
        linha += TAMANHOBLOCOS;
        if( mapa.ret.x + linha >= (GetScreenWidth() - mapa.ret.x) )
        {
            linha = 0;
            coluna += TAMANHOBLOCOS;
        }
        l++;
    }
    linha = TAMANHOBLOCOS;
    coluna = TAMANHOBLOCOS;
    while( mapa.ret.y + coluna < (GetScreenHeight() - mapa.ret.y - TAMANHOBLOCOS) )
    {
        l = 0;
        while( l < NBLOCOS && (blocos[l].ret.x != mapa.ret.x + linha || blocos[l].ret.y != mapa.ret.y + coluna) )
        {
            l++;
        }
        if(l < NBLOCOS)
        {
            //printf("%4d", l);
            blocos[l].status = intransponivel;
            blocos[l-1].blockDir[dir] = 1;
            blocos[l+1].blockDir[esq] = 1;
            blocos[l+NLINHAS].blockDir[cima] = 1;
            blocos[l-NLINHAS].blockDir[baixo] = 1;
            //printf("%4d", l+1);
        }
        linha += TAMANHOBLOCOS*2;
        if( mapa.ret.x + linha >= (GetScreenWidth() - mapa.ret.x - TAMANHOBLOCOS) )
        {
            linha = TAMANHOBLOCOS;
            coluna += TAMANHOBLOCOS*2;
        }
    }
    for(l=0; l<NBLOCOS; l++)
    {
        if(l%NLINHAS == 0)
        {
            //printf("\n\n");
        }
        //printf("%4d=%d", l, contador);
    }
}

void desenhaBlocos(BLOCO blocos[], Rectangle formaTexturaExplosao, Texture2D texturaBloco, Texture2D texturaMuro, Texture2D texturaExplosao)
{
    int l;
    for(l = 0; l < NBLOCOS; l++)
    {
        if(blocos[l].status == explosao)
        {
            DrawTextureRec(texturaExplosao, formaTexturaExplosao, (Vector2) {blocos[l].ret.x, blocos[l].ret.y}, WHITE);
        }
        else if(blocos[l].status == quebravel)
        {
            DrawTextureRec(texturaMuro, formaTexturaExplosao, (Vector2) {blocos[l].ret.x, blocos[l].ret.y}, WHITE);
        }
        else if(blocos[l].status == intransponivel)
        {
            DrawTexture(texturaBloco, blocos[l].ret.x, blocos[l].ret.y, WHITE);
        }
        else if(blocos[l].portal == 1)
        {
            //DrawTexture(blocos[l].texturaPortal, blocos[l].ret.x, blocos[l].ret.y, WHITE);
            DrawRectangle(blocos[l].ret.x, blocos[l].ret.y, blocos[l].ret.width, blocos[l].ret.height, CORPORTAL);
        }
        //DrawText(TextFormat("%d", l), blocos[l].ret.x, blocos[l].ret.y, 20, CORINSTRUCAO);
    }
}

void inicializaBombas(BOMBA listaBombas[], int nbombas)
{
    int l;
    for(l = 0; l < nbombas; l++)
    {
        listaBombas[l].pos.x = POSIN_BOMBAS;
        listaBombas[l].pos.y = POSIN_BOMBAS;
        listaBombas[l].status = inerte;
    }
}

void geraBombas(BOMBA listaBombas[], int nbombas, BOMBERMAN bomberman, BLOCO blocos[])
{
    int l=0, c=0;
    while( l < nbombas && listaBombas[l].status != inerte )
    {
        l++;
    }
    if(l < nbombas)
    {
        while( c < NBLOCOS && (blocos[c].status != transponivel
                || !CheckCollisionRecs((Rectangle) {bomberman.ret.x + (bomberman.ret.width/2), bomberman.ret.y + (bomberman.ret.height/2), bomberman.ret.width/2, bomberman.ret.height/2}, blocos[c].ret)
                || blocos[c].bomba != INVERTE) )
        {
            c++;
        }
        if(c < NBLOCOS)
        {
            //printf("%d %f %f", c, blocos[c].ret.x, blocos[c].ret.x + (blocos[c].ret.x/2));
            listaBombas[l].status = ativada;
            listaBombas[l].pos.x = blocos[c].ret.x + (blocos[c].ret.width/2);
            listaBombas[l].pos.y = blocos[c].ret.y + (blocos[c].ret.height/2);
            blocos[c].bomba = l;
        }
    }
}

int explodeBomberman(BOMBERMAN *bomberman, BOMBA bomba, int nbomba, BLOCO blocos[])
{
    int l=0, c=0;
    while(l < NBLOCOS && blocos[l].bomba != nbomba)
    {
        l++;
    }
    if(l < NBLOCOS)
    {
        if(CheckCollisionRecs(blocos[l].ret, bomberman->ret))
        {
            return 1;
        }
        else{

            //alcance direita
            c=l+1;
            while( c >= 0 && c < NBLOCOS && c <= l+ALCANCE && blocos[c].status != intransponivel )
            {
                if(CheckCollisionRecs(blocos[c].ret, bomberman->ret))
                {
                    return 1;
                }
                c++;
            }

            //alcance esquerda
            c=l-1;
            while( c >= 0 && c < NBLOCOS && c >= l-ALCANCE && c%NLINHAS != 0 && blocos[c].status != intransponivel )
            {
                if(CheckCollisionRecs(blocos[c].ret, bomberman->ret))
                {
                    return 1;
                }
                c--;
            }

            //alcance baixo
            c=l+NLINHAS;
            while( c >= 0 && c < NBLOCOS && c <= l+(ALCANCE*NLINHAS) && c%NLINHAS != NLINHAS-1 && blocos[c].status != intransponivel )
            {
                if(CheckCollisionRecs(blocos[c].ret, bomberman->ret))
                {
                    return 1;
                }
                c += NLINHAS;
            }

            //alcance cima
            c=l-NLINHAS;
            while( c >= 0 && c < NBLOCOS && c >= l-(ALCANCE*NLINHAS) && blocos[c].status != intransponivel )
            {
                if(CheckCollisionRecs(blocos[c].ret, bomberman->ret))
                {
                    return 1;
                }
                c -= NLINHAS;
            }
        }
    }
    return 0;
}

void explodeInimigos(INIMIGO listaInimigos[], int ninimigos, BOMBA bomba, int nbomba, BLOCO blocos[])
{
    int l=0, i, c=0;
    while(l < NBLOCOS && blocos[l].bomba != nbomba)
    {
        l++;
    }
    if(l < NBLOCOS)
    {
        for(i = 0; i < ninimigos; i++)
        {
            if(listaInimigos[i].morto == 1 || CheckCollisionCircleRec(listaInimigos[i].pos, TAMANHOINIMIGO, blocos[l].ret))
            {
                listaInimigos[i].morto = 1;
            }
            else{

                //alcance direita
                c=l+1;
                while( c >= 0 && c < NBLOCOS && c <= l+ALCANCE && c%NLINHAS != 0 && blocos[c].status != intransponivel )
                {
                    if(CheckCollisionCircleRec(listaInimigos[i].pos, TAMANHOINIMIGO, blocos[c].ret))
                    {
                        listaInimigos[i].morto = 1;
                    }
                    c++;
                }

                //alcance esquerda
                c=l-1;
                while( c >= 0 && c < NBLOCOS && c >= l-ALCANCE && c%NLINHAS != NLINHAS-1 && blocos[c].status != intransponivel )
                {
                    if(CheckCollisionCircleRec(listaInimigos[i].pos, TAMANHOINIMIGO, blocos[c].ret))
                    {
                        listaInimigos[i].morto = 1;
                    }
                    c--;
                }

                //alcance baixo
                c=l+NLINHAS;
                while( c >= 0 && c < NBLOCOS && c <= l+(ALCANCE*NLINHAS) && blocos[c].status != intransponivel )
                {
                    if(CheckCollisionCircleRec(listaInimigos[i].pos, TAMANHOINIMIGO, blocos[c].ret))
                    {
                        listaInimigos[i].morto = 1;
                    }
                    c += NLINHAS;
                }

                //alcance cima
                c=l-NLINHAS;
                while( c >= 0 && c < NBLOCOS && c >= l-(ALCANCE*NLINHAS) && blocos[c].status != intransponivel )
                {
                    if(CheckCollisionCircleRec(listaInimigos[i].pos, TAMANHOINIMIGO, blocos[c].ret))
                    {
                        listaInimigos[i].morto = 1;
                    }
                    c -= NLINHAS;
                }
            }
        }
    }
}

void explodeMuros(BLOCO blocos[], BOMBA bomba, int nbomba)
{
    int l=0, c=0;
    while(l < NBLOCOS && blocos[l].bomba != nbomba)
    {
        l++;
    }
    if(l < NBLOCOS)
    {

        //alcance direita
        c=l+1;
        while( c >= 0 && c < NBLOCOS && c <= l+ALCANCE && c%NLINHAS != 0 && blocos[c].status != intransponivel )
        {
            if(blocos[c].status == quebravel)
            {
                blocos[c].status = transponivel;
            }
            c++;
        }

        //alcance esquerda
        c=l-1;
        while( c >= 0 && c < NBLOCOS && c >= l-ALCANCE && c%NLINHAS != NLINHAS-1 && blocos[c].status != intransponivel )
        {
            if(blocos[c].status == quebravel)
            {
                blocos[c].status = transponivel;
            }
            c--;
        }

        //alcance baixo
        c=l+NLINHAS;
        while( c >= 0 && c < NBLOCOS && c <= l+(ALCANCE*NLINHAS) && blocos[c].status != intransponivel )
        {
            if(blocos[c].status == quebravel)
            {
                blocos[c].status = transponivel;
            }
            c += NLINHAS;
        }

        //alcance cima
        c=l-NLINHAS;
        while( c >= 0 && c < NBLOCOS && c >= l-(ALCANCE*NLINHAS) && blocos[c].status != intransponivel )
        {
            if(blocos[c].status == quebravel)
            {
                blocos[c].status = transponivel;
            }
            c -= NLINHAS;
        }
    }
}

void animaExplosao(BOMBA bomba, int nbomba, BLOCO blocos[])
{
    int l=0, c=0;
    while(l < NBLOCOS && blocos[l].bomba != nbomba)
    {
        l++;
    }
    if(l < NBLOCOS)
    {
        blocos[l].status = explosao;
        blocos[l].contadorExplosao = FRAMES_EXPLOSAO;

        //alcance direita
        c=l+1;
        while( c >= 0 && c < NBLOCOS && c <= l+ALCANCE && c%NLINHAS != 0 && blocos[c].status != intransponivel )
        {
            blocos[c].status = explosao;
            blocos[c].contadorExplosao = FRAMES_EXPLOSAO;
            c++;
        }

        //alcance esquerda
        c=l-1;
        while( c >= 0 && c < NBLOCOS && c >= l-ALCANCE && c%NLINHAS != NLINHAS-1 && blocos[c].status != intransponivel )
        {
            blocos[c].status = explosao;
            blocos[c].contadorExplosao = FRAMES_EXPLOSAO;
            c--;
        }

        //alcance baixo
        c=l+NLINHAS;
        while( c >= 0 && c < NBLOCOS && c <= l+(ALCANCE*NLINHAS) && blocos[c].status != intransponivel )
        {
            blocos[c].status = explosao;
            blocos[c].contadorExplosao = FRAMES_EXPLOSAO;
            c += NLINHAS;
        }

        //alcance cima
        c=l-NLINHAS;
        while( c >= 0 && c < NBLOCOS && c >= l-(ALCANCE*NLINHAS) && blocos[c].status != intransponivel )
        {
            blocos[c].status = explosao;
            blocos[c].contadorExplosao = FRAMES_EXPLOSAO;
            c -= NLINHAS;
        }
    }
}

void explodeBombas(BOMBA listaBombas[], int nbombas, BOMBERMAN *bomberman, INIMIGO listaInimigos[], int ninimigos, BLOCO blocos[])
{
    int l;
    for(l = 0; l < nbombas; l++)
    {
        if(listaBombas[l].status == ativada)
        {
            animaExplosao(listaBombas[l], l, blocos);
            bomberman->morto = explodeBomberman(bomberman, listaBombas[l], l, blocos);
            explodeInimigos(listaInimigos, ninimigos, listaBombas[l], l, blocos);
            explodeMuros(blocos, listaBombas[l], l);

            listaBombas[l].pos.x = POSIN_BOMBAS;
            listaBombas[l].pos.y = POSIN_BOMBAS;
            listaBombas[l].status = inerte;
        }
    }
    for(l = 0; l < NBLOCOS; l++)
    {
        blocos[l].bomba = INVERTE;
    }
}

void geraMuros(int nmuros, BLOCO blocos[], MAPA mapa, BOMBERMAN bomberman)
{
    int l, pos_al, portal;
    for(l = 0; l <= nmuros; l++)
    {
        pos_al = GetRandomValue(0, NBLOCOS-1);
        while( blocos[pos_al].status != transponivel
              || CheckCollisionRecs( blocos[pos_al].ret,
                    (Rectangle) { bomberman.ret.x + (bomberman.ret.width/2) - (DIST_MIN_BOMBERMAN_MUROS/2),
                                    bomberman.ret.y + (bomberman.ret.height/2) - (DIST_MIN_BOMBERMAN_MUROS/2),
                                    DIST_MIN_BOMBERMAN_MUROS, DIST_MIN_BOMBERMAN_MUROS } ) )
        {
            pos_al = GetRandomValue(0, NBLOCOS-1);
        }
        blocos[pos_al].status = quebravel;
    }

    //gera o portal em algum dos muros
    portal = GetRandomValue(0, NBLOCOS-1);
    while( blocos[portal].status != quebravel )
    {
        portal = GetRandomValue(0, NBLOCOS-1);
    }
    blocos[portal].portal = 1;
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
    else {"Não foi possível abrir o arquivo";}
    fclose(arq);
}

void LerdoArquivo(SCORE tabela[])
{
    FILE *arq = fopen("score.txt","rb");
    if(arq)
    {
        fread(tabela, sizeof(SCORE), TABELASCORE, arq);
        fclose(arq);
        return 1;

    }
    else
    {
        printf("Erro ao abrir score");
        return 0;
    }
}

void OrdenaHighscore(SCORE tabela[])
{
    int i;
}

void Scorefinal(BOMBERMAN *player, SCORE tabela[])
{
    player->score.score = (player->score.score * player->fase) /GetFrameTime()*1000;
    tabela[TABELASCORE+1].score = player->score.score;
}



void apagaExplosao(BLOCO blocos[])
{
    int l;
    for(l = 0; l < NBLOCOS; l++)
    {
        if(blocos[l].status == explosao && blocos[l].contadorExplosao == 0)
        {
            blocos[l].status = transponivel;
            //printf("%d", blocos[l].contadorExplosao);
        }
        else{
            blocos[l].contadorExplosao--;
            //printf("%d", blocos[l].contadorExplosao);
        }
    }
}

int main(void)
{

    // Inicialização
    //--------------------------------------------------------------------------------------

        InitWindow(LARGURATELA, ALTURATELA, "Bomberman");
        SetTargetFPS(FPS);
        //ToggleFullscreen();

        //--------------------------------------------------------------------------------------

        //define o MAPA e seus dados
        MAPA mapa = {
            { ((GetScreenWidth() - LARGURAMAPA)/2), ((GetScreenHeight() - ALTURAMAPA)/2), LARGURAMAPA, ALTURAMAPA },
            CORMAPA
        };

        //--------------------------------------------------------------------------------------

        //define os blocos nao quebraveis do mapa e os transponiveis
        BLOCO blocos[NBLOCOS];
        geraBlocos(blocos, mapa);

        //textura dos blocos
        Texture2D texturaBloco = LoadTexture("bloco.png");

        //textura de explosao
        Texture2D texturaExplosao = LoadTexture("explosao.png");

        //Indica as formas da textura da explosao
        Rectangle formaTexturaExplosao = { 0, 0, TAMANHOBLOCOS, TAMANHOBLOCOS };

        //--------------------------------------------------------------------------------------

        //define o BOMBERMAN e seus dados
        int fase = 1;
        BOMBERMAN bomberman = { (Rectangle)
            {
                (GetScreenWidth()/2) - (LARGURABOMBERMAN/2) + TAMANHOBLOCOS,
                (GetScreenHeight()/2) - (ALTURABOMBERMAN/2),
                LARGURABOMBERMAN,
                ALTURABOMBERMAN
            },
            0,
            LoadTexture("bomberman.png"),
            fase, //fase inicial
            VIDAS, //vida inicial
            0,
            0,
        };

        //--------------------------------------------------------------------------------------

        //define os muros quebraveis do mapa
        geraMuros(NMUROS, blocos, mapa, bomberman);

        //textura dos muros
        Texture2D texturaMuro = LoadTexture("muro.png");

        //--------------------------------------------------------------------------------------

        //inicializa o vetor de INIMIGOS
        int l;
        INIMIGO listaInimigos[NINIMIGOS];

        //gera os INIMIGOS
        geraInimigos(listaInimigos, NINIMIGOS, mapa, blocos, bomberman);

        //--------------------------------------------------------------------------------------

        //inicializa o vetor de BOMBAS
        BOMBA listaBombas[NBOMBAS];

        //inicializa as BOMBAS
        inicializaBombas(listaBombas, NBOMBAS);

        //--------------------------------------------------------------------------------------

    //--------------------------------------------------------------------------------------

    // Loop de Jogo (continua enquanto o usuario nao clicar "ESC" e enquanto o bomberman nao morrer para um inimigo
    //--------------------------------------------------------------------------------------

        while (!WindowShouldClose() && !bomberman.morto)
        {

            // Atualização da tela do jogo
            //----------------------------------------------------------------------------------
                checkColisaoPlayerInimigo1(listaInimigos,NINIMIGOS,bomberman.ret, bomberman);

                //movimenta o Bomberman a partir do teclado
                movimentaBomberman(&bomberman, mapa, blocos, listaBombas, NBOMBAS);

                //Atualiza a posicao dos inimigos
                movimentaInimigos(listaInimigos, NINIMIGOS, mapa, blocos, listaBombas, NBOMBAS, bomberman);

                //gera as bombas
                if(IsKeyPressed(KEY_SPACE))
                {
                    geraBombas(listaBombas, NBOMBAS, bomberman, blocos);
                }

                //explode todas as bombas
                if(IsKeyDown(KEY_ENTER))
                {
                    explodeBombas(listaBombas, NBOMBAS, &bomberman, listaInimigos, NINIMIGOS, blocos);
                }

                //apaga a textura da explosao
                apagaExplosao(blocos);

            //----------------------------------------------------------------------------------

            // Desenho do jogo
            //----------------------------------------------------------------------------------

                BeginDrawing();

                    //Desenha fundo da tela (borda do mapa)
                    ClearBackground(CORFUNDO);

                    //Desenha o mapa
                    DrawRectangleRec(mapa.ret, mapa.cor);

                    //Desenha os blocos
                    desenhaBlocos(blocos, formaTexturaExplosao, texturaBloco, texturaMuro, texturaExplosao);

                    //Desenha o texto da parte superior da tela
                    DrawText("Mova o quadrado com as setas do seu teclado e fuja do inimigo", 10, 10, 20, CORINSTRUCAO);

                    //Desenha o bomberman
                    DrawTexture(bomberman.textura, bomberman.ret.x, bomberman.ret.y, WHITE);
                    //DrawRectangleRec(bomberman.ret, CORBOMBERMAN);

                    //Desenha os inimigos
                    for(l = 0; l < NINIMIGOS; l++)
                    {
                        if(listaInimigos[l].morto == 0)
                        {
                            DrawCircleV(listaInimigos[l].pos, TAMANHOINIMIGO, CORINIMIGO);
                        }
                    }

                    //Desenha as bombas
                    for(l = 0; l < NBOMBAS; l++)
                    {
                        if(listaBombas[l].status == ativada)
                        {
                            DrawCircleV(listaBombas[l].pos, TAMANHOBOMBA, CORBOMBA);
                        }
                    }

                EndDrawing();

            //----------------------------------------------------------------------------------


        }
    //--------------------------------------------------------------------------------------

    return 0;
}
