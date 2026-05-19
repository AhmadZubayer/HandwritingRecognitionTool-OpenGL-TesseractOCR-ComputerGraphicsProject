#include <GL/glut.h>

void drawBezier(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4) {
    int segments = 20;
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i <= segments; i++) {
        float t = (float)i / segments;
        float omt = 1.0f - t;
        float x = omt * omt * omt * x1 + 3 * t * omt * omt * x2 + 3 * t * t * omt * x3 + t * t * t * x4;
        float y = omt * omt * omt * y1 + 3 * t * omt * omt * y2 + 3 * t * t * omt * y3 + t * t * t * y4;
        glVertex2f(x, y);
    }
    glEnd();
}

void drawA() {
    glLineWidth(3.0f);
    glBegin(GL_LINES);
    glVertex2f(-0.15f, -0.3f); glVertex2f(0.00f, 0.3f);
    glVertex2f(0.15f, -0.3f); glVertex2f(0.00f, 0.3f);
    glVertex2f(-0.08f, 0.0f); glVertex2f(0.08f, 0.0f);
    glEnd();
}

void drawB() {
    glLineWidth(3.0f);
    glBegin(GL_LINES);
    glVertex2f(-0.15f, 0.3f); glVertex2f(-0.15f, -0.3f);
    glVertex2f(-0.15f, 0.3f); glVertex2f(0.05f, 0.3f);
    glVertex2f(-0.15f, 0.0f); glVertex2f(0.05f, 0.0f);
    glVertex2f(-0.15f, -0.3f); glVertex2f(0.05f, -0.3f);
    glEnd();
    drawBezier(0.05f, 0.3f, 0.2f, 0.3f, 0.2f, 0.0f, 0.05f, 0.0f);
    drawBezier(0.05f, 0.0f, 0.25f, 0.0f, 0.25f, -0.3f, 0.05f, -0.3f);
}

void drawC() {
    glLineWidth(3.0f);
    drawBezier(0.15f, 0.2f, 0.15f, 0.3f, -0.15f, 0.3f, -0.15f, 0.0f);
    drawBezier(-0.15f, 0.0f, -0.15f, -0.3f, 0.15f, -0.3f, 0.15f, -0.2f);
}

void drawD() {
    glLineWidth(3.0f);
    glBegin(GL_LINES);
    glVertex2f(-0.15f, 0.3f); glVertex2f(-0.15f, -0.3f);
    glVertex2f(-0.15f, 0.3f); glVertex2f(0.0f, 0.3f);
    glVertex2f(-0.15f, -0.3f); glVertex2f(0.0f, -0.3f);
    glEnd();
    drawBezier(0.0f, 0.3f, 0.25f, 0.3f, 0.25f, -0.3f, 0.0f, -0.3f);
}

void drawE() {
    glLineWidth(3.0f);
    glBegin(GL_LINE_STRIP);
    glVertex2f(0.15f, 0.3f);
    glVertex2f(-0.15f, 0.3f);
    glVertex2f(-0.15f, -0.3f);
    glVertex2f(0.15f, -0.3f);
    glEnd();
    glBegin(GL_LINES);
    glVertex2f(-0.15f, 0.0f); glVertex2f(0.05f, 0.0f);
    glEnd();
}

void drawF() {
    glLineWidth(3.0f);
    glBegin(GL_LINE_STRIP);
    glVertex2f(0.15f, 0.3f);
    glVertex2f(-0.15f, 0.3f);
    glVertex2f(-0.15f, -0.3f);
    glEnd();
    glBegin(GL_LINES);
    glVertex2f(-0.15f, 0.0f); glVertex2f(0.05f, 0.0f);
    glEnd();
}

void drawG() {
    glLineWidth(3.0f);
    drawBezier(0.15f, 0.2f, 0.15f, 0.3f, -0.15f, 0.3f, -0.15f, 0.0f);
    drawBezier(-0.15f, 0.0f, -0.15f, -0.3f, 0.15f, -0.3f, 0.15f, -0.1f);
    glBegin(GL_LINE_STRIP);
    glVertex2f(0.15f, -0.1f);
    glVertex2f(0.15f, -0.1f);
    glVertex2f(0.0f, -0.1f);
    glEnd();
}

void drawH() {
    glLineWidth(3.0f);
    glBegin(GL_LINES);
    glVertex2f(-0.15f, 0.3f); glVertex2f(-0.15f, -0.3f);
    glVertex2f(0.15f, 0.3f); glVertex2f(0.15f, -0.3f);
    glVertex2f(-0.15f, 0.0f); glVertex2f(0.15f, 0.0f);
    glEnd();
}

void drawI() {
    glLineWidth(3.0f);
    glBegin(GL_LINES);
    glVertex2f(-0.1f, 0.3f); glVertex2f(0.1f, 0.3f);
    glVertex2f(0.0f, 0.3f); glVertex2f(0.0f, -0.3f);
    glVertex2f(-0.1f, -0.3f); glVertex2f(0.1f, -0.3f);
    glEnd();
}

void drawJ() {
    glLineWidth(3.0f);
    glBegin(GL_LINES);
    glVertex2f(-0.05f, 0.3f); glVertex2f(0.15f, 0.3f);
    glVertex2f(0.05f, 0.3f); glVertex2f(0.05f, -0.1f);
    glEnd();
    drawBezier(0.05f, -0.1f, 0.05f, -0.3f, -0.15f, -0.3f, -0.15f, -0.1f);
}

void drawK() {
    glLineWidth(3.0f);
    glBegin(GL_LINES);
    glVertex2f(-0.15f, 0.3f); glVertex2f(-0.15f, -0.3f);
    glVertex2f(-0.15f, 0.0f); glVertex2f(0.15f, 0.3f);
    glVertex2f(-0.15f, 0.0f); glVertex2f(0.15f, -0.3f);
    glEnd();
}

