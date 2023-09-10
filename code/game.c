/* ========================================================================
   Creator: Grimleik $
   ========================================================================*/

// TODO: Render a Triangle.
// TODO: Incorporate OpenGL.
// TODO: 3D Rendering.
// TODO: Start CubeWorld.

#include "age.h"

GAME_INIT(GameInit)
{
}

GAME_UPDATE(GameUpdate)
{
   if(InputKeyPressed(platformState->input, KEY_ESCAPE))
   {
      platformState->isRunning = false;
   }
   rcClearColor_t *cmd = PushRenderCommand(platformState->renderList, rcClearColor);
   // cmd->color = ageCOLOR_BLUE; 
   cmd->color = ageCOLOR_BLACK; 
   
   rcLine_t *cmd2 = PushRenderCommand(platformState->renderList, rcLine);
   cmd2->p0 = (v2f){ 5.0f, 5.0f};
   cmd2->col0 = ageCOLOR_RED;
   cmd2->p1 = (v2f){ 10.5f, 10.5f};
   cmd2->col1 = ageCOLOR_GREEN;
}

GAME_SHUTDOWN(GameShutdown)
{
}