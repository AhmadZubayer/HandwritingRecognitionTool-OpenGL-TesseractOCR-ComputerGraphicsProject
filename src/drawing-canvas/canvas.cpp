#define _CRT_SECURE_NO_WARNINGS
#include "canvas.h"
#include <iostream>
#include <algorithm>
#include <string>
#include <vector>
#include <cmath>
#include <cstring>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

//  Color helper 
static void applyColor(StrokeColor c, bool eraser) {
    if (eraser) { glColor3f(1, 1, 1); return; }
    switch (c) {
    case StrokeColor::BLACK: glColor3f(0.05f, 0.05f, 0.05f); break;
    case StrokeColor::BLUE:  glColor3f(0.10f, 0.30f, 0.90f); break;
    case StrokeColor::RED:   glColor3f(0.90f, 0.10f, 0.15f); break;
    }
}

//  Init 
void Canvas::init(GLFWwindow* window) {
    glfwGetFramebufferSize(window, &windowWidth, &windowHeight);
    glClearColor(1, 1, 1, 1);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
}

//  Coordinate helper 
CanvasPoint Canvas::screenToCanvas(float sx, float sy) {
    float cy = (float)windowHeight - sy;
    return { (sx - panX) / zoom, (cy - panY) / zoom };
}

//  Cubic Bezier interpolation 
CanvasPoint Canvas::cubicBezier(CanvasPoint p0, CanvasPoint p1, CanvasPoint p2, CanvasPoint p3, float t) {
    float omt = 1.0f - t;
    float x = omt * omt * omt * p0.x + 3 * t * omt * omt * p1.x
        + 3 * t * t * omt * p2.x + t * t * t * p3.x;
    float y = omt * omt * omt * p0.y + 3 * t * omt * omt * p1.y
        + 3 * t * t * omt * p2.y + t * t * t * p3.y;
    return { x, y };
}

//  Draw stroke using Bezier curves (smooth mode) 
void Canvas::drawStrokeSmooth(const Stroke& stroke) {
    auto& pts = stroke.points;
    if (pts.size() < 2) return;

    glLineWidth(stroke.thickness);
    applyColor(stroke.color, stroke.isEraser);
    glBegin(GL_LINE_STRIP);

    if (pts.size() < 4) {
        // Not enough points for cubic bezier — draw raw
        for (auto& p : pts) glVertex2f(p.x, p.y);
    }
    else {
        // Always start from first point
        glVertex2f(pts[0].x, pts[0].y);

        // Fit cubic bezier through every set of 4 points
        for (size_t i = 0; i + 3 < pts.size(); i += 3) {
            int segments = 20;
            for (int j = 1; j <= segments; j++) {
                float t = (float)j / (float)segments;
                CanvasPoint p = cubicBezier(pts[i], pts[i + 1],
                    pts[i + 2], pts[i + 3], t);
                glVertex2f(p.x, p.y);
            }
        }

        // Draw remaining points that don't fill a full group of 4
        size_t remainder = pts.size() % 3;
        if (remainder > 0) {
            size_t start = pts.size() - remainder;
            for (size_t i = start; i < pts.size(); i++)
                glVertex2f(pts[i].x, pts[i].y);
        }
    }
    glEnd();
}

//  Draw stroke raw (original mode) 
void Canvas::drawStrokeRaw(const Stroke& stroke) {
    if (stroke.points.size() < 2) return;
    glLineWidth(stroke.thickness);
    applyColor(stroke.color, stroke.isEraser);
    glBegin(GL_LINE_STRIP);
    for (auto& p : stroke.points) glVertex2f(p.x, p.y);
    glEnd();
}

