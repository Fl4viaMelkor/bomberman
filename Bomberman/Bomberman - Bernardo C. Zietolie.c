#include "raylib.h"

#define LARGURATELA 800
#define ALTURATELA 450
#define LARGURAJOGADOR 50
#define ALTURAJOGADOR 50
#define TAMANHOINIMIGO 50
#define VELJOGADOR 2.0f
#define VELINIMIGO 5.0f
#define NPASSOS 200

int main(void)
{
    // Inicialização
    //--------------------------------------------------------------------------------------
    InitWindow(LARGURATELA, ALTURATELA, "Bomberman");
    int vel_alx, vel_aly, passos = 0;
    Vector2 posicaoMeio = { GetScreenWidth() /2, GetScreenHeight() /2 };

    //Jogador
    Vector2 posicaoJogador = { GetScreenWidth() /2, GetScreenHeight() /2 };
    Vector2 tamanhoJogador = {LARGURAJOGADOR,ALTURAJOGADOR};
    Rectangle jogador = { GetScreenWidth()/2, GetScreenHeight()/2, LARGURAJOGADOR, ALTURAJOGADOR};

    //Inimigo 1
    vel_alx = GetRandomValue(-1, 1);
    vel_aly = GetRandomValue(-1, 1);
    while(vel_alx == 0 && vel_aly == 0)
    {
        vel_alx = GetRandomValue(-1, 1);
        vel_aly = GetRandomValue(-1, 1);
    }
    Vector2 velocidadeInimigo1 = { vel_alx * VELINIMIGO, vel_aly * VELINIMIGO };
    Vector2 posicaoInimigo1 = { GetRandomValue(tamanhoJogador.x, (GetScreenWidth() - tamanhoJogador.x)), GetRandomValue(tamanhoJogador.y, (GetScreenHeight() - tamanhoJogador.y)) };

    SetTargetFPS(60);
    //--------------------------------------------------------------------------------------

    // Loop de Jogo
    while (!WindowShouldClose() && !CheckCollisionCircleRec(posicaoInimigo1, TAMANHOINIMIGO, jogador))
    {
        // Atualização
        //----------------------------------------------------------------------------------

        //Movimenta o jogador e impede que ele atravesse as paredes do jogo
        if (IsKeyDown(KEY_RIGHT) && (posicaoJogador.x < (GetScreenWidth() - tamanhoJogador.x))) posicaoJogador.x += VELJOGADOR;
        if (IsKeyDown(KEY_LEFT) && (posicaoJogador.x > 0)) posicaoJogador.x -= VELJOGADOR;
        if (IsKeyDown(KEY_UP) && (posicaoJogador.y > 0)) posicaoJogador.y -= VELJOGADOR;
        if (IsKeyDown(KEY_DOWN) && (posicaoJogador.y < (GetScreenHeight() - tamanhoJogador.y))) posicaoJogador.y += VELJOGADOR;
        //----------------------------------------------------------------------------------

        // Logica do balanco inimigo 1
        posicaoInimigo1.x += velocidadeInimigo1.x;
        posicaoInimigo1.y += velocidadeInimigo1.y;
        passos++;
        if ((posicaoInimigo1.x >= (GetScreenWidth() - TAMANHOINIMIGO)) || (posicaoInimigo1.x <= TAMANHOINIMIGO)) velocidadeInimigo1.x *= -1.0f;
        if ((posicaoInimigo1.y >= (GetScreenHeight() - TAMANHOINIMIGO)) || (posicaoInimigo1.y <= TAMANHOINIMIGO)) velocidadeInimigo1.y *= -1.0f;
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
        jogador.x = posicaoJogador.x;
        jogador.y = posicaoJogador.y;

        //-----------------------------------------------------

        // Desenho
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(RAYWHITE);

            DrawText("Mova o quadrado com as setas do seu teclado", 10, 10, 20, DARKGRAY);

            DrawCircleV(posicaoInimigo1, TAMANHOINIMIGO, RED);
            DrawRectangleV(posicaoJogador, tamanhoJogador, GREEN);

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    //CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}
