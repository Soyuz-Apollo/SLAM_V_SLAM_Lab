#ifndef _glutloop_h_
#define _glutloop_h_

namespace VW {

  // Initialise GLUT
    void GLUTInit(int argc, char** argv, bool use_idler=false);


     // The main loop. After creating all displays and stuff, call this baby to
    // hand over control

    void GLUTLoop();


}


#endif
