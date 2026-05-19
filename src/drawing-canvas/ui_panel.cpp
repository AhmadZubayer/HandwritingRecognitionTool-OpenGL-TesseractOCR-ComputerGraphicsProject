#define _CRT_SECURE_NO_WARNINGS
#include "ui_panel.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <cstring>

// ─────────────────────────────────────────────────────────────────────────────
//  Tiny bitmap font — renders ASCII via GL_LINES (no external font needed)
//  Each char is drawn in a 6x8 cell; scale multiplies both axes.
// ─────────────────────────────────────────────────────────────────────────────
static void glLine(float x0,float y0,float x1,float y1){
    glVertex2f(x0,y0); glVertex2f(x1,y1);
}
static float CW = 6.0f, CH = 9.0f; // cell width / height at scale=1

static void drawBitmapChar(char c, float ox, float oy, float s){
    // s = scale; ox,oy = bottom-left of character cell
    glBegin(GL_LINES);
    switch(c){
    case 'A': glLine(0,0,2,4*s); glLine(2,4*s,4*s,0); glLine(s,2*s,3*s,2*s); break;
    case 'B': glLine(0,0,0,4*s); glLine(0,4*s,3*s,4*s); glLine(3*s,4*s,4*s,3.5f*s);
              glLine(4*s,3.5f*s,3*s,2*s); glLine(3*s,2*s,0,2*s);
              glLine(3*s,2*s,4*s,s); glLine(4*s,s,3*s,0); glLine(3*s,0,0,0); break;
    case 'C': glLine(4*s,3.5f*s,3*s,4*s); glLine(3*s,4*s,s,4*s);
              glLine(s,4*s,0,3*s); glLine(0,3*s,0,s); glLine(0,s,s,0);
              glLine(s,0,3*s,0); glLine(3*s,0,4*s,0.5f*s); break;
    case 'D': glLine(0,0,0,4*s); glLine(0,4*s,2*s,4*s); glLine(2*s,4*s,4*s,3*s);
              glLine(4*s,3*s,4*s,s); glLine(4*s,s,2*s,0); glLine(2*s,0,0,0); break;
    case 'E': glLine(0,0,0,4*s); glLine(0,4*s,4*s,4*s); glLine(0,2*s,3*s,2*s);
              glLine(0,0,4*s,0); break;
    case 'F': glLine(0,0,0,4*s); glLine(0,4*s,4*s,4*s); glLine(0,2*s,3*s,2*s); break;
    case 'G': glLine(4*s,3.5f*s,3*s,4*s); glLine(3*s,4*s,s,4*s);
              glLine(s,4*s,0,3*s); glLine(0,3*s,0,s); glLine(0,s,s,0);
              glLine(s,0,3*s,0); glLine(3*s,0,4*s,s); glLine(4*s,s,4*s,2*s);
              glLine(4*s,2*s,2.5f*s,2*s); break;
    case 'H': glLine(0,0,0,4*s); glLine(4*s,0,4*s,4*s); glLine(0,2*s,4*s,2*s); break;
    case 'I': glLine(s,0,3*s,0); glLine(2*s,0,2*s,4*s); glLine(s,4*s,3*s,4*s); break;
    case 'J': glLine(3*s,4*s,3*s,s); glLine(3*s,s,2*s,0); glLine(2*s,0,s,0);
              glLine(s,0,0,s); glLine(s,4*s,4*s,4*s); break;
    case 'K': glLine(0,0,0,4*s); glLine(0,2*s,4*s,4*s); glLine(0,2*s,4*s,0); break;
    case 'L': glLine(0,4*s,0,0); glLine(0,0,4*s,0); break;
    case 'M': glLine(0,0,0,4*s); glLine(0,4*s,2*s,2*s); glLine(2*s,2*s,4*s,4*s);
              glLine(4*s,4*s,4*s,0); break;
    case 'N': glLine(0,0,0,4*s); glLine(0,4*s,4*s,0); glLine(4*s,0,4*s,4*s); break;
    case 'O': glLine(s,4*s,3*s,4*s); glLine(3*s,4*s,4*s,3*s); glLine(4*s,3*s,4*s,s);
              glLine(4*s,s,3*s,0); glLine(3*s,0,s,0); glLine(s,0,0,s);
              glLine(0,s,0,3*s); glLine(0,3*s,s,4*s); break;
    case 'P': glLine(0,0,0,4*s); glLine(0,4*s,3*s,4*s); glLine(3*s,4*s,4*s,3.5f*s);
              glLine(4*s,3.5f*s,4*s,2.5f*s); glLine(4*s,2.5f*s,3*s,2*s);
              glLine(3*s,2*s,0,2*s); break;
    case 'Q': glLine(s,4*s,3*s,4*s); glLine(3*s,4*s,4*s,3*s); glLine(4*s,3*s,4*s,s);
              glLine(4*s,s,3*s,0); glLine(3*s,0,s,0); glLine(s,0,0,s);
              glLine(0,s,0,3*s); glLine(0,3*s,s,4*s); glLine(2.5f*s,s,4*s,0); break;
    case 'R': glLine(0,0,0,4*s); glLine(0,4*s,3*s,4*s); glLine(3*s,4*s,4*s,3.5f*s);
              glLine(4*s,3.5f*s,4*s,2.5f*s); glLine(4*s,2.5f*s,3*s,2*s);
              glLine(3*s,2*s,0,2*s); glLine(2*s,2*s,4*s,0); break;
    case 'S': glLine(4*s,3.5f*s,3*s,4*s); glLine(3*s,4*s,s,4*s);
              glLine(s,4*s,0,3*s); glLine(0,3*s,0,2.5f*s); glLine(0,2.5f*s,s,2*s);
              glLine(s,2*s,3*s,2*s); glLine(3*s,2*s,4*s,1.5f*s);
              glLine(4*s,1.5f*s,4*s,s); glLine(4*s,s,3*s,0); glLine(3*s,0,s,0);
              glLine(s,0,0,0.5f*s); break;
    case 'T': glLine(0,4*s,4*s,4*s); glLine(2*s,4*s,2*s,0); break;
    case 'U': glLine(0,4*s,0,s); glLine(0,s,s,0); glLine(s,0,3*s,0);
              glLine(3*s,0,4*s,s); glLine(4*s,s,4*s,4*s); break;
    case 'V': glLine(0,4*s,2*s,0); glLine(2*s,0,4*s,4*s); break;
    case 'W': glLine(0,4*s,s,0); glLine(s,0,2*s,2*s); glLine(2*s,2*s,3*s,0);
              glLine(3*s,0,4*s,4*s); break;
    case 'X': glLine(0,4*s,4*s,0); glLine(0,0,4*s,4*s); break;
    case 'Y': glLine(0,4*s,2*s,2*s); glLine(4*s,4*s,2*s,2*s); glLine(2*s,2*s,2*s,0); break;
    case 'Z': glLine(0,4*s,4*s,4*s); glLine(4*s,4*s,0,0); glLine(0,0,4*s,0); break;
    case 'a': glLine(s,3*s,3*s,3*s); glLine(3*s,3*s,4*s,2.5f*s); glLine(4*s,2.5f*s,4*s,0);
              glLine(4*s,s,3*s,0); glLine(3*s,0,s,0); glLine(s,0,0,s);
              glLine(0,s,s,2*s); glLine(s,2*s,3*s,2*s); glLine(4*s,0,4*s,0); break;
    case 'b': glLine(0,0,0,4*s); glLine(0,2*s,s,3*s); glLine(s,3*s,3*s,3*s);
              glLine(3*s,3*s,4*s,2*s); glLine(4*s,2*s,4*s,s); glLine(4*s,s,3*s,0);
              glLine(3*s,0,s,0); glLine(s,0,0,s); break;
    case 'c': glLine(4*s,2.5f*s,3*s,3*s); glLine(3*s,3*s,s,3*s); glLine(s,3*s,0,2*s);
              glLine(0,2*s,0,s); glLine(0,s,s,0); glLine(s,0,3*s,0);
              glLine(3*s,0,4*s,0.5f*s); break;
    case 'd': glLine(4*s,0,4*s,4*s); glLine(4*s,2*s,3*s,3*s); glLine(3*s,3*s,s,3*s);
              glLine(s,3*s,0,2*s); glLine(0,2*s,0,s); glLine(0,s,s,0);
              glLine(s,0,3*s,0); glLine(3*s,0,4*s,s); break;
    case 'e': glLine(0,1.5f*s,4*s,1.5f*s); glLine(4*s,1.5f*s,4*s,2*s);
              glLine(4*s,2*s,3*s,3*s); glLine(3*s,3*s,s,3*s); glLine(s,3*s,0,2*s);
              glLine(0,2*s,0,s); glLine(0,s,s,0); glLine(s,0,3*s,0);
              glLine(3*s,0,4*s,0.5f*s); break;
    case 'f': glLine(4*s,3.5f*s,3*s,4*s); glLine(3*s,4*s,2*s,4*s);
              glLine(2*s,4*s,s,3.5f*s); glLine(s,3.5f*s,s,0);
              glLine(0,2*s,3*s,2*s); break;
    case 'g': glLine(4*s,3*s,3*s,3*s); glLine(3*s,3*s,s,3*s); glLine(s,3*s,0,2*s);
              glLine(0,2*s,0,s); glLine(0,s,s,0); glLine(s,0,3*s,0);
              glLine(3*s,0,4*s,s); glLine(4*s,s,4*s,3*s);
              glLine(4*s,-s,3*s,-1.5f*s); glLine(3*s,-1.5f*s,s,-1.5f*s);
              glLine(s,-1.5f*s,0,-s); break;
    case 'h': glLine(0,0,0,4*s); glLine(0,2*s,s,3*s); glLine(s,3*s,3*s,3*s);
              glLine(3*s,3*s,4*s,2*s); glLine(4*s,2*s,4*s,0); break;
    case 'i': glLine(2*s,0,2*s,3*s); glLine(2*s,3.5f*s,2*s,4*s); break;
    case 'j': glLine(3*s,3*s,3*s,0); glLine(3*s,0,2*s,-s); glLine(2*s,-s,s,-s);
              glLine(s,-s,0,0); glLine(3*s,3.5f*s,3*s,4*s); break;
    case 'k': glLine(0,0,0,4*s); glLine(0,1.5f*s,4*s,3*s); glLine(0,1.5f*s,4*s,0); break;
    case 'l': glLine(s,4*s,2*s,4*s); glLine(1.5f*s,4*s,1.5f*s,0.5f*s);
              glLine(s,0,3*s,0); break;
    case 'm': glLine(0,0,0,3*s); glLine(0,3*s,s,3*s); glLine(s,3*s,2*s,2*s);
              glLine(2*s,2*s,2*s,0); glLine(2*s,3*s,3*s,3*s); glLine(3*s,3*s,4*s,2*s);
              glLine(4*s,2*s,4*s,0); break;
    case 'n': glLine(0,0,0,3*s); glLine(0,3*s,s,3*s); glLine(s,3*s,3*s,3*s);
              glLine(3*s,3*s,4*s,2*s); glLine(4*s,2*s,4*s,0); break;
    case 'o': glLine(s,3*s,3*s,3*s); glLine(3*s,3*s,4*s,2*s); glLine(4*s,2*s,4*s,s);
              glLine(4*s,s,3*s,0); glLine(3*s,0,s,0); glLine(s,0,0,s);
              glLine(0,s,0,2*s); glLine(0,2*s,s,3*s); break;
    case 'p': glLine(0,-1.5f*s,0,3*s); glLine(0,2*s,s,3*s); glLine(s,3*s,3*s,3*s);
              glLine(3*s,3*s,4*s,2*s); glLine(4*s,2*s,4*s,s); glLine(4*s,s,3*s,0);
              glLine(3*s,0,s,0); glLine(s,0,0,s); break;
    case 'q': glLine(4*s,-1.5f*s,4*s,3*s); glLine(4*s,2*s,3*s,3*s);
              glLine(3*s,3*s,s,3*s); glLine(s,3*s,0,2*s); glLine(0,2*s,0,s);
              glLine(0,s,s,0); glLine(s,0,3*s,0); glLine(3*s,0,4*s,s); break;
    case 'r': glLine(0,0,0,3*s); glLine(0,2.5f*s,s,3*s); glLine(s,3*s,3*s,3*s);
              glLine(3*s,3*s,4*s,2.5f*s); break;
    case 's': glLine(4*s,2.5f*s,3*s,3*s); glLine(3*s,3*s,s,3*s);
              glLine(s,3*s,0,2.5f*s); glLine(0,2.5f*s,0,2*s); glLine(0,2*s,s,1.5f*s);
              glLine(s,1.5f*s,3*s,1.5f*s); glLine(3*s,1.5f*s,4*s,s);
              glLine(4*s,s,4*s,0.5f*s); glLine(4*s,0.5f*s,3*s,0);
              glLine(3*s,0,s,0); glLine(s,0,0,0.5f*s); break;
    case 't': glLine(0,2*s,4*s,2*s); glLine(2*s,4*s,2*s,0.5f*s);
              glLine(2*s,0.5f*s,3*s,0); break;
    case 'u': glLine(0,3*s,0,s); glLine(0,s,s,0); glLine(s,0,3*s,0);
              glLine(3*s,0,4*s,s); glLine(4*s,s,4*s,3*s); break;
    case 'v': glLine(0,3*s,2*s,0); glLine(2*s,0,4*s,3*s); break;
    case 'w': glLine(0,3*s,s,0); glLine(s,0,2*s,1.5f*s); glLine(2*s,1.5f*s,3*s,0);
              glLine(3*s,0,4*s,3*s); break;
    case 'x': glLine(0,3*s,4*s,0); glLine(0,0,4*s,3*s); break;
    case 'y': glLine(0,3*s,2*s,1.5f*s); glLine(4*s,3*s,s,-1.5f*s); break;
    case 'z': glLine(0,3*s,4*s,3*s); glLine(4*s,3*s,0,0); glLine(0,0,4*s,0); break;
    case '0': glLine(s,4*s,3*s,4*s); glLine(3*s,4*s,4*s,3*s); glLine(4*s,3*s,4*s,s);
              glLine(4*s,s,3*s,0); glLine(3*s,0,s,0); glLine(s,0,0,s);
              glLine(0,s,0,3*s); glLine(0,3*s,s,4*s); glLine(s,3*s,3*s,s); break;
    case '1': glLine(s,3*s,2*s,4*s); glLine(2*s,4*s,2*s,0); glLine(s,0,3*s,0); break;
    case '2': glLine(0,3*s,s,4*s); glLine(s,4*s,3*s,4*s); glLine(3*s,4*s,4*s,3*s);
              glLine(4*s,3*s,4*s,2.5f*s); glLine(4*s,2.5f*s,0,0); glLine(0,0,4*s,0); break;
    case '3': glLine(0,3.5f*s,s,4*s); glLine(s,4*s,3*s,4*s); glLine(3*s,4*s,4*s,3*s);
              glLine(4*s,3*s,4*s,2.5f*s); glLine(4*s,2.5f*s,2*s,2*s);
              glLine(2*s,2*s,4*s,1.5f*s); glLine(4*s,1.5f*s,4*s,s);
              glLine(4*s,s,3*s,0); glLine(3*s,0,s,0); glLine(s,0,0,0.5f*s); break;
    case '4': glLine(3*s,0,3*s,4*s); glLine(3*s,4*s,0,1.5f*s); glLine(0,1.5f*s,4*s,1.5f*s); break;
    case '5': glLine(4*s,4*s,0,4*s); glLine(0,4*s,0,2.5f*s); glLine(0,2.5f*s,3*s,2.5f*s);
              glLine(3*s,2.5f*s,4*s,2*s); glLine(4*s,2*s,4*s,s); glLine(4*s,s,3*s,0);
              glLine(3*s,0,s,0); glLine(s,0,0,0.5f*s); break;
    case '6': glLine(4*s,3.5f*s,3*s,4*s); glLine(3*s,4*s,s,4*s); glLine(s,4*s,0,3*s);
              glLine(0,3*s,0,s); glLine(0,s,s,0); glLine(s,0,3*s,0);
              glLine(3*s,0,4*s,s); glLine(4*s,s,4*s,2*s); glLine(4*s,2*s,3*s,2.5f*s);
              glLine(3*s,2.5f*s,s,2.5f*s); glLine(s,2.5f*s,0,2*s); break;
    case '7': glLine(0,4*s,4*s,4*s); glLine(4*s,4*s,2*s,0); break;
    case '8': glLine(s,2*s,3*s,2*s); glLine(0,3*s,s,4*s); glLine(s,4*s,3*s,4*s);
              glLine(3*s,4*s,4*s,3*s); glLine(4*s,3*s,4*s,2.5f*s);
              glLine(4*s,2.5f*s,3*s,2*s); glLine(3*s,2*s,4*s,1.5f*s);
              glLine(4*s,1.5f*s,4*s,s); glLine(4*s,s,3*s,0); glLine(3*s,0,s,0);
              glLine(s,0,0,s); glLine(0,s,0,1.5f*s); glLine(0,1.5f*s,s,2*s);
              glLine(s,2*s,0,2.5f*s); glLine(0,2.5f*s,0,3*s); break;
    case '9': glLine(4*s,s,3*s,0); glLine(3*s,0,s,0); glLine(s,0,0,s);
              glLine(0,s,0,2*s); glLine(0,2*s,s,2.5f*s); glLine(s,2.5f*s,3*s,2.5f*s);
              glLine(3*s,2.5f*s,4*s,2*s); glLine(4*s,2*s,4*s,s);
              glLine(4*s,4*s,4*s,2*s); glLine(4*s,4*s,3*s,4*s);
              glLine(3*s,4*s,s,4*s); glLine(s,4*s,0,3*s); glLine(0,3*s,0,2.5f*s); break;
    case '.': glLine(2*s,0,2*s,0.5f*s); break;
    case ',': glLine(2*s,0,1.5f*s,-s); break;
    case '!': glLine(2*s,4*s,2*s,1.5f*s); glLine(2*s,0.5f*s,2*s,0); break;
    case '?': glLine(0,3*s,s,4*s); glLine(s,4*s,3*s,4*s); glLine(3*s,4*s,4*s,3*s);
              glLine(4*s,3*s,4*s,2.5f*s); glLine(4*s,2.5f*s,2*s,1.5f*s);
              glLine(2*s,1.5f*s,2*s,s); glLine(2*s,0.3f*s,2*s,0); break;
    case ':': glLine(2*s,2.5f*s,2*s,3*s); glLine(2*s,s,2*s,1.5f*s); break;
    case '/': glLine(0,0,4*s,4*s); break;
    case '-': glLine(s,2*s,3*s,2*s); break;
    case '_': glLine(0,0,4*s,0); break;
    case '(': glLine(3*s,4*s,s,3*s); glLine(s,3*s,s,s); glLine(s,s,3*s,0); break;
    case ')': glLine(s,4*s,3*s,3*s); glLine(3*s,3*s,3*s,s); glLine(3*s,s,s,0); break;
    case ' ': break;  // space — no lines
    case '\n': break;
    default:  // unknown — draw small dot
        glLine(2*s,2*s,2*s,2.1f*s); break;
    }
    glEnd();
    // Translate for next char (caller handles ox advance)
}

