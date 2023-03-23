#include "raylib.h"
#include <stdio.h>
#include <string.h>

//LINHAS E COLUNAS
#define NLINHAS 27
#define NCOLUNAS 15

//BLOCOS
#define NBLOCOS NLINHAS * NCOLUNAS //((NLINHAS-1)/2) * ((NCOLUNAS-1)/2) + 1
#define TAMANHOBLOCOS 40
#define CORBLOCOS GRAY
#define NFORMAS_EXPLOSAO 7

//TELA GERAL
#define LARGURATELA GetMonitorWidth(0)
#define ALTURATELA GetMonitorHeight(0)
#define NOMEJANELA "Bomberman"

//TELA INICIAL
#define CORFUNDO_TI BLACK
#define TEXTO_TI "PRESSIONE [ENTER] PARA JOGAR"
#define TEXTO_CPYRGT "Feito por Bernardo C. Zietolie & Flavia O. da Silva"
#define TAM_TEXTO_TI 40
#define TAM_TEXTO_CPYRGT 20

//TELA DE CARREGAMENTO
#define MAX_STR_FASE 20
#define TAM_TEXTO_CARREG 80
#define TEXT_CARREG "FASE %d"

//TELA DE JOGO
#define CORFUNDO DARKGRAY
#define TAM_BORDA 10
#define COR_BORDA GRAY
#define TAM_TEXTO_MENU 20
#define TEXT_VIDAS "Vidas: %d"
#define TEXT_PAUSE "[P] Pause"
#define TEXT_ESC "[ESC] Sair"
#define ESPACAMENTO 30
#define CORMENU LIGHTGRAY

//TELA FINAL
#define TEXTO_FIM "FIM DE JOGO"

//MAPA
#define LARGURAMAPA NLINHAS*TAMANHOBLOCOS
#define ALTURAMAPA NCOLUNAS*TAMANHOBLOCOS
#define CORPADRAO_MAPA (Color){ 1, 149, 1, 255 }
#define NCORES 10

//BOMBERMAN
#define LARGURABOMBERMAN 26
#define ALTURABOMBERMAN 38
#define VELBOMBERMAN 1.5f
#define CORBOMBERMAN BROWN
#define NVIDAS 3
#define NOME 20

//INIMIGOS
#define TAMANHOINIMIGO 39
#define VELINIMIGO 1.0f
#define CORINIMIGO RED
#define NINIMIGOS 3
#define DIST_MIN_BOMBERMAN_INI 250
#define DIST_MIN_PERSEG TAMANHOBLOCOS*8
#define MIN_PASSOS 4

//BOMBAS
#define NBOMBAS 3
#define POSIN_BOMBAS INVERTE
#define TAMANHOBOMBA 19
#define CORBOMBA BLACK
#define ALCANCE 2
#define FRAMES_EXPLOSAO 20
#define COREXPLOSAO ORANGE

//MUROS
#define MIN_MUROS 10
#define MAX_MUROS 100 //LIMITE: 314
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
#define TAMLISTA 6
#define ORDENALISTA 5

typedef enum
{
    esc,
    morte,
    vitoria
} STATUSFASE;

typedef struct
{
    int n;
    int vidas;
    int ninimigos;
    int nbombas;
    int nmuros;
    Color cormapa;
} FASE;

typedef struct
{
    int score;
} SCORE;


typedef struct
{
    char nome[NOME];
    SCORE pontos;
} RANKING;

RANKING listaDeRanks[TAMLISTA];

typedef struct
{
    Rectangle ret;
    int morto;
    Texture2D textura;
    SCORE pontos;
} BOMBERMAN;

typedef struct
{
    Rectangle ret;
    Color cor;
} MAPA;

typedef enum
{
    esq,
    dir,
    cima,
    baixo
} DIRECAO;

typedef struct
{
    Rectangle ret;
    DIRECAO dir;
    Vector2 vel;
    int passos;
    int morto;
} INIMIGO;

typedef enum
{
    transponivel,
    intransponivel,
    quebravel,
    explosao
} STATUSBLOCO;

typedef struct
{
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

typedef struct
{
    Vector2 pos;
    STATUSBOMBA status;
} BOMBA;



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
        //impede que o inimigo nasca em lugares invalidos
        if( blocos[l].status != transponivel
                && CheckCollisionRecs((Rectangle)
    {
        posx_al - VELINIMIGO, posy_al - VELINIMIGO, TAMANHOINIMIGO + VELINIMIGO, TAMANHOINIMIGO + VELINIMIGO
    }, blocos[l].ret) )
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
        //gera uma direcao inicial aleatoria para o inimigo
        direcao_al = GetRandomValue(0, NDIRECOES-1);
        listaInimigos[l].dir = direcao_al;
        geraVelInimigo(&listaInimigos[l]);

        //gera uma velocidade inicial aleatoria para o inimigo
        posx_al = GetRandomValue(mapa.ret.x, (GetScreenWidth() - mapa.ret.x - TAMANHOINIMIGO));
        posy_al = GetRandomValue(mapa.ret.y, (GetScreenHeight() - mapa.ret.y - TAMANHOINIMIGO));

        //verifica se a posicao e valida e fica longe do bomberman
        while( verficaPosInimigoBlocos(blocos, posx_al, posy_al)
                || CheckCollisionRecs( (Rectangle)
    {
        posx_al, posy_al, TAMANHOINIMIGO, TAMANHOINIMIGO
    },
    (Rectangle)
    {
        bomberman.ret.x + (bomberman.ret.width/2) - (DIST_MIN_BOMBERMAN_INI/2),
                            bomberman.ret.y + (bomberman.ret.height/2) - (DIST_MIN_BOMBERMAN_INI/2),
                            DIST_MIN_BOMBERMAN_INI, DIST_MIN_BOMBERMAN_INI
        } ) )
        {
            posx_al = GetRandomValue(mapa.ret.x, (GetScreenWidth() - mapa.ret.x - TAMANHOINIMIGO));
            posy_al = GetRandomValue(mapa.ret.y, (GetScreenHeight() - mapa.ret.y - TAMANHOINIMIGO));
        }

        //inicia os valores do inimigo
        listaInimigos[l].ret.x = posx_al;
        listaInimigos[l].ret.y = posy_al;
        listaInimigos[l].ret.width = TAMANHOINIMIGO;
        listaInimigos[l].ret.height = TAMANHOINIMIGO;
        listaInimigos[l].passos = 0;
        listaInimigos[l].morto = 0;
    }
}