void drawL() {
    glLineWidth(3.0f);
    glBegin(GL_LINE_STRIP);
    glVertex2f(-0.15f, 0.3f);
    glVertex2f(-0.15f, -0.3f);
    glVertex2f(0.15f, -0.3f);
    glEnd();
}

void drawM() {
    glLineWidth(3.0f);
    glBegin(GL_LINE_STRIP);
    glVertex2f(-0.15f, -0.3f);
    glVertex2f(-0.15f, 0.3f);
    glVertex2f(0.0f, 0.0f);
    glVertex2f(0.15f, 0.3f);
    glVertex2f(0.15f, -0.3f);
    glEnd();
}

void drawN() {
    glLineWidth(3.0f);
    glBegin(GL_LINE_STRIP);
    glVertex2f(-0.15f, -0.3f);
    glVertex2f(-0.15f, 0.3f);
    glVertex2f(0.15f, -0.3f);
    glVertex2f(0.15f, 0.3f);
    glEnd();
}

void drawO() {
    glLineWidth(3.0f);
    drawBezier(0.0f, 0.3f, 0.2f, 0.3f, 0.2f, -0.3f, 0.0f, -0.3f);
    drawBezier(0.0f, -0.3f, -0.2f, -0.3f, -0.2f, 0.3f, 0.0f, 0.3f);
}

void drawP() {
    glLineWidth(3.0f);
    glBegin(GL_LINES);
    glVertex2f(-0.15f, 0.3f); glVertex2f(-0.15f, -0.3f);
    glVertex2f(-0.15f, 0.3f); glVertex2f(0.05f, 0.3f);
    glVertex2f(-0.15f, 0.0f); glVertex2f(0.05f, 0.0f);
    glEnd();
    drawBezier(0.05f, 0.3f, 0.2f, 0.3f, 0.2f, 0.0f, 0.05f, 0.0f);
}

void drawQ() {
    drawO();
    glBegin(GL_LINES);
    glVertex2f(0.05f, -0.1f); glVertex2f(0.2f, -0.35f);
    glEnd();
}

void drawR() {
    glLineWidth(3.0f);
    glBegin(GL_LINES);
    glVertex2f(-0.15f, 0.3f); glVertex2f(-0.15f, -0.3f);
    glVertex2f(-0.15f, 0.3f); glVertex2f(0.05f, 0.3f);
    glVertex2f(-0.15f, 0.0f); glVertex2f(0.05f, 0.0f);
    glVertex2f(0.0f, 0.0f); glVertex2f(0.15f, -0.3f);
    glEnd();
    drawBezier(0.05f, 0.3f, 0.2f, 0.3f, 0.2f, 0.0f, 0.05f, 0.0f);
}

void drawS() {
    glLineWidth(3.0f);
    drawBezier(0.15f, 0.25f, 0.15f, 0.4f, -0.15f, 0.4f, -0.15f, 0.1f);
    drawBezier(-0.15f, 0.1f, -0.15f, -0.1f, 0.15f, -0.1f, 0.15f, -0.25f);
    drawBezier(0.15f, -0.25f, 0.15f, -0.4f, -0.15f, -0.4f, -0.15f, -0.3f);
}

void drawT() {
    glLineWidth(3.0f);
    glBegin(GL_LINES);
    glVertex2f(-0.15f, 0.3f); glVertex2f(0.15f, 0.3f);
    glVertex2f(0.0f, 0.3f); glVertex2f(0.0f, -0.3f);
    glEnd();
}

void drawU() {
    glLineWidth(3.0f);
    glBegin(GL_LINES);
    glVertex2f(-0.15f, 0.3f); glVertex2f(-0.15f, -0.1f);
    glVertex2f(0.15f, 0.3f); glVertex2f(0.15f, -0.1f);
    glEnd();
    drawBezier(-0.15f, -0.1f, -0.15f, -0.3f, 0.15f, -0.3f, 0.15f, -0.1f);
}

void drawV() {
    glLineWidth(3.0f);
    glBegin(GL_LINE_STRIP);
    glVertex2f(-0.15f, 0.3f);
    glVertex2f(0.0f, -0.3f);
    glVertex2f(0.15f, 0.3f);
    glEnd();
}

void drawW() {
    glLineWidth(3.0f);
    glBegin(GL_LINE_STRIP);
    glVertex2f(-0.2f, 0.3f);
    glVertex2f(-0.1f, -0.3f);
    glVertex2f(0.0f, 0.0f);
    glVertex2f(0.1f, -0.3f);
    glVertex2f(0.2f, 0.3f);
    glEnd();
}

void drawX() {
    glLineWidth(3.0f);
    glBegin(GL_LINES);
    glVertex2f(-0.15f, 0.3f); glVertex2f(0.15f, -0.3f);
    glVertex2f(0.15f, 0.3f); glVertex2f(-0.15f, -0.3f);
    glEnd();
}

void drawY() {
    glLineWidth(3.0f);
    glBegin(GL_LINES);
    glVertex2f(-0.15f, 0.3f); glVertex2f(0.0f, 0.0f);
    glVertex2f(0.15f, 0.3f); glVertex2f(0.0f, 0.0f);
    glVertex2f(0.0f, 0.0f); glVertex2f(0.0f, -0.3f);
    glEnd();
}

void drawZ() {
    glLineWidth(3.0f);
    glBegin(GL_LINE_STRIP);
    glVertex2f(-0.15f, 0.3f);
    glVertex2f(0.15f, 0.3f);
    glVertex2f(-0.15f, -0.3f);
    glVertex2f(0.15f, -0.3f);
    glEnd();
}

void drawDigit(int d) {
}

void drawSpace() {
}