// ─────────────────────────────────────────────────────────────────────────────

void UIPanel::init(int winW, int winH) {
    windowW = winW;
    windowH = winH;
    panelX  = (float)(winW - (int)panelW);
    panelH  = (float)winH;
}

// ─── Draw helpers ─────────────────────────────────────────────────────────────

void UIPanel::drawRect(float x,float y,float w,float h,
                       float r,float g,float b,float a){
    glColor4f(r,g,b,a);
    glBegin(GL_QUADS);
    glVertex2f(x,y); glVertex2f(x+w,y);
    glVertex2f(x+w,y+h); glVertex2f(x,y+h);
    glEnd();
}

void UIPanel::drawRectOutline(float x,float y,float w,float h,
                              float r,float g,float b,float a,float lw){
    glLineWidth(lw);
    glColor4f(r,g,b,a);
    glBegin(GL_LINE_LOOP);
    glVertex2f(x,y); glVertex2f(x+w,y);
    glVertex2f(x+w,y+h); glVertex2f(x,y+h);
    glEnd();
}

float UIPanel::charWidth(float scale){ return (CW+3)*scale; }

void UIPanel::drawText(const std::string& text, float x, float y,
                       float r, float g, float b, float scale){
    glLineWidth(1.3f*scale);
    glColor3f(r,g,b);
    float cx = x;
    for(char c : text){
        if(c=='\n'){ cx=x; y-=(CH+4)*scale; continue; }
        glPushMatrix();
        glTranslatef(cx, y, 0);
        drawBitmapChar(c, cx, y, scale*2.2f);
        glPopMatrix();
        cx += charWidth(scale);
    }
}