void checaColisaoBombermanInimigos(INIMIGO listaInimigos[], int ninimigos, BOMBERMAN *bomberman)
{
    int l = 0;
    while( l < ninimigos && (listaInimigos[l].morto == 1 || !CheckCollisionRecs(listaInimigos[l].ret, bomberman->ret)) )
    {
        l++;
    }
    if(l < ninimigos)
    {
        bomberman->morto = 1;
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
            if( CheckCollisionRecs((Rectangle)
        {
            bomberman.ret.x + VELBOMBERMAN, bomberman.ret.y, bomberman.ret.width, bomberman.ret.height
        }, blocos[l].ret) )
            {
                blockDirec[dir] = 1;
            }

            //Checando colisao com bloco a esquerda do Bomberman
            if( CheckCollisionRecs((Rectangle)
        {
            bomberman.ret.x - VELBOMBERMAN, bomberman.ret.y, bomberman.ret.width, bomberman.ret.height
        }, blocos[l].ret) )
            {
                blockDirec[esq] = 1;
            }

            //Checando colisao com bloco acima do Bomberman
            if( CheckCollisionRecs((Rectangle)
        {
            bomberman.ret.x, bomberman.ret.y - VELBOMBERMAN, bomberman.ret.width, bomberman.ret.height
        }, blocos[l].ret) )
            {
                blockDirec[cima] = 1;
            }

            //Checando colisao com bloco abaixo do Bomberman
            if( CheckCollisionRecs((Rectangle)
        {
            bomberman.ret.x, bomberman.ret.y + VELBOMBERMAN, bomberman.ret.width, bomberman.ret.height
        }, blocos[l].ret) )
            {
                blockDirec[baixo] = 1;
            }
        }
        l++;
    }
    //Retorna 1 caso a direcao escolhida esteja bloqueada por um bloco
    if(blockDirec[direcao] == 1)
    {
        return 1;
    }
    return 0;

}

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
    int l=0, c, blockDirec[NDIRECOES] = {};
    while( l < NBLOCOS && blockDirec[direcao] == 0)
    {
        if(CheckCollisionRecs(inimigo.ret, blocos[l].ret))
        {
            for(c = 0; c < NDIRECOES; c++)
            {
                //pega as direcoes bloqueadas indicadas pelo bloco em questao
                if(blocos[l].blockDir[c] == 1)
                {
                    blockDirec[c] = 1;
                }
            }
        }
        l++;
    }
    //printf("bloqueado");
    for(c=0; c<NBLOCOS; c++)
    {
        if(CheckCollisionRecs(inimigo.ret, blocos[c].ret))
        {
            //printf("\n c=%4d\nposinx=%4f\nposretx=%4f\nposiny=%4f\nposrety=%4f", c, inimigo.ret.x, blocos[c].ret.x, inimigo.ret.y, blocos[c].ret.y);
        }
    }

    //Retorna 1 caso a direcao escolhida nao possa ser utilizada naquela linha/coluna
    if(blockDirec[direcao] == 1)
    {
        //printf("\nl = %d; direcao = %d", l-1, direcao);
        return 1;
    }
    return 0;
}