// Mouse button 
void Canvas::onMouseButton(GLFWwindow* w, int button, int action, int mods) {
    double mx, my;
    glfwGetCursorPos(w, &mx, &my);

    if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
        isPanning = (action == GLFW_PRESS);
        lastMouseX = mx; lastMouseY = my;
        return;
    }

    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            isDrawing = true;
            currentStroke.points.clear();
            currentStroke.thickness = strokeThickness;
            currentStroke.color = currentColor;
            currentStroke.isEraser = isErasing;
            CanvasPoint p = screenToCanvas((float)mx, (float)my);
            currentStroke.points.push_back(p);
        }
        else if (action == GLFW_RELEASE && isDrawing) {
            isDrawing = false;
            if (!currentStroke.points.empty())
                strokes.push_back(currentStroke);
            currentStroke.points.clear();
        }
    }
}

//  Mouse move 
void Canvas::onMouseMove(GLFWwindow* w, double xpos, double ypos) {
    if (isPanning) {
        panX += (float)(xpos - lastMouseX);
        panY -= (float)(ypos - lastMouseY);
        lastMouseX = xpos; lastMouseY = ypos;
        return;
    }
    if (isDrawing) {
        CanvasPoint p = screenToCanvas((float)xpos, (float)ypos);
        currentStroke.points.push_back(p);
    }
    lastMouseX = xpos;
    lastMouseY = ypos;
}

//  Scroll zoom 
void Canvas::onScroll(GLFWwindow* w, double xoffset, double yoffset) {
    float factor = (yoffset > 0) ? 1.1f : 0.9f;
    double mx, my;
    glfwGetCursorPos(w, &mx, &my);
    float cy = (float)windowHeight - (float)my;
    panX = (float)mx - factor * ((float)mx - panX);
    panY = cy - factor * (cy - panY);
    zoom *= factor;
    zoom = std::max(0.1f, std::min(zoom, 20.0f));
}

//  Keyboard 
void Canvas::onKey(GLFWwindow* w, int key, int scancode, int action, int mods) {
    if (action != GLFW_PRESS && action != GLFW_REPEAT) return;

    // Colors
    if (key == GLFW_KEY_1 || key == GLFW_KEY_B) { currentColor = StrokeColor::BLACK; isErasing = false; }
    if (key == GLFW_KEY_2 || key == GLFW_KEY_U) { currentColor = StrokeColor::BLUE;  isErasing = false; }
    if (key == GLFW_KEY_3 || (key == GLFW_KEY_R && !(mods & GLFW_MOD_CONTROL))) {
        currentColor = StrokeColor::RED; isErasing = false;
    }
    // Tools
    if (key == GLFW_KEY_E) { isErasing = !isErasing; }
    if (key == GLFW_KEY_C) { clearCanvas(); }
    if (key == GLFW_KEY_Z && (mods & GLFW_MOD_CONTROL)) { undo(); }

    // Thickness
    if (key == GLFW_KEY_EQUAL || key == GLFW_KEY_KP_ADD)
        strokeThickness = std::min(strokeThickness + 1.0f, 30.0f);
    if (key == GLFW_KEY_MINUS || key == GLFW_KEY_KP_SUBTRACT)
        strokeThickness = std::max(strokeThickness - 1.0f, 1.0f);

    // Export
    if (key == GLFW_KEY_S && (mods & GLFW_MOD_CONTROL)) {
        showBoundingBox = true;
        if (exportToPNG("canvas_export.png"))
            std::cout << "[Export] Saved: canvas_export.png\n";
    }

    // Grid
    if (key == GLFW_KEY_G) { showGrid = !showGrid; }

    // ── Transformations ───────────────────────────────────────────────────────
// Rotation: [ and ] keys
    if (key == GLFW_KEY_LEFT_BRACKET)  applyRotation(-15.0f);
    if (key == GLFW_KEY_RIGHT_BRACKET) applyRotation(15.0f);

    // Scale: Shift+= grow, Shift+- shrink
    if (key == GLFW_KEY_EQUAL && (mods & GLFW_MOD_SHIFT)) applyScale(1.2f, 1.2f);
    if (key == GLFW_KEY_MINUS && (mods & GLFW_MOD_SHIFT)) applyScale(0.8f, 0.8f);

    // Translation: arrow keys
    if (key == GLFW_KEY_LEFT && !(mods & GLFW_MOD_CONTROL)) applyTranslation(-20.0f, 0.0f);
    if (key == GLFW_KEY_RIGHT && !(mods & GLFW_MOD_CONTROL)) applyTranslation(20.0f, 0.0f);
    if (key == GLFW_KEY_UP && !(mods & GLFW_MOD_CONTROL)) applyTranslation(0.0f, 20.0f);
    if (key == GLFW_KEY_DOWN && !(mods & GLFW_MOD_CONTROL)) applyTranslation(0.0f, -20.0f);

    //  NEW: Z key toggles Bezier smooth mode 
    if (key == GLFW_KEY_Z && !(mods & GLFW_MOD_CONTROL)) {
        smoothStrokes = !smoothStrokes;
        std::cout << "[Canvas] Smooth strokes: "
            << (smoothStrokes ? "ON (Bezier)" : "OFF (Raw)")
            << "\n";
    }
}