bool UIPanel::pointInRect(double px,double py,float rx,float ry,float rw,float rh){
    return px>=rx && px<=rx+rw && py>=ry && py<=ry+rh;
}

// ─── Draw a single button ─────────────────────────────────────────────────────
void UIPanel::drawButton(const UIButton& btn){
    float br=0.22f,bg=0.22f,bb=0.25f;
    if(btn.hovered){ br=0.30f;bg=0.30f;bb=0.35f; }
    if(btn.pressed){ br=0.15f;bg=0.40f;bb=0.60f; }
    drawRect(btn.x,btn.y,btn.w,btn.h,br,bg,bb,1.0f);
    drawRectOutline(btn.x,btn.y,btn.w,btn.h,0.5f,0.5f,0.6f,0.8f,1.0f);
    // Center text
    float tw = (float)btn.label.size()*charWidth(0.95f);
    float tx = btn.x + (btn.w - tw)*0.5f;
    float ty = btn.y + btn.h*0.35f;
    drawText(btn.label, tx, ty, 0.85f,0.90f,0.95f, 0.95f);
}

// ─── Draw a single slider ─────────────────────────────────────────────────────
void UIPanel::drawSlider(const UISlider& sl){
    // Track
    float trackY = sl.y + 5;
    drawRect(sl.x, trackY, sl.w, 4, 0.3f,0.3f,0.35f,1.0f);
    // Fill
    float frac = (sl.value - sl.minVal) / (sl.maxVal - sl.minVal);
    drawRect(sl.x, trackY, sl.w*frac, 4, 0.25f,0.55f,0.85f,1.0f);
    // Knob
    float kx = sl.x + sl.w*frac - 6;
    drawRect(kx, sl.y, 12, 14, 0.7f,0.85f,1.0f,1.0f);
    // Label + value
    std::string lbl = sl.label + ": ";
    if(sl.label == "Contrast")
        lbl += std::to_string((int)(sl.value*100)/100.0f).substr(0,4);
    else
        lbl += (sl.value == sl.minVal ? "Auto" : std::to_string((int)sl.value));
    drawText(lbl, sl.x, sl.y+22, 0.65f,0.70f,0.75f, 0.85f);
    }


