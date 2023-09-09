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
}

GAME_SHUTDOWN(GameShutdown)
{
}