//  Clear 
void Canvas::clearCanvas() {
    strokes.clear();
    currentStroke.points.clear();
    showBoundingBox = false;
}

//  Undo 
void Canvas::undo() {
    if (!strokes.empty()) strokes.pop_back();
}

//  Grid 
void Canvas::drawGrid() {
    glLineWidth(0.5f);
    glColor4f(0.8f, 0.8f, 0.9f, 0.6f);
    float step = 40.0f;
    float x0 = -panX / zoom, y0 = -panY / zoom;
    float x1 = ((float)windowWidth - panX) / zoom;
    float y1 = ((float)windowHeight - panY) / zoom;
    float startX = x0 - fmodf(x0, step);
    for (float x = startX; x < x1; x += step) {
        glBegin(GL_LINES); glVertex2f(x, y0); glVertex2f(x, y1); glEnd();
    }
    float startY = y0 - fmodf(y0, step);
    for (float y = startY; y < y1; y += step) {
        glBegin(GL_LINES); glVertex2f(x0, y); glVertex2f(x1, y); glEnd();
    }
}

// Bounding box 
void Canvas::getStrokeBounds(float& minX, float& minY, float& maxX, float& maxY) {
    minX = minY = 1e9f;
    maxX = maxY = -1e9f;
    for (auto& s : strokes)
        for (auto& p : s.points) {
            minX = std::min(minX, p.x); maxX = std::max(maxX, p.x);
            minY = std::min(minY, p.y); maxY = std::max(maxY, p.y);
        }
}

