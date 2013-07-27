#include "stdafx.h"
#include "Window.h"


int WINAPI WinMain(HINSTANCE	hInstance,			// Instance
                   HINSTANCE	hPrevInstance,	// Previous Instance
                   LPSTR		  lpCmdLine,			// Command Line Parameters
                   int			  nCmdShow)				// Window Show State
{

  //std::ifstream INf("C:/Users/bogert/Documents/halos_xyzmt.out");
  //std::ofstream OUTf("C:/Users/bogert/Documents/halos_xyzmt.bin", std::ios::out | std::ios::binary);// | std::ios::binary);



  Window *WIN = new Window();

  ParticleSystem * PT = new ParticleSystem(lpCmdLine);
  WIN->AddParticleSystem(PT);

  WIN->ProgramLoop();

  delete PT;
  delete WIN;
}


 /*



  */