// ─── Feature 18: Processing indicator ────────────────────────────────────────
void UIPanel::drawProcessingIndicator(int winW, int winH){
    float cx = panelX + panelW*0.5f;
    float cy = (float)winH * 0.5f;
    // Dim overlay
    drawRect(panelX, toolbarH, panelW, (float)winH-toolbarH, 0.1f,0.1f,0.12f,0.7f);
    // Box
    drawRect(cx-70, cy-20, 140, 40, 0.18f,0.18f,0.22f,1.0f);
    drawRectOutline(cx-70, cy-20, 140, 40, 0.3f,0.6f,1.0f,1.0f, 1.5f);
    drawText("Processing...", cx-55, cy-6, 0.3f,0.7f,1.0f, 0.9f);
}

// ─── Toolbar (Feature 1, 2, 8-10) ────────────────────────────────────────────
void UIPanel::drawToolbar(int winW, int winH) {
    // Background
    drawRect(0, (float)winH - toolbarH, (float)winW, toolbarH, 0.12f,0.12f,0.15f,1.0f);
    drawRect(0, (float)winH - toolbarH - 1, (float)winW, 1, 0.25f,0.50f,0.85f,1.0f);

    // Draw toolbar buttons
    for(auto& btn : buttons) drawButton(btn);
}