int checaColisaoInimigoBombas(BOMBA bombas[], int nbombas, INIMIGO inimigo, int direcao)
{
    int l = 0;
    while(l < nbombas)
    {
        if(bombas[l].status == ativada)
        {
            //printf("bomba");
            //Checando colisao com bomba a direita do inimigo
            if( direcao == dir && CheckCollisionCircleRec(bombas[l].pos, TAMANHOBOMBA, (Rectangle)
        {
            inimigo.ret.x + VELINIMIGO, inimigo.ret.y, inimigo.ret.width, inimigo.ret.height
        }) )
            {
                return 1;
            }

            //Checando colisao com bomba a esquerda do inimigo
            if( direcao == esq && CheckCollisionCircleRec(bombas[l].pos, TAMANHOBOMBA, (Rectangle)
        {
            inimigo.ret.x - VELINIMIGO, inimigo.ret.y, inimigo.ret.width, inimigo.ret.height
        }) )
            {
                return 1;
            }

            //Checando colisao com bomba acima do inimigo
            if( direcao == cima && CheckCollisionCircleRec(bombas[l].pos, TAMANHOBOMBA, (Rectangle)
        {
            inimigo.ret.x, inimigo.ret.y - VELINIMIGO, inimigo.ret.width, inimigo.ret.height
        }) )
            {
                return 1;
            }

            //Checando colisao com bomba abaixo do inimigo
            if( direcao == baixo && CheckCollisionCircleRec(bombas[l].pos, TAMANHOBOMBA, (Rectangle)
        {
            inimigo.ret.x, inimigo.ret.y + VELINIMIGO, inimigo.ret.width, inimigo.ret.height
        }) )
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
            if( direcao == dir && CheckCollisionRecs((Rectangle)
        {
            inimigo.ret.x + VELINIMIGO, inimigo.ret.y, inimigo.ret.width, inimigo.ret.height
        }, blocos[l].ret) )
            {
                return 1;
            }

            //Checando colisao com bomba a esquerda do inimigo
            if( direcao == esq && CheckCollisionRecs((Rectangle)
        {
            inimigo.ret.x - VELINIMIGO, inimigo.ret.y, inimigo.ret.width, inimigo.ret.height
        }, blocos[l].ret) )
            {
                return 1;
            }

            //Checando colisao com bomba acima do inimigo
            if( direcao == cima && CheckCollisionRecs((Rectangle)
        {
            inimigo.ret.x, inimigo.ret.y - VELINIMIGO, inimigo.ret.width, inimigo.ret.height
        }, blocos[l].ret) )
            {
                return 1;
            }

            //Checando colisao com bomba abaixo do inimigo
            if( direcao == baixo && CheckCollisionRecs((Rectangle)
        {
            inimigo.ret.x, inimigo.ret.y + VELINIMIGO, inimigo.ret.width, inimigo.ret.height
        }, blocos[l].ret) )
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
    if(CheckCollisionRecs( inimigo.ret, (Rectangle)
{
    bomberman.ret.x + (bomberman.ret.width/2) - (DIST_MIN_PERSEG/2),
                        bomberman.ret.y + (bomberman.ret.height/2) - (DIST_MIN_PERSEG/2),
                        DIST_MIN_PERSEG, DIST_MIN_PERSEG
    } ))
    {
        //verifica se o bomberman e o inimigo estao na mesma linha vertical
        if( (inimigo.ret.x >= bomberman.ret.x && inimigo.ret.x <= bomberman.ret.x + bomberman.ret.height)
                || (inimigo.ret.x + TAMANHOINIMIGO >= bomberman.ret.x && inimigo.ret.x + TAMANHOINIMIGO <= bomberman.ret.x + bomberman.ret.height) )
        {
            if(inimigo.ret.y >= bomberman.ret.y )
            {
                //printf("cima");
                return cima;
            }
            else
            {
                //printf("baixo");
                return baixo;
            }
        }
        else if( (inimigo.ret.y >= bomberman.ret.y && inimigo.ret.y <= bomberman.ret.y + bomberman.ret.width)
                 || (inimigo.ret.y + TAMANHOINIMIGO >= bomberman.ret.y && inimigo.ret.y + TAMANHOINIMIGO <= bomberman.ret.y + bomberman.ret.width) )
        {
            if(inimigo.ret.x >= bomberman.ret.x )
            {
                //printf("esq");
                return esq;
            }
            else
            {
                //printf("dir");
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

int fogeBombas(INIMIGO inimigo, BOMBA bombas[], int nbombas, BLOCO blocos[], int direcao)
{
    int i=0, l=0, c, blockDirec[NDIRECOES] = {};
    while( i < nbombas && blockDirec[direcao] == 0)
    {
        l=0;
        while( l < NBLOCOS && blockDirec[direcao] == 0)
        {
            if(blocos[l].bomba == i)
            {
                //alcance direita
                c=l+1;
                while( c >= 0 && c < NBLOCOS && c <= l+ALCANCE && c%NLINHAS != 0 && blocos[c].status != intransponivel )
                {
                    if(CheckCollisionRecs((Rectangle)
                {
                    inimigo.ret.x + VELINIMIGO, inimigo.ret.y, inimigo.ret.width, inimigo.ret.height
                }, blocos[c].ret))
                    {
                        blockDirec[dir] = 1;
                    }
                    if(CheckCollisionRecs((Rectangle)
                {
                    inimigo.ret.x - VELINIMIGO, inimigo.ret.y, inimigo.ret.width, inimigo.ret.height
                }, blocos[c].ret))
                    {
                        blockDirec[esq] = 1;
                    }
                    if(CheckCollisionRecs((Rectangle)
                {
                    inimigo.ret.x, inimigo.ret.y - VELINIMIGO, inimigo.ret.width, inimigo.ret.height
                }, blocos[c].ret))
                    {
                        blockDirec[cima] = 1;
                    }
                    if(CheckCollisionRecs((Rectangle)
                {
                    inimigo.ret.x, inimigo.ret.y + VELINIMIGO, inimigo.ret.width, inimigo.ret.height
                }, blocos[c].ret))
                    {
                        blockDirec[baixo] = 1;
                    }
                    c++;
                }

                //alcance esquerda
                c=l-1;
                while( c >= 0 && c < NBLOCOS && c >= l-ALCANCE && c%NLINHAS != NLINHAS-1 && blocos[c].status != intransponivel )
                {
                    if(CheckCollisionRecs((Rectangle)
                {
                    inimigo.ret.x + VELINIMIGO, inimigo.ret.y, inimigo.ret.width, inimigo.ret.height
                }, blocos[c].ret))
                    {
                        blockDirec[dir] = 1;
                    }
                    if(CheckCollisionRecs((Rectangle)
                {
                    inimigo.ret.x - VELINIMIGO, inimigo.ret.y, inimigo.ret.width, inimigo.ret.height
                }, blocos[c].ret))
                    {
                        blockDirec[esq] = 1;
                    }
                    if(CheckCollisionRecs((Rectangle)
                {
                    inimigo.ret.x, inimigo.ret.y - VELINIMIGO, inimigo.ret.width, inimigo.ret.height
                }, blocos[c].ret))
                    {
                        blockDirec[cima] = 1;
                    }
                    if(CheckCollisionRecs((Rectangle)
                {
                    inimigo.ret.x, inimigo.ret.y + VELINIMIGO, inimigo.ret.width, inimigo.ret.height
                }, blocos[c].ret))
                    {
                        blockDirec[baixo] = 1;
                    }
                    c--;
                }

                //alcance baixo
                c=l+NLINHAS;
                while( c >= 0 && c < NBLOCOS && c <= l+(ALCANCE*NLINHAS) && blocos[c].status != intransponivel )
                {
                    if(CheckCollisionRecs((Rectangle)
                {
                    inimigo.ret.x + VELINIMIGO, inimigo.ret.y, inimigo.ret.width, inimigo.ret.height
                }, blocos[c].ret))
                    {
                        blockDirec[dir] = 1;
                    }
                    if(CheckCollisionRecs((Rectangle)
                {
                    inimigo.ret.x - VELINIMIGO, inimigo.ret.y, inimigo.ret.width, inimigo.ret.height
                }, blocos[c].ret))
                    {
                        blockDirec[esq] = 1;
                    }
                    if(CheckCollisionRecs((Rectangle)
                {
                    inimigo.ret.x, inimigo.ret.y - VELINIMIGO, inimigo.ret.width, inimigo.ret.height
                }, blocos[c].ret))
                    {
                        blockDirec[cima] = 1;
                    }
                    if(CheckCollisionRecs((Rectangle)
                {
                    inimigo.ret.x, inimigo.ret.y + VELINIMIGO, inimigo.ret.width, inimigo.ret.height
                }, blocos[c].ret))
                    {
                        blockDirec[baixo] = 1;
                    }
                    c += NLINHAS;
                }

                //alcance cima
                c=l-NLINHAS;
                while( c >= 0 && c < NBLOCOS && c >= l-(ALCANCE*NLINHAS) && blocos[c].status != intransponivel )
                {
                    if(CheckCollisionRecs((Rectangle)
                {
                    inimigo.ret.x + VELINIMIGO, inimigo.ret.y, inimigo.ret.width, inimigo.ret.height
                }, blocos[c].ret))
                    {
                        blockDirec[dir] = 1;
                    }
                    if(CheckCollisionRecs((Rectangle)
                {
                    inimigo.ret.x - VELINIMIGO, inimigo.ret.y, inimigo.ret.width, inimigo.ret.height
                }, blocos[c].ret))
                    {
                        blockDirec[esq] = 1;
                    }
                    if(CheckCollisionRecs((Rectangle)
                {
                    inimigo.ret.x, inimigo.ret.y - VELINIMIGO, inimigo.ret.width, inimigo.ret.height
                }, blocos[c].ret))
                    {
                        blockDirec[cima] = 1;
                    }
                    if(CheckCollisionRecs((Rectangle)
                {
                    inimigo.ret.x, inimigo.ret.y + VELINIMIGO, inimigo.ret.width, inimigo.ret.height
                }, blocos[c].ret))
                    {
                        blockDirec[baixo] = 1;
                    }
                    c -= NLINHAS;
                }
            }
            l++;
        }
        i++;
    }

    //Retorna 1 caso tenha uma bomba na direcao escolhida
    if(blockDirec[direcao] == 1)
    {
        //printf("bbbbb");
        return 1;
    }
    return 0;
}

int fogeBlocos(INIMIGO inimigo, BLOCO blocos[], int direcao)
{
    int l=0, c, blockDirec[NDIRECOES] = {};
    while( l < NBLOCOS && blockDirec[direcao] == 0)
    {
        if(blocos[l].status == quebravel)
        {
            //alcance direita
            c=l+1;
            while( c >= 0 && c < NBLOCOS && c <= l+1 && c%NLINHAS != 0 && blocos[c].status != intransponivel )
            {
                if(CheckCollisionRecs((Rectangle)
            {
                inimigo.ret.x + VELINIMIGO, inimigo.ret.y, inimigo.ret.width, inimigo.ret.height
            }, blocos[c].ret))
                {
                    blockDirec[dir] = 1;
                }
                if(CheckCollisionRecs((Rectangle)
            {
                inimigo.ret.x - VELINIMIGO, inimigo.ret.y, inimigo.ret.width, inimigo.ret.height
            }, blocos[c].ret))
                {
                    blockDirec[esq] = 1;
                }
                if(CheckCollisionRecs((Rectangle)
            {
                inimigo.ret.x, inimigo.ret.y - VELINIMIGO, inimigo.ret.width, inimigo.ret.height
            }, blocos[c].ret))
                {
                    blockDirec[cima] = 1;
                }
                if(CheckCollisionRecs((Rectangle)
            {
                inimigo.ret.x, inimigo.ret.y + VELINIMIGO, inimigo.ret.width, inimigo.ret.height
            }, blocos[c].ret))
                {
                    blockDirec[baixo] = 1;
                }
                c++;
            }

            //alcance esquerda
            c=l-1;
            while( c >= 0 && c < NBLOCOS && c >= l-1 && c%NLINHAS != NLINHAS-1 && blocos[c].status != intransponivel )
            {
                if(CheckCollisionRecs((Rectangle)
            {
                inimigo.ret.x + VELINIMIGO, inimigo.ret.y, inimigo.ret.width, inimigo.ret.height
            }, blocos[c].ret))
                {
                    blockDirec[dir] = 1;
                }
                if(CheckCollisionRecs((Rectangle)
            {
                inimigo.ret.x - VELINIMIGO, inimigo.ret.y, inimigo.ret.width, inimigo.ret.height
            }, blocos[c].ret))
                {
                    blockDirec[esq] = 1;
                }
                if(CheckCollisionRecs((Rectangle)
            {
                inimigo.ret.x, inimigo.ret.y - VELINIMIGO, inimigo.ret.width, inimigo.ret.height
            }, blocos[c].ret))
                {
                    blockDirec[cima] = 1;
                }
                if(CheckCollisionRecs((Rectangle)
            {
                inimigo.ret.x, inimigo.ret.y + VELINIMIGO, inimigo.ret.width, inimigo.ret.height
            }, blocos[c].ret))
                {
                    blockDirec[baixo] = 1;
                }
                c--;
            }

            //alcance baixo
            c=l+NLINHAS;
            while( c >= 0 && c < NBLOCOS && c <= l+NLINHAS && blocos[c].status != intransponivel )
            {
                if(CheckCollisionRecs((Rectangle)
            {
                inimigo.ret.x + VELINIMIGO, inimigo.ret.y, inimigo.ret.width, inimigo.ret.height
            }, blocos[c].ret))
                {
                    blockDirec[dir] = 1;
                }
                if(CheckCollisionRecs((Rectangle)
            {
                inimigo.ret.x - VELINIMIGO, inimigo.ret.y, inimigo.ret.width, inimigo.ret.height
            }, blocos[c].ret))
                {
                    blockDirec[esq] = 1;
                }
                if(CheckCollisionRecs((Rectangle)
            {
                inimigo.ret.x, inimigo.ret.y - VELINIMIGO, inimigo.ret.width, inimigo.ret.height
            }, blocos[c].ret))
                {
                    blockDirec[cima] = 1;
                }
                if(CheckCollisionRecs((Rectangle)
            {
                inimigo.ret.x, inimigo.ret.y + VELINIMIGO, inimigo.ret.width, inimigo.ret.height
            }, blocos[c].ret))
                {
                    blockDirec[baixo] = 1;
                }
                c += NLINHAS;
            }

            //alcance cima
            c=l-NLINHAS;
            while( c >= 0 && c < NBLOCOS && c >= l-NLINHAS && blocos[c].status != intransponivel )
            {
                if(CheckCollisionRecs((Rectangle)
            {
                inimigo.ret.x + VELINIMIGO, inimigo.ret.y, inimigo.ret.width, inimigo.ret.height
            }, blocos[c].ret))
                {
                    blockDirec[dir] = 1;
                }
                if(CheckCollisionRecs((Rectangle)
            {
                inimigo.ret.x - VELINIMIGO, inimigo.ret.y, inimigo.ret.width, inimigo.ret.height
            }, blocos[c].ret))
                {
                    blockDirec[esq] = 1;
                }
                if(CheckCollisionRecs((Rectangle)
            {
                inimigo.ret.x, inimigo.ret.y - VELINIMIGO, inimigo.ret.width, inimigo.ret.height
            }, blocos[c].ret))
                {
                    blockDirec[cima] = 1;
                }
                if(CheckCollisionRecs((Rectangle)
            {
                inimigo.ret.x, inimigo.ret.y + VELINIMIGO, inimigo.ret.width, inimigo.ret.height
            }, blocos[c].ret))
                {
                    blockDirec[baixo] = 1;
                }
                c -= NLINHAS;
            }
        }
        l++;
    }

    //Retorna 1 caso tenha uma bomba na direcao escolhida
    if(blockDirec[direcao] == 1)
    {
        //printf("ccccccccc");
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
            if( (listaInimigos[l].ret.x + TAMANHOINIMIGO + VELINIMIGO) >= (GetScreenWidth() - mapa.ret.x)
                    || verificaLinhaColunaValidas(blocos, listaInimigos[l], dir)
                    || checaColisaoInimigoBombas(bombas, nbombas, listaInimigos[l], dir)
                    || checaColisaoInimigoMuros(blocos, listaInimigos[l], dir) )
            {
                blockDirec[dir] = 1;
            }

            //Checando colisao na borda da esquerda
            if( (listaInimigos[l].ret.x - VELINIMIGO) <= mapa.ret.x
                    || verificaLinhaColunaValidas(blocos, listaInimigos[l], esq)
                    || checaColisaoInimigoBombas(bombas, nbombas, listaInimigos[l], esq)
                    || checaColisaoInimigoMuros(blocos, listaInimigos[l], esq) )
            {
                blockDirec[esq] = 1;
            }

            //Checando colisao na borda superior
            if( (listaInimigos[l].ret.y - VELINIMIGO) <= mapa.ret.y
                    || verificaLinhaColunaValidas(blocos, listaInimigos[l], cima)
                    || checaColisaoInimigoBombas(bombas, nbombas, listaInimigos[l], cima)
                    || checaColisaoInimigoMuros(blocos, listaInimigos[l], cima) )
            {
                blockDirec[cima] = 1;
            }

            //Andar para baixo
            if( (listaInimigos[l].ret.y + TAMANHOINIMIGO + VELINIMIGO) >= (GetScreenHeight() - mapa.ret.y)
                    || verificaLinhaColunaValidas(blocos, listaInimigos[l], baixo)
                    || checaColisaoInimigoBombas(bombas, nbombas, listaInimigos[l], baixo)
                    || checaColisaoInimigoMuros(blocos, listaInimigos[l], baixo) )
            {
                blockDirec[baixo] = 1;
            }

            //gera uma nova direcao para perseguir o bomberman
            direcao_al = persegueBomberman(listaInimigos[l], bomberman, blocos);

            if( direcao_al != listaInimigos[l].dir && blockDirec[direcao_al] == 0
                    && !fogeBombas(listaInimigos[l], bombas, nbombas, blocos, direcao_al)
                    && !fogeBlocos(listaInimigos[l], blocos, direcao_al) )
            {
                listaInimigos[l].dir = direcao_al;
                geraVelInimigo(&listaInimigos[l]);
            }

            //Gera nova direcao aleatoria caso tenha colidido com uma parede ou tenha a oportunidade de ir para 3 ou mais direcoes
            else if( blockDirec[listaInimigos[l].dir] == 1 || (contaDesblock(blockDirec) >= NDIRECOES-1 && listaInimigos[l].passos >= MIN_PASSOS) )
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
                listaInimigos[l].passos = 0;
            }

            //Movimenta o inimigo
            listaInimigos[l].ret.x += listaInimigos[l].vel.x;
            listaInimigos[l].ret.y += listaInimigos[l].vel.y;
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

void desenhaBlocos(BLOCO blocos[], Rectangle formaTexturaExplosao, Texture2D texturaBloco, Texture2D texturaMuro, Texture2D texturaExplosao, Texture2D texturaPortal, Texture2D texturaMapa, FASE fase)
{
    int l;
    for(l = 0; l < NBLOCOS; l++)
    {
        if(blocos[l].status == explosao)
        {
            DrawTextureRec(texturaExplosao, formaTexturaExplosao, (Vector2)
            {
                blocos[l].ret.x, blocos[l].ret.y
            }, WHITE);
        }
        else if(blocos[l].status == quebravel)
        {
            DrawTexture(texturaMuro, blocos[l].ret.x, blocos[l].ret.y, WHITE);
        }
        else if(blocos[l].status == intransponivel)
        {
            DrawTexture(texturaBloco, blocos[l].ret.x, blocos[l].ret.y, WHITE);
        }
        else if(blocos[l].portal == 1)
        {
            DrawTexture(texturaPortal, blocos[l].ret.x, blocos[l].ret.y, WHITE);
        }
        else if(blocos[l].status == transponivel)
        {
            DrawTexture(texturaMapa, blocos[l].ret.x, blocos[l].ret.y, fase.cormapa);
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

int checaInimigosNoBloco(INIMIGO inimigos[], int ninimigos, BLOCO bloco)
{
    int l=0;
    while(l < ninimigos)
    {
        if(inimigos[l].morto == 0 && CheckCollisionRecs(inimigos[l].ret, bloco.ret))
        {
            return 1;
        }
        l++;
    }
    return 0;
}

void geraBombas(BOMBA listaBombas[], int nbombas, BOMBERMAN bomberman, BLOCO blocos[], INIMIGO inimigos[], int ninimigos)
{
    int l=0, c=0;
    while( l < nbombas && listaBombas[l].status != inerte )
    {
        l++;
    }
    if(l < nbombas)
    {
        while( c < NBLOCOS && (blocos[c].status != transponivel
                               || !CheckCollisionRecs((Rectangle)
    {
        bomberman.ret.x + (bomberman.ret.width/2), bomberman.ret.y + (bomberman.ret.height/2), bomberman.ret.width/2, bomberman.ret.height/2
        }, blocos[c].ret)
        || blocos[c].bomba != INVERTE
        || checaInimigosNoBloco(inimigos, ninimigos, blocos[c])) )
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
        else
        {

            //alcance direita
            c=l+1;
            while( c >= 0 && c < NBLOCOS && c <= l+ALCANCE && blocos[c].status != intransponivel  && blocos[c].status != quebravel)
            {
                if(CheckCollisionRecs(blocos[c].ret, bomberman->ret))
                {
                    return 1;
                }
                c++;
            }

            //alcance esquerda
            c=l-1;
            while( c >= 0 && c < NBLOCOS && c >= l-ALCANCE && c%NLINHAS != 0 && blocos[c].status != intransponivel  && blocos[c].status != quebravel)
            {
                if(CheckCollisionRecs(blocos[c].ret, bomberman->ret))
                {
                    return 1;
                }
                c--;
            }

            //alcance baixo
            c=l+NLINHAS;
            while( c >= 0 && c < NBLOCOS && c <= l+(ALCANCE*NLINHAS) && c%NLINHAS != NLINHAS-1 && blocos[c].status != intransponivel  && blocos[c].status != quebravel)
            {
                if(CheckCollisionRecs(blocos[c].ret, bomberman->ret))
                {
                    return 1;
                }
                c += NLINHAS;
            }

            //alcance cima
            c=l-NLINHAS;
            while( c >= 0 && c < NBLOCOS && c >= l-(ALCANCE*NLINHAS) && blocos[c].status != intransponivel  && blocos[c].status != quebravel)
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
            if(listaInimigos[i].morto == 1 || CheckCollisionRecs(listaInimigos[i].ret, blocos[l].ret))
            {
                listaInimigos[i].morto = 1;
            }
            else
            {

                //alcance direita
                c=l+1;
                while( c >= 0 && c < NBLOCOS && c <= l+ALCANCE && c%NLINHAS != 0 && blocos[c].status != intransponivel && blocos[c].status != quebravel )
                {
                    if(CheckCollisionRecs(listaInimigos[i].ret, blocos[c].ret))
                    {
                        listaInimigos[i].morto = 1;
                    }
                    c++;
                }

                //alcance esquerda
                c=l-1;
                while( c >= 0 && c < NBLOCOS && c >= l-ALCANCE && c%NLINHAS != NLINHAS-1 && blocos[c].status != intransponivel && blocos[c].status != quebravel )
                {
                    if(CheckCollisionRecs(listaInimigos[i].ret, blocos[c].ret))
                    {
                        listaInimigos[i].morto = 1;
                    }
                    c--;
                }

                //alcance baixo
                c=l+NLINHAS;
                while( c >= 0 && c < NBLOCOS && c <= l+(ALCANCE*NLINHAS) && blocos[c].status != intransponivel && blocos[c].status != quebravel )
                {
                    if(CheckCollisionRecs(listaInimigos[i].ret, blocos[c].ret))
                    {
                        listaInimigos[i].morto = 1;
                    }
                    c += NLINHAS;
                }

                //alcance cima
                c=l-NLINHAS;
                while( c >= 0 && c < NBLOCOS && c >= l-(ALCANCE*NLINHAS) && blocos[c].status != intransponivel && blocos[c].status != quebravel )
                {
                    if(CheckCollisionRecs(listaInimigos[i].ret, blocos[c].ret))
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
    int l=0, c=0, continuar = 1;
    while(l < NBLOCOS && blocos[l].bomba != nbomba)
    {
        l++;
    }
    if(l < NBLOCOS)
    {

        //alcance direita
        c=l+1;
        while( c >= 0 && c < NBLOCOS && c <= l+ALCANCE && c%NLINHAS != 0 && blocos[c].status != intransponivel && continuar )
        {
            if(blocos[c].status == quebravel)
            {
                blocos[c].status = transponivel;
                continuar = 0;
            }
            c++;
        }

        //alcance esquerda
        continuar = 1;
        c=l-1;
        while( c >= 0 && c < NBLOCOS && c >= l-ALCANCE && c%NLINHAS != NLINHAS-1 && blocos[c].status != intransponivel && continuar )
        {
            if(blocos[c].status == quebravel)
            {
                blocos[c].status = transponivel;
                continuar = 0;
            }
            c--;
        }

        //alcance baixo
        continuar = 1;
        c=l+NLINHAS;
        while( c >= 0 && c < NBLOCOS && c <= l+(ALCANCE*NLINHAS) && blocos[c].status != intransponivel && continuar )
        {
            if(blocos[c].status == quebravel)
            {
                blocos[c].status = transponivel;
                continuar = 0;
            }
            c += NLINHAS;
        }

        //alcance cima
        continuar = 1;
        c=l-NLINHAS;
        while( c >= 0 && c < NBLOCOS && c >= l-(ALCANCE*NLINHAS) && blocos[c].status != intransponivel && continuar )
        {
            if(blocos[c].status == quebravel)
            {
                blocos[c].status = transponivel;
                continuar = 0;
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
        while( c >= 0 && c < NBLOCOS && c <= l+ALCANCE && c%NLINHAS != 0 && blocos[c].status != intransponivel && blocos[c].status != quebravel )
        {
            blocos[c].status = explosao;
            blocos[c].contadorExplosao = FRAMES_EXPLOSAO;
            c++;
        }

        //alcance esquerda
        c=l-1;
        while( c >= 0 && c < NBLOCOS && c >= l-ALCANCE && c%NLINHAS != NLINHAS-1 && blocos[c].status != intransponivel && blocos[c].status != quebravel )
        {
            blocos[c].status = explosao;
            blocos[c].contadorExplosao = FRAMES_EXPLOSAO;
            c--;
        }

        //alcance baixo
        c=l+NLINHAS;
        while( c >= 0 && c < NBLOCOS && c <= l+(ALCANCE*NLINHAS) && blocos[c].status != intransponivel && blocos[c].status != quebravel )
        {
            blocos[c].status = explosao;
            blocos[c].contadorExplosao = FRAMES_EXPLOSAO;
            c += NLINHAS;
        }

        //alcance cima
        c=l-NLINHAS;
        while( c >= 0 && c < NBLOCOS && c >= l-(ALCANCE*NLINHAS) && blocos[c].status != intransponivel && blocos[c].status != quebravel )
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
                                       (Rectangle)
    {
        bomberman.ret.x + (bomberman.ret.width/2) - (DIST_MIN_BOMBERMAN_MUROS/2),
                            bomberman.ret.y + (bomberman.ret.height/2) - (DIST_MIN_BOMBERMAN_MUROS/2),
                            DIST_MIN_BOMBERMAN_MUROS, DIST_MIN_BOMBERMAN_MUROS
        } ) )
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
        else
        {
            blocos[l].contadorExplosao--;
            //printf("%d", blocos[l].contadorExplosao);
        }
    }
}

int contaInimigosVivos(INIMIGO inimigos[], int ninimigos)
{
    int l, contador = 0;
    for(l = 0; l < ninimigos; l++)
    {
        if(inimigos[l].morto == 0)
        {
            contador++;
        }
    }
    return contador;
}

int checaColisaoBombermanPortal(BOMBERMAN bomberman, BLOCO blocos[])
{
    int l = 0;
    while( l < NBLOCOS )
    {
        if( blocos[l].status == transponivel && blocos[l].portal == 1 && CheckCollisionRecs(bomberman.ret, blocos[l].ret) )
        {
            return 1;
        }
        l++;
    }
    return 0;
}

void geraFase(FASE *fase, Color coresMapa[])
{
    Color novaCor;
    fase->n++;
    fase->ninimigos = NINIMIGOS + fase->n-1;
    fase->nbombas = NBOMBAS + fase->n-1;
    fase->nmuros = GetRandomValue(MIN_MUROS, MAX_MUROS);
    if(fase->n != 1)
    {
        novaCor = coresMapa[GetRandomValue(0, NCORES)];
        while(novaCor.r == fase->cormapa.r && novaCor.g == fase->cormapa.g && novaCor.b == fase->cormapa.b)
        {
            novaCor = coresMapa[GetRandomValue(0, NCORES-1)];
        }
        fase->cormapa = novaCor;
    }
}

int telaInicial(Sound musica)
{

    // Inicialização
    //--------------------------------------------------------------------------------------

    InitWindow(LARGURATELA, ALTURATELA, NOMEJANELA);
    SetTargetFPS(FPS);
    //ToggleFullscreen();

    //toca a musica de fundo
    PlaySound(musica);

    //textura do logo do Bomberman
    Texture2D logoBomberman = LoadTexture("logo.png");
    Vector2 posicaoLogo = {(LARGURATELA/2) - (logoBomberman.width/2), (ALTURATELA/2) - logoBomberman.height};

    //--------------------------------------------------------------------------------------

    while (!WindowShouldClose())
    {
        //verifica se o usuario apertou enter para comecar o jogo
        if(IsKeyDown(KEY_ENTER))
        {
            StopSound(musica);
            return 1;
        }

        //verifica se a musica esta tocando
        if(!IsSoundPlaying(musica))
        {
            PlaySound(musica);
        }

        // Desenho da tela inicial
        //----------------------------------------------------------------------------------

        BeginDrawing();

        //Desenha fundo da tela
        ClearBackground(CORFUNDO_TI);

        //Desenha o logo
        DrawTextureV(logoBomberman, posicaoLogo, WHITE);

        //Desenha o texto de instrucao
        DrawText(TEXTO_TI, (LARGURATELA/2) - (MeasureText(TEXTO_TI, TAM_TEXTO_TI)/2), (ALTURATELA/2) - TAM_TEXTO_TI + (logoBomberman.height/2), TAM_TEXTO_TI, CORINSTRUCAO);

        //Desenha o texto de copyright
        DrawText(TEXTO_CPYRGT, (LARGURATELA/2) - (MeasureText(TEXTO_CPYRGT, TAM_TEXTO_CPYRGT)/2), ALTURATELA - (logoBomberman.height/2), TAM_TEXTO_CPYRGT, CORINSTRUCAO);

        EndDrawing();
    }

    //--------------------------------------------------------------------------------------

    CloseAudioDevice();

    return 0;
}

int carregaFase(FASE fase, Sound musica)
{

    // Inicialização
    //--------------------------------------------------------------------------------------

    //toca a musica de fundo
    PlaySound(musica);

    //inicia o string da fase
    char textoFase[MAX_STR_FASE];
    strcpy(textoFase, TextFormat(TEXT_CARREG, fase.n));

    //--------------------------------------------------------------------------------------

    while (!WindowShouldClose())
    {
        //verifica se o usuario apertou enter para comecar o jogo
        /*if(IsKeyDown(KEY_ENTER))
        {
            StopSound(musica);
            return 1;
        }*/

        //fecha a tela de carregamento caso a musica tenha acabado
        if(!IsSoundPlaying(musica))
        {
            StopSound(musica);
            return 1;
        }

        // Desenho da tela de carregamento
        //----------------------------------------------------------------------------------

        BeginDrawing();

        //Desenha fundo da tela
        ClearBackground(CORFUNDO_TI);

        //Desenha o texto de instrucao
        DrawText(textoFase, (LARGURATELA/2) - (MeasureText(textoFase, TAM_TEXTO_CARREG)/2), (ALTURATELA/2) - (TAM_TEXTO_CARREG/2), TAM_TEXTO_CARREG, CORINSTRUCAO);

        EndDrawing();
    }

    //--------------------------------------------------------------------------------------

    CloseAudioDevice();

    return 0;
}

int iniciaFase(FASE fase, Sound musicaFundo)
{

    // Inicialização
    //--------------------------------------------------------------------------------------

    //----------------------------------------------------------------------------------

    //toca a musica de fundo
    PlaySound(musicaFundo);

    //--------------------------------------------------------------------------------------

    //define o MAPA e seus dados
    MAPA mapa =
    {
        { ((GetScreenWidth() - LARGURAMAPA)/2), ((GetScreenHeight() - ALTURAMAPA)/2), LARGURAMAPA, ALTURAMAPA }, fase.cormapa
    };

    //define a textura do fundo do mapa
    Texture2D texturaMapa = LoadTexture("grama.png");

    //define as medidas do menu
    Rectangle gridMenu, menu = {mapa.ret.x - TAM_BORDA, mapa.ret.y - TAM_BORDA - TAM_TEXTO_MENU*2, mapa.ret.width + TAM_BORDA*2, TAM_TEXTO_MENU*2};

    //--------------------------------------------------------------------------------------

    //define os blocos nao quebraveis do mapa e os transponiveis
    BLOCO blocos[NBLOCOS];
    geraBlocos(blocos, mapa);

    //textura dos blocos
    Texture2D texturaBloco = LoadTexture("bloco.png");

    //textura de explosao
    Texture2D texturaExplosao = LoadTexture("explosao.png");

    //Indica a forma da textura de explosao
    Rectangle formaTexturaExplosao = { 0, 0, TAMANHOBLOCOS, TAMANHOBLOCOS };

    //Textura do portal
    Texture2D texturaPortal = LoadTexture("portal.png");

    //--------------------------------------------------------------------------------------

    //define o BOMBERMAN e seus dados
    BOMBERMAN bomberman = { (Rectangle)
    {
        (GetScreenWidth()/2) - (LARGURABOMBERMAN/2) + TAMANHOBLOCOS,
        (GetScreenHeight()/2) - (ALTURABOMBERMAN/2),
        LARGURABOMBERMAN,
        ALTURABOMBERMAN
    }, 0, LoadTexture("bomberman.png")
                          };

    //--------------------------------------------------------------------------------------

    //define os muros quebraveis do mapa
    geraMuros(fase.nmuros, blocos, mapa, bomberman);

    //textura dos muros
    Texture2D texturaMuro = LoadTexture("muro.png");

    //--------------------------------------------------------------------------------------

    //inicializa o vetor de INIMIGOS
    int l;
    INIMIGO listaInimigos[fase.ninimigos];

    //gera os INIMIGOS
    geraInimigos(listaInimigos, fase.ninimigos, mapa, blocos, bomberman);

    //textura dos inimigos
    Texture2D texturaInimigo = LoadTexture("inimigo.png");

    //--------------------------------------------------------------------------------------

    //inicializa o vetor de BOMBAS
    BOMBA listaBombas[fase.nbombas];

    //inicializa as BOMBAS
    inicializaBombas(listaBombas, fase.nbombas);

    //textura das bombas
    Texture2D texturaBomba = LoadTexture("bomba.png");

    //--------------------------------------------------------------------------------------

    //--------------------------------------------------------------------------------------

    // Loop de Jogo (continua enquanto o usuario nao clicar "ESC" e enquanto o bomberman nao morrer para um inimigo
    //--------------------------------------------------------------------------------------

    while (!WindowShouldClose())
    {

        // Atualização da tela do jogo
        //----------------------------------------------------------------------------------

        //verifica se a musica de fundo esta tocando
        if(!IsSoundPlaying(musicaFundo))
        {
            PlaySound(musicaFundo);
        }

        //movimenta o Bomberman a partir do teclado
        movimentaBomberman(&bomberman, mapa, blocos, listaBombas, fase.nbombas);

        //Atualiza a posicao dos inimigos
        movimentaInimigos(listaInimigos, fase.ninimigos, mapa, blocos, listaBombas, fase.nbombas, bomberman);

        //gera as bombas
        if(IsKeyPressed(KEY_SPACE))
        {
            geraBombas(listaBombas, fase.nbombas, bomberman, blocos, listaInimigos, fase.ninimigos);
        }

        //explode todas as bombas
        if(IsKeyDown(KEY_ENTER))
        {
            explodeBombas(listaBombas, fase.nbombas, &bomberman, listaInimigos, fase.ninimigos, blocos);
        }

        //apaga a textura da explosao
        apagaExplosao(blocos);

        //----------------------------------------------------------------------------------

        // Desenho do jogo
        //----------------------------------------------------------------------------------

        BeginDrawing();

        //Desenha fundo da tela
        ClearBackground(CORFUNDO);

        //desenha a borda do mapa
        DrawRectangleLinesEx( (Rectangle)
        {
            mapa.ret.x - TAM_BORDA, mapa.ret.y - TAM_BORDA,
                       mapa.ret.width + (TAM_BORDA*2), mapa.ret.height + (TAM_BORDA*2)
        }, TAM_BORDA, COR_BORDA);

        //desenha o fundo do menu
        DrawRectangleRec(menu, CORMENU);

        //calcula o tamanho do texto
        gridMenu.height = TAM_TEXTO_MENU;
        gridMenu.width = MeasureText(TextFormat(TEXT_CARREG, fase.n), TAM_TEXTO_MENU) + ESPACAMENTO;

        //Desenha os blocos
        desenhaBlocos(blocos, formaTexturaExplosao, texturaBloco, texturaMuro, texturaExplosao, texturaPortal, texturaMapa, fase);

        //Escreve o numero da fase no menu de opcoes
        DrawText(TextFormat(TEXT_CARREG, fase.n), menu.x, menu.y + (menu.y/2) - (TAM_TEXTO_MENU/2), TAM_TEXTO_MENU, CORINSTRUCAO);

        //Escreve o numero de vidas restantes no menu de opcoes
        DrawText(TextFormat(TEXT_VIDAS, fase.vidas), mapa.ret.x + MeasureText(TextFormat("Fase %d", fase.n), TAM_TEXTO_MENU) + ESPACAMENTO, mapa.ret.y - TAM_TEXTO_MENU - (TAM_TEXTO_MENU/2), TAM_TEXTO_MENU, CORINSTRUCAO);

        //Escreve a opcao de pause
        DrawText(TEXT_PAUSE, mapa.ret.x + mapa.ret.width - MeasureText(TEXT_PAUSE, TAM_TEXTO_MENU) - MeasureText(TEXT_ESC, TAM_TEXTO_MENU) - ESPACAMENTO, mapa.ret.y - TAM_TEXTO_MENU - (TAM_TEXTO_MENU/2), TAM_TEXTO_MENU, CORINSTRUCAO);

        //Escreve a opcao de saida
        DrawText(TEXT_ESC, mapa.ret.x + mapa.ret.width - MeasureText(TEXT_ESC, TAM_TEXTO_MENU), mapa.ret.y - TAM_TEXTO_MENU - (TAM_TEXTO_MENU/2), TAM_TEXTO_MENU, CORINSTRUCAO);

        //Desenha o bomberman
        DrawTexture(bomberman.textura, bomberman.ret.x, bomberman.ret.y, WHITE);
        //DrawRectangleRec(bomberman.ret, CORBOMBERMAN);

        //Desenha os inimigos
        for(l = 0; l < fase.ninimigos; l++)
        {
            if(listaInimigos[l].morto == 0)
            {
                DrawTexture(texturaInimigo, listaInimigos[l].ret.x, listaInimigos[l].ret.y, WHITE);
            }
        }

        //Desenha as bombas
        for(l = 0; l < fase.nbombas; l++)
        {
            if(listaBombas[l].status == ativada)
            {
                DrawTexture(texturaBomba, listaBombas[l].pos.x - TAMANHOBOMBA, listaBombas[l].pos.y - TAMANHOBOMBA, WHITE);
            }
        }

        EndDrawing();

        //----------------------------------------------------------------------------------

        // Verificacoes de status
        //----------------------------------------------------------------------------------

        //verifica se o bomberman colidiu com algum inimigo
        checaColisaoBombermanInimigos(listaInimigos, fase.ninimigos, &bomberman);

        //verifica se o bomberman morreu
        if(bomberman.morto)
        {
            StopSound(musicaFundo);
            return morte;
        }

        //verifica se o bomberman morreu
        if(contaInimigosVivos(listaInimigos, fase.ninimigos) == 0 && checaColisaoBombermanPortal(bomberman, blocos))
        {
            printf("%d", checaColisaoBombermanPortal(bomberman, blocos));
            StopSound(musicaFundo);
            //return vitoria;
        }

        //----------------------------------------------------------------------------------

    }
    //--------------------------------------------------------------------------------------

    CloseAudioDevice();

    return esc;
}

    void SalvarPontuacao(listaDeRanks, BOMBERMAN bomberman)
    {
        char nome[NOME];
        printf("\tInsira um nome:\n");
        gets("%c"&nome);
        int pontuacao = bomberman.pontos * 1000;
        listaDeRanks[TAMLISTA].nome = nome;
        listaDeRanks[TAMLISTA].pontos = pontuacao;
    }

int copiarArquivoParaVetor(RANKING vetor[])
{
    FILE *arquivo;
    int i = 0;

    // Abrir o arquivo para leitura binária
    arquivo = fopen("RANKING.bin", "rb");
    if (arquivo == NULL)
    {
        printf("Erro ao abrir o arquivo\n");
        return 0;
    }
    while (fread(&vetor[i], sizeof(RANKING), 1, arquivo) == 1 && i < ORDENALISTA)
    {
        i++;
    }


    //--------------------- bubblesort.
    void ordenaLista(RANKING lista[])
{
    int tamanho = TAMLISTA;
    int i, j;
    int temp;

    for(i = 0; i < tamanho - 1; i++)
    {
        for(j = 0; j < tamanho - i - 1; j++)
        {
            if(lista[j].pontos.score > lista[j+1].pontos.score)
            {
                temp = lista[j].pontos.score;
                lista[j].pontos.score = lista[j+1].pontos.score;
                lista[j+1].pontos.score = temp;
            }
        }
    }
}

    int copiarVetorParaArquivo(RANKING vetor[]) {
        FILE *arquivo;
        int quantidade = TAMLISTA;
        // Abrir o arquivo para escrita binária
        arquivo = fopen("RANKING.bin", "wb");
        if (arquivo == NULL) {
            printf("Erro ao abrir o arquivo\n");
            return 0;
        }

    // Escrever cada struct do vetor no arquivo
        for (int i = 0; i < quantidade; i++) {
            fwrite(&vetor[i], sizeof(RANKING), 1, arquivo);
        }

        // Fechar o arquivo
        fclose(arquivo);
}

void Ranqueador (BOMBERMAN bomberman)
{
    SalvarPontuacao(listaDeRanks, bomberman);
    copiarArquivoParaVetor(listaDeRanks);
    ordenaLista(listaDeRanks);
    copiarVetorParaArquivo(listaDeRanks);
}




    int telaFinal(Sound musica)
    {

        // Inicialização
        //--------------------------------------------------------------------------------------

        //toca a musica de fundo
        PlaySound(musica);

        //--------------------------------------------------------------------------------------

        while (!WindowShouldClose())
        {

            //verifica se a musica parou de tocar
            if(!IsSoundPlaying(musica))
            {
                PlaySound(musica);
            }

            // Desenho da tela de carregamento
            //----------------------------------------------------------------------------------

            BeginDrawing();

            //Desenha fundo da tela
            ClearBackground(CORFUNDO_TI);

            //Desenha o texto de instrucao
            DrawText(TEXTO_FIM, (LARGURATELA/2) - (MeasureText(TEXTO_FIM, TAM_TEXTO_CARREG)/2), (ALTURATELA/2) - (TAM_TEXTO_CARREG/2), TAM_TEXTO_CARREG, CORINSTRUCAO);

            EndDrawing();
        }

        //--------------------------------------------------------------------------------------

        CloseAudioDevice();

        return 0;
    }

    void iniciaJogo(FASE *fase, Color coresMapa[], Sound musicaCarregamento, Sound musicaFundoFase, Sound musicaFinal)
    {
        //inicia o indicador de morte do bomberman
        int statusFase, continuar=1;

        //Inicia o carregamento da fase
        geraFase(fase, coresMapa);
        if(carregaFase(*fase, musicaCarregamento))
        {
            //Inicia a fase
            statusFase = iniciaFase(*fase, musicaFundoFase);

            //diminui a vida se o bomberman morreu
            if(statusFase == morte && fase->vidas > 0)
            {
                fase->vidas--;
            }

            //Recomeca a fase enquanto o bomberman nao vencer, o usuario nao apertar "esc" e as vidas nao acabarem
            while(statusFase == morte && fase->vidas > 0 && continuar == 1)
            {
                if(carregaFase(*fase, musicaCarregamento))
                {
                    statusFase = iniciaFase(*fase, musicaFundoFase);
                    fase->vidas--;
                }
                else
                {
                    continuar = 0;
                }
            }

            //passa para a próxima fase caso o bomberman tenha vencido a fase anterior e ainda tenha vidas
            if(statusFase == vitoria && fase->vidas > 0  && continuar == 1)
            {
                iniciaJogo(fase, coresMapa, musicaCarregamento, musicaFundoFase, musicaFinal);
            }
            else if(fase->vidas <= 0 && continuar == 1)
            {
                telaFinal(musicaFinal);
            }
        }
    }

    int main(void)
    {

        //Inicializacao
        //----------------------------------------------------------------------------------

        //cria o vetor com as possiveis cores de mapa
        Color coresMapa[] = { CORPADRAO_MAPA, ORANGE, PINK, RED, GREEN, BLUE, PURPLE, DARKPURPLE, BEIGE, DARKBROWN };

        //inicializa a estrutura de fase
        FASE fase = { 0, NVIDAS, 0, 0, 0, coresMapa[0]};

        //----------------------------------------------------------------------------------

        //Musicas
        //----------------------------------------------------------------------------------

        InitAudioDevice();

        //Musica da tela inicial
        Sound musicaTelaInicial = LoadSound("telainicial.mp3");

        //Musica da tela de carregamento
        Sound musicaCarregamento = LoadSound("carregamento.mp3");

        //Musica de fundo da fase
        Sound musicaFundoFase = LoadSound("fase.mp3");

        //Musica da tela final
        Sound musicaFinal = LoadSound("fim.mp3");

        //----------------------------------------------------------------------------------

        //Jogo
        //----------------------------------------------------------------------------------

        //Inicia a tela inicial
        if(telaInicial(musicaTelaInicial))
        {
            iniciaJogo(&fase, coresMapa, musicaCarregamento, musicaFundoFase, musicaFinal);
        }

        //----------------------------------------------------------------------------------

        return 0;
    }
