#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <string>

struct CanvasPoint { float x, y; };
enum class StrokeColor { BLACK, BLUE, RED };

struct Stroke {
    std::vector<CanvasPoint> points;
    float thickness;
    StrokeColor color;
    bool isEraser;
};

class Canvas {
public:
    int   windowWidth = 1200;
    int   windowHeight = 800;
    bool  isDrawing = false;
    bool  isErasing = false;
    float strokeThickness = 3.0f;
    StrokeColor currentColor = StrokeColor::BLACK;
    float panX = 0.0f, panY = 0.0f;
    float zoom = 1.0f;
    bool  isPanning = false;
    double lastMouseX = 0, lastMouseY = 0;
    bool  showGrid = false;
    bool  showBoundingBox = false;
    bool  smoothStrokes = true;   // Feature: Bezier smooth toggle (Z key)

    float rotationAngle = 0.0f;
    float scaleFactorX = 1.0f;
    float scaleFactorY = 1.0f;
    float translateX = 0.0f;
    float translateY = 0.0f;

    void applyRotation(float degrees);
    void applyScale(float sx, float sy);
    void applyTranslation(float tx, float ty);
    void resetTransforms();

    std::vector<Stroke> strokes;
    Stroke currentStroke;

    void  init(GLFWwindow* window);
    void  onMouseButton(GLFWwindow* w, int button, int action, int mods);
    void  onMouseMove(GLFWwindow* w, double xpos, double ypos);
    void  onScroll(GLFWwindow* w, double xoffset, double yoffset);
    void  onKey(GLFWwindow* w, int key, int scancode, int action, int mods);
    void  render();
    void  clearCanvas();
    void  undo();
    bool  exportToPNG(const std::string& filename);
    void  drawCursor(double mx, double my);
    void  drawGrid();
    void  drawBoundingBox();
    CanvasPoint screenToCanvas(float sx, float sy);
    void  getStrokeBounds(float& minX, float& minY, float& maxX, float& maxY);

    // Bezier helper
    CanvasPoint cubicBezier(CanvasPoint p0, CanvasPoint p1, CanvasPoint p2, CanvasPoint p3, float t);
    void  drawStrokeSmooth(const Stroke& stroke);
    void  drawStrokeRaw(const Stroke& stroke);
};