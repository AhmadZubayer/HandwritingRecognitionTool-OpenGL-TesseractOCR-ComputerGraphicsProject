#include <GL/glut.h>
#include <string>
#include <vector>

// Forward declarations from characters.cpp
void drawA(); void drawB(); void drawC(); void drawD(); void drawE(); void drawF();
void drawG(); void drawH(); void drawI(); void drawJ(); void drawK(); void drawL();
void drawM(); void drawN(); void drawO(); void drawP(); void drawQ(); void drawR();
void drawS(); void drawT(); void drawU(); void drawV(); void drawW(); void drawX();
void drawY(); void drawZ();

// Forward declarations from animation.cpp
extern float animSpeed;
void drawWaveAnimation();
void handleAnimationSpeed(unsigned char key, int x, int y);

void drawLetter(char c) {
    switch (c) {
        case 'A': drawA(); break; case 'B': drawB(); break; case 'C': drawC(); break;
        case 'D': drawD(); break; case 'E': drawE(); break; case 'F': drawF(); break;
        case 'G': drawG(); break; case 'H': drawH(); break; case 'I': drawI(); break;
        case 'J': drawJ(); break; case 'K': drawK(); break; case 'L': drawL(); break;
        case 'M': drawM(); break; case 'N': drawN(); break; case 'O': drawO(); break;
        case 'P': drawP(); break; case 'Q': drawQ(); break; case 'R': drawR(); break;
        case 'S': drawS(); break; case 'T': drawT(); break; case 'U': drawU(); break;
        case 'V': drawV(); break; case 'W': drawW(); break; case 'X': drawX(); break;
        case 'Y': drawY(); break; case 'Z': drawZ(); break;
    }
}

void drawString(const std::string& str, float x, float y, float scale, float r, float g, float b) {
    glPushMatrix();
    glTranslatef(x, y, 0.0f);
    glScalef(scale, scale, 1.0f);
    glColor3f(r, g, b);
    for (char c : str) {
        if (c == ' ') {
            glTranslatef(0.4f, 0.0f, 0.0f);
        } else {
            drawLetter(c);
            glTranslatef(0.5f, 0.0f, 0.0f);
        }
    }
    glPopMatrix();
}

float getStringWidth(const std::string& str, float scale) {
    float width = 0;
    for (size_t i = 0; i < str.length(); ++i) {
        if (str[i] == ' ') width += 0.4f * scale;
        else width += 0.5f * scale;
    }
    return width;
}

void drawCenteredString(const std::string& str, float y, float scale, float r, float g, float b) {
    float width = getStringWidth(str, scale);
    drawString(str, -width / 2.0f, y, scale, r, g, b);
}

void drawNextButton() {
    glPushMatrix();
    glTranslatef(0.8f, -0.85f, 0.0f);
    
    // Button background (Blue-ish)
    glColor3f(0.1f, 0.4f, 0.8f);
    glBegin(GL_QUADS);
    glVertex2f(-0.2f, -0.1f); glVertex2f(0.2f, -0.1f);
    glVertex2f(0.2f, 0.1f); glVertex2f(-0.2f, 0.1f);
    glEnd();
    
    // Button border
    glColor3f(1.0f, 1.0f, 1.0f);
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(-0.2f, -0.1f); glVertex2f(0.2f, -0.1f);
    glVertex2f(0.2f, 0.1f); glVertex2f(-0.2f, 0.1f);
    glEnd();
    glLineWidth(1.0f);
    
    drawString("NEXT", -0.12f, -0.05f, 0.18f, 1.0f, 1.0f, 1.0f);
    glPopMatrix();
}

bool transitionToMain = false;
float zoomFactor = 1.0f;

void startDisplay() {
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();

    glPushMatrix();
    glScalef(zoomFactor, zoomFactor, 1.0f);

    // Draw Animation Background
    glPushMatrix();
    glTranslatef(0.0f, -0.4f, 0.0f);
    drawWaveAnimation();
    glPopMatrix();

    // Title in Red (centered and bigger)
    drawCenteredString("HANDWRITING RECOGNITION TOOL", 0.8f, 0.1f, 1.0f, 0.0f, 0.0f);
    drawCenteredString("USING OPENGL AND GOOGLE TESSERACT  OCR", 0.65f, 0.1f, 1.0f, 0.0f, 0.0f);

    // Course
    drawCenteredString("COURSE COMPUTER GRAPHICS", 0.4f, 0.08f, 1.0f, 1.0f, 1.0f);

    // Developed By
    drawCenteredString("DEVELOPED BY", 0.15f, 0.07f, 0.8f, 0.8f, 0.8f);
    drawCenteredString("MD ABU SAIEED", 0.0f, 0.08f, 1.0f, 1.0f, 1.0f);
    drawCenteredString("AHMAD ZUBAYER", -0.15f, 0.08f, 1.0f, 1.0f, 1.0f);
    drawCenteredString("ROHAN RUBAYET", -0.3f, 0.08f, 1.0f, 1.0f, 1.0f);

    if (transitionToMain) {
        drawCenteredString("LOADING MAIN TOOL", -0.6f, 0.1f, 0.0f, 1.0f, 0.0f);
    } else {
        drawNextButton();
    }

    glPopMatrix(); 

    glutSwapBuffers();
}

void startTimer(int value) {
    glutPostRedisplay();
    glutTimerFunc(16, startTimer, 0);
}

void startKeyboard(unsigned char key, int x, int y) {
    switch (key) {
        case 'a': animSpeed = 0.0f; break; 
        case 'w': animSpeed = 0.02f; break; 
        case '+': zoomFactor += 0.1f; break; 
        case '-': zoomFactor -= 0.1f; if (zoomFactor < 0.1f) zoomFactor = 0.1f; break; 
    }
}

void startSpecial(int key, int x, int y) {
    int mod = glutGetModifiers();
    if (key == GLUT_KEY_RIGHT && (mod == GLUT_ACTIVE_CTRL)) {
        transitionToMain = true;
    }
    
    // Up arrow to increase speed, Down arrow to decrease speed
    if (key == GLUT_KEY_UP) {
        animSpeed += 0.005f;
    } else if (key == GLUT_KEY_DOWN) {
        animSpeed -= 0.005f;
        if (animSpeed < 0.0f) animSpeed = 0.0f;
    }
}

void startMouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        float width = (float)glutGet(GLUT_WINDOW_WIDTH);
        float height = (float)glutGet(GLUT_WINDOW_HEIGHT);
        float aspect = width / height;
        
        // Map window coordinates to world coordinates (accounting for aspect and zoom)
        float wx = ((float)x / (width / 2.0f) - 1.0f) * aspect / zoomFactor;
        float wy = (1.0f - (float)y / (height / 2.0f)) / zoomFactor;
        
        // Button center at 0.8, -0.85. Size 0.4 x 0.2
        if (wx > 0.6f && wx < 1.0f && wy > -0.95f && wy < -0.75f) {
            transitionToMain = true;
            glutLeaveMainLoop();
        }
    }
}

void startReshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    float aspect = (float)w / (float)h;
    gluOrtho2D(-1.0 * aspect, 1.0 * aspect, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
}

void initStart() {
    glClearColor(0.094f, 0.094f, 0.094f, 1.0f); // #181818
}
