#include "raylib.h"

#define LARGURATELA 1000
#define ALTURATELA 650
#define LARGURAMAPA 900
#define ALTURAMAPA 550
#define LARGURABOMBERMAN 50
#define ALTURABOMBERMAN 50
#define TAMANHOINIMIGO 25
#define VELBOMBERMAN 2.0f
#define VELINIMIGO 3.0f
#define NPASSOS 100
#define CORBOMBERMAN BROWN
#define CORINIMIGO RED
#define CORINSTRUCAO WHITE
#define CORMAPA GREEN
#define CORFUNDO DARKGRAY

int main(void)
{
    // Inicialização
    InitWindow(LARGURATELA, ALTURATELA, "Bomberman");

    //Monta os vetores relacionados ao mapa
    Vector2 posicaoMapa = { ((GetScreenWidth() - LARGURAMAPA)/2), ((GetScreenHeight() -1 - ALTURAMAPA)/2) }; //inicia o jogador no meio da tela
    Vector2 tamanhoMapa = { LARGURAMAPA, ALTURAMAPA+2 };

    //definicoes de variaveis
    int vel_alx, vel_aly, passos = 0;

    //Monta os vetores e estruturas relacionados ao jogador (bomberman)
    Vector2 posicaoBomberman = { GetScreenWidth() /2, GetScreenHeight() /2 }; //inicia o jogador no meio da tela
    Vector2 tamanhoBomberman = { LARGURABOMBERMAN, ALTURABOMBERMAN };
    Rectangle bomberman = { GetScreenWidth()/2, GetScreenHeight()/2, LARGURABOMBERMAN, ALTURABOMBERMAN};

    //Inimigo 1
    vel_alx = GetRandomValue(-1, 1);
    vel_aly = GetRandomValue(-1, 1);
    while(vel_alx == 0 && vel_aly == 0)
    {
        vel_alx = GetRandomValue(-1, 1);
        vel_aly = GetRandomValue(-1, 1);
    }
    Vector2 velocidadeInimigo1 = { vel_alx * VELINIMIGO, vel_aly * VELINIMIGO };
    Vector2 posicaoInimigo1 = {
        GetRandomValue(TAMANHOINIMIGO + ((GetScreenWidth() - LARGURAMAPA)/2), (GetScreenWidth() - ((GetScreenWidth() - LARGURAMAPA)/2) - TAMANHOINIMIGO)),
        GetRandomValue(TAMANHOINIMIGO + ((GetScreenHeight() - ALTURAMAPA)/2), (GetScreenHeight() - ((GetScreenHeight() - ALTURAMAPA)/2) - TAMANHOINIMIGO))
    };

    SetTargetFPS(60);
    //--------------------------------------------------------------------------------------

    // Loop de Jogo
    while (!WindowShouldClose() && !CheckCollisionCircleRec(posicaoInimigo1, TAMANHOINIMIGO, bomberman))
    {
        // Atualização
        //----------------------------------------------------------------------------------

        //Movimenta o jogador e impede que ele atravesse as paredes do jogo
        if (IsKeyDown(KEY_RIGHT) && (posicaoBomberman.x < (GetScreenWidth() - ((GetScreenWidth() - LARGURAMAPA)/2) - tamanhoBomberman.x))) posicaoBomberman.x += VELBOMBERMAN;
        if (IsKeyDown(KEY_LEFT) && (posicaoBomberman.x > ((GetScreenWidth() - LARGURAMAPA)/2))) posicaoBomberman.x -= VELBOMBERMAN;
        if (IsKeyDown(KEY_UP) && (posicaoBomberman.y > ((GetScreenHeight() - ALTURAMAPA)/2))) posicaoBomberman.y -= VELBOMBERMAN;
        if (IsKeyDown(KEY_DOWN) && (posicaoBomberman.y < (GetScreenHeight() - ((GetScreenHeight() - ALTURAMAPA)/2) - tamanhoBomberman.y))) posicaoBomberman.y += VELBOMBERMAN;
        bomberman.x = posicaoBomberman.x;
        bomberman.y = posicaoBomberman.y;
        //----------------------------------------------------------------------------------

        // Logica do movimento inimigo 1
        posicaoInimigo1.x += velocidadeInimigo1.x;
        posicaoInimigo1.y += velocidadeInimigo1.y;
        passos++;
        if ((posicaoInimigo1.x >= (GetScreenWidth() - ((GetScreenWidth() - LARGURAMAPA)/2) - TAMANHOINIMIGO)) || (posicaoInimigo1.x <= TAMANHOINIMIGO + ((GetScreenWidth() - LARGURAMAPA)/2))) velocidadeInimigo1.x *= -1.0f;
        if ((posicaoInimigo1.y >= (GetScreenHeight() - ((GetScreenHeight() - ALTURAMAPA)/2) - TAMANHOINIMIGO)) || (posicaoInimigo1.y <= TAMANHOINIMIGO + ((GetScreenHeight() - ALTURAMAPA)/2))) velocidadeInimigo1.y *= -1.0f;
        if(passos >= NPASSOS)
        {
            vel_alx = GetRandomValue(-1, 1);
            vel_aly = GetRandomValue(-1, 1);
            while(vel_alx == 0 && vel_aly == 0)
            {
                vel_alx = GetRandomValue(-1, 1);
                vel_aly = GetRandomValue(-1, 1);
            }
            velocidadeInimigo1.x = vel_alx * VELINIMIGO;
            velocidadeInimigo1.y = vel_aly * VELINIMIGO;
            passos = 0;
        }

        // Desenho do jogo
        BeginDrawing();

            ClearBackground(CORFUNDO);
            DrawRectangleV(posicaoMapa, tamanhoMapa, CORMAPA);

            DrawText("Mova o quadrado com as setas do seu teclado", 10, 10, 20, CORINSTRUCAO);

            DrawCircleV(posicaoInimigo1, TAMANHOINIMIGO, CORINIMIGO);
            DrawRectangleV(posicaoBomberman, tamanhoBomberman, CORBOMBERMAN);

        EndDrawing();
    }

    return 0;
}