// ─── Result panel (Features 3,4,5,6,7) ───────────────────────────────────────
void UIPanel::drawResultPanel(int winW, int winH){
    // Background
    drawRect(panelX, 0, panelW, (float)winH - toolbarH, 0.10f,0.10f,0.12f,1.0f);
    // Left border accent
    drawRect(panelX, 0, 2, (float)winH - toolbarH, 0.25f,0.50f,0.85f,1.0f);

    // Header
    drawRect(panelX+2, (float)winH-toolbarH-30, panelW-2, 30, 0.14f,0.14f,0.18f,1.0f);
    drawText("OCR RESULT", panelX+12, (float)winH-toolbarH-20, 0.4f,0.7f,1.0f, 1.1f);

    // Feature 7: word/char count
    if(hasResult){
        std::string info = "W:" + std::to_string(currentResult.wordCount)
                         + " C:" + std::to_string(currentResult.charCount)
                         + " T:" + std::to_string((int)currentResult.processingTimeMs) + "ms";
        float iw = (float)info.size()*charWidth(0.75f);
        drawText(info, panelX+panelW-iw-8, (float)winH-toolbarH-20,
                 0.45f,0.55f,0.65f, 0.75f);
    }

    // Scrollable text area
    float areaTop    = (float)winH - toolbarH - 32;
    float areaBottom = 300.0f;  // above sliders
    float areaH      = areaTop - areaBottom;

    // Clip region (scissor)
    glEnable(GL_SCISSOR_TEST);
    glScissor((int)panelX+2, (int)areaBottom, (int)panelW-4, (int)areaH);

    if(!hasResult && !isProcessing){
        drawText("Draw on canvas,", panelX+16, areaTop-25-scrollOffset,
                 0.35f,0.35f,0.40f, 0.95f);
        drawText("then press Ctrl+R", panelX+16, areaTop-48-scrollOffset,
                 0.35f,0.35f,0.40f, 0.95f);
        drawText("to recognize.", panelX+16, areaTop-71-scrollOffset,
                 0.35f,0.35f,0.40f, 0.95f);
    } else if(hasResult){
        float lineH  = (float)(fontSize + 6);
        float scale  = (float)fontSize / 12.0f;
        float ty     = areaTop - 8 - scrollOffset;

        // Feature 4: highlight low-confidence words in red
        // Draw full text line by line, coloring low-confidence words
        std::istringstream ss(currentResult.fullText);
        std::string line;
        while(std::getline(ss, line)){
            float tx = panelX + 10;
            // Split line into words and match confidence
            std::istringstream ws(line);
            std::string word;
            while(ws >> word){
                bool lowConf = false;
                for(auto& wr : currentResult.words){
                    if(wr.text.find(word) != std::string::npos && wr.lowConfidence()){
                        lowConf = true; break;
                    }
                }
                float wr_val = lowConf ? 1.0f : 0.88f;
                float wg_val = lowConf ? 0.3f : 0.88f;
                float wb_val = lowConf ? 0.3f : 0.88f;
                drawText(word, tx, ty, wr_val, wg_val, wb_val, scale);
                tx += (float)(word.size()+1) * charWidth(scale);
                if(tx > panelX + panelW - 20){ tx = panelX+10; ty -= lineH; }
            }
            ty -= lineH;
        }
        // Update max scroll
        float contentH = (areaTop - ty);
        maxScroll = std::max(0.0f, contentH - areaH + 20);
    }

    glDisable(GL_SCISSOR_TEST);

    // Sliders area (Features 17, 18)
    float slY = 92.0f;
    for(auto& sl : sliders) drawSlider(sl);

    // Copy + Save buttons at bottom
    // (These are already in the buttons array, drawn in toolbar)

    // Scroll bar
    if(maxScroll > 0){
        float sbH   = areaH;
        float thumbH= std::max(20.0f, sbH * (areaH / (areaH + maxScroll)));
        float thumbY= areaBottom + (sbH - thumbH) * (1.0f - scrollOffset/maxScroll);
        drawRect(panelX+panelW-8, areaBottom, 6, areaH, 0.2f,0.2f,0.22f,1.0f);
        drawRect(panelX+panelW-7, thumbY, 4, thumbH, 0.4f,0.55f,0.75f,1.0f);
    }
}

