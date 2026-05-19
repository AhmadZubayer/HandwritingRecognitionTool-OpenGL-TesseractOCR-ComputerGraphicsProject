#define _CRT_SECURE_NO_WARNINGS
#pragma warning(push, 0)
#include <opencv2/opencv.hpp>
#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>
#pragma warning(pop)

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <thread>
#include <atomic>
#include <mutex>
#include <filesystem>

#include "canvas.cpp"
#include "ocr_engine.cpp"
#include "ui_panel.cpp"

namespace fs = std::filesystem;

// ─── Globals ──────────────────────────────────────────────────────────────────
Canvas    canvas;
OCREngine ocr;
UIPanel   ui;

std::mutex        ocrMutex;
OCRResult         lastResult;
std::atomic<bool> ocrRunning(false);
bool              freshResult = false;

GLFWwindow* gWindow = nullptr;

// ─── Forward declarations ────────────────────────────────────────────────────
void buildUI(UIPanel&, GLFWwindow*,
    std::function<void()>, std::function<void()>,
    std::function<void()>, std::function<void()>,
    std::function<void()>, std::function<void()>,
    std::function<void(float)>, std::function<void(int)>);

#include <iomanip>
#include <sstream>
#include <ctime>

// Helper to get timestamp
std::string getTimestamp() {
    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y%m%d_%H%M%S");
    return oss.str();
}

// Helper: get the directory where the executable lives
std::string getExeDir() {
    char path[MAX_PATH];
    GetModuleFileNameA(NULL, path, MAX_PATH);
    std::string exePath(path);
    size_t pos = exePath.find_last_of("\\/");
    return (pos != std::string::npos) ? exePath.substr(0, pos) : ".";
}

// Helper to ensure directory and return absolute path relative to exe location
std::string getValidExportPath(const std::string& folder, const std::string& filename) {
    std::string baseDir = getExeDir(); // e.g. D:/.../src
    std::string exportDir = baseDir + "\\" + folder;
    
    try {
        if (!fs::exists(exportDir)) {
            fs::create_directories(exportDir);
        }
    } catch (const std::exception& e) {
        std::cerr << "[Export] Failed to create directory: " << exportDir << " - " << e.what() << "\n";
        return filename; // fallback
    }
    
    return exportDir + "\\" + filename;
}

// ─── OCR trigger ─────────────────────────────────────────────────────────────
void triggerRecognize() {
    if (ocrRunning) return;
    if (!ocr.isReady()) { std::cout << "[OCR] Engine not ready.\n"; return; }
    if (canvas.strokes.empty()) { std::cout << "[OCR] Canvas is empty.\n";  return; }

    ui.setProcessing(true);
    canvas.showBoundingBox = true;

    // --- NEW: AUTO EXPORT TO CANVAS-EXPORTS ---
    std::string timestamp = getTimestamp();
    std::string imageFilename = getValidExportPath("canvas-exports", "canvas_" + timestamp + ".png");
    if (canvas.exportToPNG(imageFilename)) {
        std::cout << "[Export] Saved image: " << imageFilename << "\n";
    }

    // Capture framebuffer → OpenCV Mat
    int w = canvas.windowWidth, h = canvas.windowHeight;
    std::vector<unsigned char> pixels((size_t)w * h * 3);
    glReadPixels(0, 0, w, h, GL_RGB, GL_UNSIGNED_BYTE, pixels.data());
    cv::Mat img(h, w, CV_8UC3);
    memcpy(img.data, pixels.data(), (size_t)w * h * 3);
    cv::cvtColor(img, img, cv::COLOR_RGB2BGR);
    cv::flip(img, img, 0); // Now top-left origin

    // --- CROP TO HANDWRITING BOUNDING BOX ---
    float minX, minY, maxX, maxY;
    canvas.getStrokeBounds(minX, minY, maxX, maxY);

    int sx1 = std::max(0, (int)(minX * canvas.zoom + canvas.panX) - 15);
    int sy1 = std::max(0, (int)(minY * canvas.zoom + canvas.panY) - 15);
    int sx2 = std::min(w, (int)(maxX * canvas.zoom + canvas.panX) + 15);
    int sy2 = std::min(h, (int)(maxY * canvas.zoom + canvas.panY) + 15);

    int cropX = std::max(0, sx1);
    int cropY = std::max(0, h - sy2);
    int cropW = std::min(sx2 - sx1, w - cropX);
    int cropH = std::min(sy2 - sy1, h - cropY);

    if (cropW > 0 && cropH > 0) {
        img = img(cv::Rect(cropX, cropY, cropW, cropH)).clone();
    }

    std::thread([img, timestamp]() mutable {
        ocrRunning = true;
        OCRResult result = ocr.recognizeMat(img, ocr.settings);
        
        // --- NEW: AUTO SAVE TO OCR-EXPORTS AND PRINT TO TERMINAL ---
        std::string txtFilename = getValidExportPath("ocr-exports", "ocr_" + timestamp + ".txt");
        std::ofstream outFile(txtFilename);
        if (outFile.is_open()) {
            outFile << "Timestamp: " << timestamp << "\n";
            int conf = (result.success && !result.words.empty()) ? (int)result.words[0].confidence : 0;
            outFile << "Confidence: " << conf << "%\n";
            outFile << "Result:\n" << result.fullText << "\n";
            outFile.close();
            std::cout << "[OCR] Result saved to: " << txtFilename << "\n";
        }
        
        std::cout << "\n--- OCR Result (" << timestamp << ") ---\n";
        std::cout << result.fullText << "\n";
        std::cout << "--------------------------------\n" << std::endl;

        {
            std::lock_guard<std::mutex> lock(ocrMutex);
            lastResult = result;
            freshResult = true;
        }
        ocrRunning = false;
        }).detach();
}

