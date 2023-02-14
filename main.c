#include "raylib.h"
#include <stdio.h>

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 1080;
    const int screenHeight = 480;
    int i;
    InitWindow(screenWidth, screenHeight, "Sprite Animation Example");
    // Carregar a imagem que contém todos os frames do sprite
    Texture2D spriteSheet = LoadTexture("./sprites/sprt_hero/satyr-Sheet (2).png");
    Texture2D spriteCorrendo = LoadTexture("./sprites/sprt_hero/satyr-Sheet.png");

    //Inimigo teste
    Vector2 ballSpeed = { 5.0f, 4.0f };
    int ballRadius = 20;
    bool pause = 0;

    // Definir o tamanho do retângulo para cada frame
    Rectangle frameRec = { 0.0f, 0.0f, (float)spriteSheet.width/6, (float)spriteSheet.height/6 };
    Rectangle frameCorrendo = { 32, 32, (float)spriteCorrendo.width/9, (float)spriteCorrendo.height/6 };

    // Definir a posição inicial do sprite na tela
    Vector2 position = { screenWidth/2, screenHeight/2 };

    // Definir o tempo de atualização do sprite
    float elapsedTime = 0.0f;
    float frameSpeed = 0.1f;


    //Player teste
    Vector2 playerPosition = { 180, 130 };
    int direcao=GetRandomValue(-1,1);

    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------
    //float frameWidth = (float)(Hero.width / 6 );
    //float frameHeight = (float)(Hero.height / 5);
    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        elapsedTime += GetFrameTime();

        if (elapsedTime >= frameSpeed)
        {
            // Mudar o frame
            frameRec.x += frameRec.width;
            if (frameRec.x >= spriteSheet.width) frameRec.x = 0;

            // Reiniciar o contador de tempo
            elapsedTime = 0.0f;
        }

        // Verificar se a tecla para a esquerda está pressionada
        if (IsKeyDown(KEY_LEFT))
        {
            position.x -= 2.0f;
        }

        // Verificar se a tecla para a direita está pressionada
        if (IsKeyDown(KEY_RIGHT))
        {
            position.x += 2.0f;
        }

        // Verificar se a tecla para cima está pressionada
        if (IsKeyDown(KEY_UP))
        {
            position.y -= 2.0f;
        }

        // Verificar se a tecla para baixo está pressionada
        if (IsKeyDown(KEY_DOWN))
        {
            position.y += 2.0f;
        }
        if (!pause)
        {
            playerPosition.x += ballSpeed.x;
            playerPosition.y += ballSpeed.y;

            // Check walls collision for bouncing
            if ((playerPosition.x >= (GetScreenWidth() - ballRadius)) || (playerPosition.x <= ballRadius)) ballSpeed.x *= (GetRandomValue(-1.0f,0.0f));
            if ((playerPosition.y >= (GetScreenHeight() - ballRadius)) || (playerPosition.y <= ballRadius)) ballSpeed.y *= (GetRandomValue(-1.0f,0.0f));
        }

        //-----Colisão
        Rectangle personagem1 = {position.x, position.y, frameRec.width, frameRec.height};
        Rectangle personagem2 = {playerPosition.x, playerPosition.y, frameRec.width, frameRec.height};

        if (CheckCollisionRecs(personagem1, personagem2))
        {
            // Colisão detectada entre os dois personagens
            CloseWindow();
        }
        else
        {
            // Sem colisão
        }



        //----------------------------------------------------------------------------------
        // TODO: Update your variables here
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

        ClearBackground((Color)
        {
            0, 82, 172, 255
        });
        // Draw player Teste
            DrawRectangleV(playerPosition, (Vector2){ 32, 32 }, RED);

        DrawTextureRec(spriteCorrendo, frameCorrendo, position, RAYWHITE);
        // Desaloca as texturas do sprite
        //DrawTextureRec(spriteSheet, frameRec, position, RAYWHITE);

        DrawText("BOMBERMAN", 0, 0, 20, LIGHTGRAY);

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}