// ─── Main render ─────────────────────────────────────────────────────────────
void UIPanel::render(int winW, int winH){
    windowW = winW; windowH = winH;
    panelX  = (float)(winW - (int)panelW);

    // Setup 2D ortho (y=0 at bottom)
    glMatrixMode(GL_PROJECTION);
    glPushMatrix(); glLoadIdentity();
    glOrtho(0, winW, 0, winH, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix(); glLoadIdentity();

    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    drawToolbar(winW, winH);
    drawResultPanel(winW, winH);
    drawPreviewImage();
    if(isProcessing) drawProcessingIndicator(winW, winH);

    glMatrixMode(GL_PROJECTION); glPopMatrix();
    glMatrixMode(GL_MODELVIEW);  glPopMatrix();
}

// ─── Input: mouse button ──────────────────────────────────────────────────────
void UIPanel::onMouseButton(GLFWwindow* w, int button, int action,
                            int mods, double mx, double my){
    // Flip y (GLFW y=0 at top; our y=0 at bottom)
    double fy = windowH - my;

    if(button == GLFW_MOUSE_BUTTON_LEFT){
        if(action == GLFW_PRESS){
            // Check buttons
            for(auto& btn : buttons){
                if(pointInRect(mx, fy, btn.x, btn.y, btn.w, btn.h)){
                    btn.pressed = true;
                }
            }
            // Check sliders
            for(int i=0;i<(int)sliders.size();i++){
                auto& sl = sliders[i];
                if(pointInRect(mx, fy, sl.x, sl.y, sl.w, 14)){
                    draggingSlider = i;
                }
            }
        } else { // RELEASE
            for(auto& btn : buttons){
                if(btn.pressed && pointInRect(mx, fy, btn.x, btn.y, btn.w, btn.h)){
                    if(btn.onClick) btn.onClick();
                }
                btn.pressed = false;
            }
            draggingSlider = -1;
        }
    }
}

// ─── Input: mouse move ────────────────────────────────────────────────────────
void UIPanel::onMouseMove(GLFWwindow* w, double mx, double my){
    double fy = windowH - my;
    for(auto& btn : buttons)
        btn.hovered = pointInRect(mx, fy, btn.x, btn.y, btn.w, btn.h);

    if(draggingSlider >= 0){
        auto& sl = sliders[draggingSlider];
        float frac = std::max(0.0f, std::min(1.0f,
                     (float)(mx - sl.x) / sl.w));
        sl.value = sl.minVal + frac*(sl.maxVal - sl.minVal);
        if(sl.onChange) sl.onChange(sl.value);
    }
}

// ─── Input: scroll ────────────────────────────────────────────────────────────
void UIPanel::onScroll(double xoff, double yoff, double mx, double my){
    double fy = windowH - my;
    // Only scroll if mouse is inside result panel
    if(mx >= panelX){
        scrollOffset -= (float)yoff * 20.0f;
        scrollOffset  = std::max(0.0f, std::min(scrollOffset, maxScroll));
    }
}

// ─── Input: keyboard ─────────────────────────────────────────────────────────
void UIPanel::onKey(int key, int mods, int action){
    if(action != GLFW_PRESS) return;
    // Feature 5: font size cycling with F key
    if(key == GLFW_KEY_F){
        int sizes[] = {10,14,18,22};
        for(int i=0;i<4;i++){
            if(fontSize==sizes[i]){ fontSize=sizes[(i+1)%4]; break; }
        }
    }
}

// ─── Set result ───────────────────────────────────────────────────────────────
void UIPanel::setResult(const OCRResult& result){
    currentResult = result;
    hasResult     = (result.success && !result.fullText.empty());
    isProcessing  = false;
    scrollOffset  = 0;
}

// ─── Feature 6: Save text to file ────────────────────────────────────────────
bool UIPanel::saveTextToFile(const std::string& path){
    if(!hasResult) return false;
    std::ofstream f(path);
    if(!f.is_open()) return false;
    f << currentResult.fullText;
    f << "\n\n--- Stats ---\n";
    f << "Words: "    << currentResult.wordCount << "\n";
    f << "Chars: "    << currentResult.charCount << "\n";
    f << "Time (ms): "<< (int)currentResult.processingTimeMs << "\n";
    f.close();
    std::cout << "[UI] Text saved to: " << path << "\n";
    return true;
}

// ─── Feature 20: Copy to clipboard ───────────────────────────────────────────
void UIPanel::copyToClipboard(GLFWwindow* w){
    if(!hasResult) return;
    glfwSetClipboardString(w, currentResult.fullText.c_str());
    std::cout << "[UI] Text copied to clipboard.\n";
}

// ─── Feature 20: Write test log ──────────────────────────────────────────────
void UIPanel::writeTestLog(const std::string& input, const OCRResult& result){
    std::ofstream log("ocr_test_log.txt", std::ios::app);
    if(!log.is_open()) return;
    log << "=== Test Case ===\n";
    log << "Input image: " << input << "\n";
    log << "OCR Output:  " << result.fullText << "\n";
    log << "Words: " << result.wordCount
        << "  Chars: " << result.charCount
        << "  Time: "  << (int)result.processingTimeMs << "ms\n";
    log << "Word confidences:\n";
    for(auto& wr : result.words)
        log << "  [" << (int)wr.confidence << "%] " << wr.text << "\n";
    log << "\n";
    std::cout << "[UI] Test log written.\n";
}

// ─── Feature 19: Preprocessed Image Preview ──────────────────────────────────
void UIPanel::setPreviewImage(const cv::Mat& img) {
    if (img.empty()) return;

    // Delete old texture if we already have one
    if (previewTexture != 0) glDeleteTextures(1, &previewTexture);

    previewImgW = img.cols;
    previewImgH = img.rows;

    glGenTextures(1, &previewTexture);
    glBindTexture(GL_TEXTURE_2D, previewTexture);

    // Set texture scaling parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // OpenCV grayscale is 1 channel, color is 3
    GLenum format = (img.channels() == 1) ? GL_LUMINANCE : GL_BGR;

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, previewImgW, previewImgH, 0, format, GL_UNSIGNED_BYTE, img.data);
}