// ─── GLFW Callbacks ───────────────────────────────────────────────────────────
void mouseButtonCallback(GLFWwindow* w, int button, int action, int mods) {
    double mx, my;
    glfwGetCursorPos(w, &mx, &my);

    // Forward to UI first (toolbar + panel area)
    ui.onMouseButton(w, button, action, mods, mx, my);

    // Only forward to canvas if click is LEFT of the result panel
    if (mx < ui.panelX)
        canvas.onMouseButton(w, button, action, mods);
}

void cursorPosCallback(GLFWwindow* w, double x, double y) {
    ui.onMouseMove(w, x, y);
    if (x < ui.panelX)
        canvas.onMouseMove(w, x, y);
}

void scrollCallback(GLFWwindow* w, double xo, double yo) {
    double mx, my;
    glfwGetCursorPos(w, &mx, &my);
    ui.onScroll(xo, yo, mx, my);
    if (mx < ui.panelX)
        canvas.onScroll(w, xo, yo);
}

void keyCallback(GLFWwindow* w, int key, int sc, int action, int mods) {
    // Ctrl+R = Recognize
    if (key == GLFW_KEY_R && (mods & GLFW_MOD_CONTROL) && action == GLFW_PRESS) {
        triggerRecognize(); return;
    }
    // Ctrl+S = Export PNG
    if (key == GLFW_KEY_S && (mods & GLFW_MOD_CONTROL) && action == GLFW_PRESS) {
        canvas.showBoundingBox = true;
        if (canvas.exportToPNG("canvas_export.png"))
            std::cout << "[Export] Saved: canvas_export.png\n";
        return;
    }
    // Ctrl+Z = Undo
    if (key == GLFW_KEY_Z && (mods & GLFW_MOD_CONTROL) && action == GLFW_PRESS) {
        canvas.undo(); return;
    }
    // Feature 19: Ctrl+T = re-run OCR with current settings
    if (key == GLFW_KEY_T && (mods & GLFW_MOD_CONTROL) && action == GLFW_PRESS) {
        triggerRecognize(); return;
    }

    ui.onKey(key, mods, action);
    canvas.onKey(w, key, sc, action, mods);

    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(w, true);
}

void framebufferSizeCallback(GLFWwindow* w, int width, int height) {
    canvas.windowWidth = width;
    canvas.windowHeight = height;
    glViewport(0, 0, width, height);
}

