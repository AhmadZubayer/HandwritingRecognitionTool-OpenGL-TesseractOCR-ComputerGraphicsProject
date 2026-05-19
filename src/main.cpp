#include <GL/glew.h>
#include <GL/freeglut.h>
#include <windows.h>
#include "starting-page/characters.cpp"
#include "starting-page/animation.cpp"
#include "starting-page/start.cpp"
#include "drawing-canvas/drawing_main.cpp"

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(1200, 800);
    glutInitWindowPosition(50, 50);
    glutCreateWindow("Handwriting Recognition Tool");

    initStart();

    // Allow glutMainLoop() to return
    glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);

    glutDisplayFunc(startDisplay);
    glutReshapeFunc(startReshape);
    glutKeyboardFunc(startKeyboard);
    glutSpecialFunc(startSpecial);
    glutMouseFunc(startMouse);
    glutTimerFunc(0, startTimer, 0);

    std::cout << "[Main] Starting GLUT loop..." << std::endl;
    glutMainLoop();
    
    std::cout << "[Main] GLUT loop returned. Starting Drawing Tool..." << std::endl;
    // This will run if glutMainLoop returns (e.g., via glutLeaveMainLoop)
    runDrawingTool();
    
    return 0;
}