void UIPanel::drawPreviewImage() {
    if (previewTexture == 0) return;

    float previewBoxSize = 180.0f;
    float px = panelX + (panelW - previewBoxSize) * 0.5f;
    float py = 110.0f; // Sits right above the contrast/threshold sliders

    // Draw Title and Border
    drawText("Preprocessed Image", px + 15, py + previewBoxSize + 8, 0.4f, 0.7f, 1.0f, 0.85f);
    drawRectOutline(px - 2, py - 2, previewBoxSize + 4, previewBoxSize + 4, 0.3f, 0.3f, 0.4f, 1.0f, 1.5f);

    // Calculate aspect ratio to fit image inside the box without stretching
    float aspect = (float)previewImgW / previewImgH;
    float dw = previewBoxSize, dh = previewBoxSize;
    if (aspect > 1.0f) dh = previewBoxSize / aspect;
    else dw = previewBoxSize * aspect;

    float dx = px + (previewBoxSize - dw) * 0.5f;
    float dy = py + (previewBoxSize - dh) * 0.5f;

    // Draw the textured quad
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, previewTexture);
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

    glBegin(GL_QUADS);
    // Note: OpenGL origin is bottom-left, OpenCV is top-left, so we flip the V coordinates (0 to 1)
    glTexCoord2f(0.0f, 1.0f); glVertex2f(dx, dy);
    glTexCoord2f(1.0f, 1.0f); glVertex2f(dx + dw, dy);
    glTexCoord2f(1.0f, 0.0f); glVertex2f(dx + dw, dy + dh);
    glTexCoord2f(0.0f, 0.0f); glVertex2f(dx, dy + dh);
    glEnd();

    glDisable(GL_TEXTURE_2D);
}