// ─── Main ─────────────────────────────────────────────────────────────────────
int runDrawingTool() {
    std::cout << "[DrawingTool] Entering runDrawingTool()..." << std::endl;
    if (!glfwInit()) { std::cerr << "GLFW init failed\n"; return -1; }

    glfwWindowHint(GLFW_SAMPLES, 4);
    gWindow = glfwCreateWindow(1200, 800,
        "Handwriting Recognition Tool  |  Ctrl+R=Recognize  Ctrl+Z=Undo  Ctrl+S=Export",
        nullptr, nullptr);
    if (!gWindow) { glfwTerminate(); return -1; }

    glfwMakeContextCurrent(gWindow);
    glfwSwapInterval(1);

    if (glewInit() != GLEW_OK) { std::cerr << "GLEW init failed\n"; return -1; }

    // Callbacks
    glfwSetMouseButtonCallback(gWindow, mouseButtonCallback);
    glfwSetCursorPosCallback(gWindow, cursorPosCallback);
    glfwSetScrollCallback(gWindow, scrollCallback);
    glfwSetKeyCallback(gWindow, keyCallback);
    glfwSetFramebufferSizeCallback(gWindow, framebufferSizeCallback);

    // Init subsystems
    canvas.init(gWindow);
    ocr.init("C:\\Program Files\\Tesseract-OCR\\tessdata");

    int winW, winH;
    glfwGetFramebufferSize(gWindow, &winW, &winH);
    ui.init(winW, winH);

    // Wire UI callbacks
    buildUI(ui, gWindow,
        /* Recognize  */ [] { triggerRecognize(); },
        /* Clear      */ [] { canvas.clearCanvas(); },
        /* Undo       */ [] { canvas.undo(); },
        /* Export PNG */ [] {
            canvas.showBoundingBox = true;
            if (canvas.exportToPNG("canvas_export.png"))
                std::cout << "[Export] Saved: canvas_export.png\n";
        },
        /* Save Text  */ [] {
            if (ui.saveTextToFile("ocr_result.txt"))
                std::cout << "[UI] Saved: ocr_result.txt\n";
        },
        /* Copy       */ [] { ui.copyToClipboard(gWindow); },
        /* Contrast   */ [](float v) {
            ocr.settings.contrastAlpha = v;
            std::cout << "[OCR] Contrast set to " << v << "\n";
        },
        /* Threshold  */ [](int v) {
            ocr.settings.thresholdValue = v;
            std::cout << "[OCR] Threshold set to " << (v == 0 ? "Auto" : std::to_string(v)) << "\n";
        }
    );

    std::cout << "=== Handwriting Recognition Tool ===\n";
    std::cout << "Ctrl+R = Recognize   Ctrl+Z = Undo\n";
    std::cout << "Ctrl+S = Export PNG  Ctrl+T = Re-run OCR\n";
    std::cout << "B/1=Black  U/2=Blue  3=Red  E=Eraser\n";
    std::cout << "+/- = Thickness   G=Grid   C=Clear\n";
    std::cout << "F = Cycle font size in result panel\n";
    std::cout << "=====================================\n";

    // ── Main loop ─────────────────────────────────────────────────────────────
    while (!glfwWindowShouldClose(gWindow)) {
        glfwPollEvents();

        // Pick up OCR result from background thread
        {
            std::lock_guard<std::mutex> lock(ocrMutex);
            if (freshResult) {
                ui.setResult(lastResult);

                // --- NEW: Pass the preprocessed image to the UI ---
                ui.setPreviewImage(ocr.lastPreprocessed);

                // Feature 20: write test log automatically
                ui.writeTestLog("framebuffer_capture", lastResult);
                freshResult = false;
                std::cout << "[Main] OCR result displayed.\n";
            }
        }

        // Render canvas (left area)
        canvas.render();

        // Cursor indicator
        double mx, my;
        glfwGetCursorPos(gWindow, &mx, &my);
        if (mx < ui.panelX)
            canvas.drawCursor(mx, my);

        // UI panel (toolbar + result panel — drawn on top)
        glfwGetFramebufferSize(gWindow, &winW, &winH);
        ui.render(winW, winH);

        glfwSwapBuffers(gWindow);
    }

    glfwDestroyWindow(gWindow);
    glfwTerminate();
    return 0;
}
