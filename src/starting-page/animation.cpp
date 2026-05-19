#include <GL/glut.h>
#include <cmath>
#include <vector>

float animSpeed = 0.02f;
float phase = 0.0f;

struct Point {
    float x, y;
};

void drawWaveAnimation() {
    glColor3f(1.0f, 1.0f, 1.0f); // White dots
    glPointSize(2.0f);
    glBegin(GL_POINTS);
    for (float x = -2.0f; x <= 2.0f; x += 0.05f) {
        for (float z = -1.0f; z <= 1.0f; z += 0.1f) {
            float y = 0.2f * sinf(5.0f * x + phase) * cosf(5.0f * z + phase);
            glVertex2f(x, y + z * 0.5f); // Simple 2D projection of a 3D wave
        }
    }
    glEnd();
    phase += animSpeed;
}

void handleAnimationSpeed(unsigned char key, int x, int y) {
    int mod = glutGetModifiers();
    if (mod == GLUT_ACTIVE_CTRL) {
        if (key == '+' || key == '=' || key == 43) { // '+' or '=' with Ctrl
            animSpeed += 0.005f;
        } else if (key == '-' || key == 45 || key == 31) { // '-' or '_' with Ctrl (31 is sometimes Ctrl+-)
            animSpeed -= 0.005f;
            if (animSpeed < 0.0f) animSpeed = 0.0f;
        }
    }
}
