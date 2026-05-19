#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>
#include <vector>
#include <functional>
#include "ocr_engine.h"

// ─── Button ──────────────────────────────────────────────────────────────────
struct UIButton {
    float x, y, w, h;
    std::string label;
    bool  hovered = false;
    bool  pressed = false;
    std::function<void()> onClick;
};

// ─── Slider ──────────────────────────────────────────────────────────────────
struct UISlider {
    float x, y, w;
    float minVal, maxVal, value;
    std::string label;
    bool dragging = false;
    std::function<void(float)> onChange;
};

// ─── UIPanel ─────────────────────────────────────────────────────────────────
class UIPanel {
public:

    std::vector<UIButton> buttons;
    std::vector<UISlider> sliders;


    // Panel dimensions (right-side result panel)
    float panelX     = 850.0f;   // updated on window resize
    float panelY     = 0.0f;
    float panelW     = 350.0f;
    float panelH     = 800.0f;

    // Toolbar height at top
    float toolbarH   = 48.0f;

    // Scroll state for result panel
    float scrollOffset   = 0.0f;
    float maxScroll      = 0.0f;

    // Font size for result text (Feature 5)
    int   fontSize       = 14;   // 10 / 14 / 18 / 22

    // OCR result to display
    OCRResult currentResult;
    bool      hasResult    = false;
    bool      isProcessing = false;   // Feature 18

    // Contrast & threshold sliders (Features 17, 18)
    float contrastValue  = 1.0f;
    int   thresholdValue = 0;         // 0 = Otsu auto

    // Callbacks set by main
    std::function<void()>        onRecognize;
    std::function<void()>        onClear;
    std::function<void()>        onUndo;
    std::function<void()>        onExport;
    std::function<void()>        onSaveText;      // Feature 6
    std::function<void()>        onCopyClipboard; // Feature 20
    std::function<void(float)>   onContrastChange;
    std::function<void(int)>     onThresholdChange;

    // Init — call after window is created
    void init(int winW, int winH);

    // Call every frame AFTER canvas renders
    void render(int winW, int winH);

    // Input handlers — call from GLFW callbacks
    void onMouseButton(GLFWwindow* w, int button, int action, int mods,
                       double mx, double my);
    void onMouseMove(GLFWwindow* w, double mx, double my);
    void onScroll(double xoff, double yoff, double mx, double my);
    void onKey(int key, int mods, int action);

    // Update result display
    void setResult(const OCRResult& result);
    void setProcessing(bool val) { isProcessing = val; }

    // Feature 7: char/word count
    int getWordCount() const { return currentResult.wordCount; }
    int getCharCount() const { return currentResult.charCount; }

    // Save recognized text to .txt file (Feature 6)
    bool saveTextToFile(const std::string& path);

    // Copy to clipboard (Feature 20)
    void copyToClipboard(GLFWwindow* w);

    // Write test log (Feature 20)
    void writeTestLog(const std::string& input, const OCRResult& result);

    // --- Feature 19: Preprocessed Image Preview ---
    void setPreviewImage(const cv::Mat& img);
    void drawPreviewImage();

private:


    // Internal draw helpers
    void drawToolbar(int winW, int winH);
    void drawResultPanel(int winW, int winH);
    void drawProcessingIndicator(int winW, int winH);
    void drawButton(const UIButton& btn);
    void drawSlider(const UISlider& sl);
    void drawRect(float x, float y, float w, float h,
                  float r, float g, float b, float a);
    void drawRectOutline(float x, float y, float w, float h,
                         float r, float g, float b, float a, float lw = 1.0f);
    void drawText(const std::string& text, float x, float y,
                  float r, float g, float b, float scale = 1.0f);
    void drawWrappedText(const std::string& text, float x, float y,
                         float maxW, float lineH,
                         float r, float g, float b,
                         float clipY0, float clipY1,
                         const std::vector<WordResult>& words);

    // Bitmap font rendering (built-in GLUT-style using GL lines)
    void renderChar(char c, float& cx, float cy, float scale);
    float charWidth(float scale);

    bool pointInRect(double px, double py,
                     float rx, float ry, float rw, float rh);

    int  windowW = 1200, windowH = 800;

    // Slider drag state
    int  draggingSlider = -1;


    // --- Feature 19: Preprocessed Image Preview ---
    GLuint previewTexture = 0;
    int    previewImgW = 0;
    int    previewImgH = 0;

};
