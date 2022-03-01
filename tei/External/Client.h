#pragma once

// Called after the engine has prepared internal resources and will start the game loop shortly
extern void TeiClientInit();

// Called after the engine has stopped the game loop and is cleaning up client data
extern void TeiClientCleanup();