// ─── Build buttons and sliders (call once after init) ────────────────────────
// This is called from main.cpp after UIPanel::init()
void buildUI(UIPanel& ui, GLFWwindow* win,
             std::function<void()> onRecognize,
             std::function<void()> onClear,
             std::function<void()> onUndo,
             std::function<void()> onExport,
             std::function<void()> onSaveText,
             std::function<void()> onCopyClipboard,
             std::function<void(float)> onContrast,
             std::function<void(int)>   onThreshold)
{
    int winW, winH;
    glfwGetFramebufferSize(win, &winW, &winH);

    float tbY  = (float)winH - ui.toolbarH + 6;
    float bH   = 34.0f;
    float bW   = 90.0f;
    float gap  = 6.0f;
    float bx   = 8.0f;

    // Feature 2: Recognize button (Ctrl+R)
    UIButton btnRecognize;
    btnRecognize.label = "Recognize"; btnRecognize.x=bx; btnRecognize.y=tbY;
    btnRecognize.w=bW; btnRecognize.h=bH; btnRecognize.onClick=onRecognize;
    ui.buttons.push_back(btnRecognize); bx+=bW+gap;

    // Clear button
    UIButton btnClear;
    btnClear.label="Clear"; btnClear.x=bx; btnClear.y=tbY;
    btnClear.w=70; btnClear.h=bH; btnClear.onClick=onClear;
    ui.buttons.push_back(btnClear); bx+=70+gap;

    // Undo button
    UIButton btnUndo;
    btnUndo.label="Undo"; btnUndo.x=bx; btnUndo.y=tbY;
    btnUndo.w=70; btnUndo.h=bH; btnUndo.onClick=onUndo;
    ui.buttons.push_back(btnUndo); bx+=70+gap;

    // Export PNG button (Ctrl+S)
    UIButton btnExport;
    btnExport.label="Export PNG"; btnExport.x=bx; btnExport.y=tbY;
    btnExport.w=bW; btnExport.h=bH; btnExport.onClick=onExport;
    ui.buttons.push_back(btnExport); bx+=bW+gap;

    // Feature 6: Save text button
    UIButton btnSave;
    btnSave.label="Save Text"; btnSave.x=bx; btnSave.y=tbY;
    btnSave.w=bW; btnSave.h=bH; btnSave.onClick=onSaveText;
    ui.buttons.push_back(btnSave); bx+=bW+gap;

    // Feature 20: Copy button
    UIButton btnCopy;
    btnCopy.label="Copy"; btnCopy.x=bx; btnCopy.y=tbY;
    btnCopy.w=65; btnCopy.h=bH; btnCopy.onClick=onCopyClipboard;
    ui.buttons.push_back(btnCopy); bx+=65+gap;

    // Feature 5: Font size button
    UIButton btnFont;
    btnFont.label="Font+"; btnFont.x=bx; btnFont.y=tbY;
    btnFont.w=65; btnFont.h=bH;
    btnFont.onClick=[&ui](){ ui.onKey(GLFW_KEY_F,0,GLFW_PRESS); };
    ui.buttons.push_back(btnFont);

    // Feature 17: Contrast slider
    UISlider slContrast;
    slContrast.label="Contrast"; slContrast.x=(float)(winW-340); slContrast.y=60;
    slContrast.w=160; slContrast.minVal=0.5f; slContrast.maxVal=3.0f;
    slContrast.value=1.0f;
    slContrast.onChange=[onContrast](float v){ onContrast(v); };
    ui.sliders.push_back(slContrast);

    // Feature 18: Threshold slider (0=Otsu auto)
    UISlider slThresh;
    slThresh.label="Threshold"; slThresh.x=(float)(winW-340); slThresh.y=30;
    slThresh.w=160; slThresh.minVal=0; slThresh.maxVal=255;
    slThresh.value=0;
    slThresh.onChange=[onThreshold](float v){ onThreshold((int)v); };
    ui.sliders.push_back(slThresh);
}