void Canvas::drawBoundingBox() {
    if (strokes.empty()) return;
    float minX, minY, maxX, maxY;
    getStrokeBounds(minX, minY, maxX, maxY);
    float pad = 10.0f;
    minX -= pad; minY -= pad; maxX += pad; maxY += pad;
    glLineWidth(1.5f);
    glColor4f(0.2f, 0.6f, 1.0f, 0.8f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(minX, minY); glVertex2f(maxX, minY);
    glVertex2f(maxX, maxY); glVertex2f(minX, maxY);
    glEnd();
}

//  Cursor 
void Canvas::drawCursor(double mx, double my) {
    CanvasPoint p = screenToCanvas((float)mx, (float)my);
    float r = (strokeThickness / zoom) * 0.5f;
    glLineWidth(1.0f);
    glColor4f(0.3f, 0.3f, 0.3f, 0.5f);
    if (isErasing) glColor4f(1.0f, 0.3f, 0.3f, 0.7f);
    glBegin(GL_LINE_LOOP);
    int segs = 24;
    for (int i = 0; i < segs; i++) {
        float angle = 2.0f * 3.14159f * (float)i / (float)segs;
        glVertex2f(p.x + r * cosf(angle), p.y + r * sinf(angle));
    }
    glEnd();
}

//  Render 
void Canvas::render() {
    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, windowWidth, 0, windowHeight, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(panX, panY, 0);
    glScalef(zoom, zoom, 1);

    if (showGrid) drawGrid();

    //  Render completed strokes 
    for (auto& stroke : strokes) {
        if (smoothStrokes)
            drawStrokeSmooth(stroke);
        else
            drawStrokeRaw(stroke);
    }

    //  Render current in-progress stroke 
    if (isDrawing && currentStroke.points.size() >= 2) {
        if (smoothStrokes)
            drawStrokeSmooth(currentStroke);
        else
            drawStrokeRaw(currentStroke);
    }

    if (showBoundingBox) drawBoundingBox();
}

//  Export PNG 
bool Canvas::exportToPNG(const std::string& filename) {
    int w = windowWidth, h = windowHeight;
    std::vector<unsigned char> pixels((size_t)w * h * 3);
    glReadPixels(0, 0, w, h, GL_RGB, GL_UNSIGNED_BYTE, pixels.data());

    if (!strokes.empty()) {
        float minX, minY, maxX, maxY;
        getStrokeBounds(minX, minY, maxX, maxY);

        int sx1 = std::max(0, (int)(minX * zoom + panX) - 15);
        int sy1 = std::max(0, (int)(minY * zoom + panY) - 15);
        int sx2 = std::min(w, (int)(maxX * zoom + panX) + 15);
        int sy2 = std::min(h, (int)(maxY * zoom + panY) + 15);
        int cw = sx2 - sx1, ch = sy2 - sy1;

        if (cw > 0 && ch > 0) {
            std::vector<unsigned char> cropped((size_t)cw * ch * 3);
            for (int row = 0; row < ch; row++) {

                int srcRow = sy1 + (ch - 1 - row);
                memcpy(&cropped[(size_t)row * cw * 3],
                    &pixels[(size_t)srcRow * w * 3 + sx1 * 3],
                    (size_t)cw * 3);
            }
            return stbi_write_png(filename.c_str(), cw, ch, 3,
                cropped.data(), cw * 3) != 0;
        }
    }
    return stbi_write_png(filename.c_str(), w, h, 3,
        pixels.data(), w * 3) != 0;
}

// ─── Apply Translation to all strokes ────────────────────────────────────────
void Canvas::applyTranslation(float tx, float ty) {
    for (auto& stroke : strokes)
        for (auto& p : stroke.points) {
            p.x += tx;
            p.y += ty;
        }
}

// ─── Apply Rotation to all strokes ───────────────────────────────────────────
void Canvas::applyRotation(float degrees) {
    float rad = degrees * 3.14159f / 180.0f;
    float cosA = cosf(rad);
    float sinA = sinf(rad);

    // Find center of all strokes
    float minX, minY, maxX, maxY;
    getStrokeBounds(minX, minY, maxX, maxY);
    float cx = (minX + maxX) / 2.0f;
    float cy = (minY + maxY) / 2.0f;

    for (auto& stroke : strokes)
        for (auto& p : stroke.points) {
            float dx = p.x - cx;
            float dy = p.y - cy;
            p.x = cx + dx * cosA - dy * sinA;
            p.y = cy + dx * sinA + dy * cosA;
        }
}

// ─── Apply Scale to all strokes ───────────────────────────────────────────────
void Canvas::applyScale(float sx, float sy) {
    // Find center of all strokes
    float minX, minY, maxX, maxY;
    getStrokeBounds(minX, minY, maxX, maxY);
    float cx = (minX + maxX) / 2.0f;
    float cy = (minY + maxY) / 2.0f;

    for (auto& stroke : strokes)
        for (auto& p : stroke.points) {
            p.x = cx + (p.x - cx) * sx;
            p.y = cy + (p.y - cy) * sy;
        }
}

// ─── Reset all transforms ─────────────────────────────────────────────────────
void Canvas::resetTransforms() {
    rotationAngle = 0.0f;
    scaleFactorX = 1.0f;
    scaleFactorY = 1.0f;
}