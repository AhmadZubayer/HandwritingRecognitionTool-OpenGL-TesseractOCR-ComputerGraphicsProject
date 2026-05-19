#pragma once
#define _CRT_SECURE_NO_WARNINGS

#pragma warning(push, 0)
#include <opencv2/opencv.hpp>
#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>
#pragma warning(pop)

#include <string>
#include <vector>
#include <functional>

// ─── OCR Result ──────────────────────────────────────────────────────────────
struct WordResult {
    std::string text;
    float       confidence;  // 0.0 - 100.0
    bool        lowConfidence() const { return confidence < 60.0f; }
};

struct OCRResult {
    std::string            fullText;
    std::vector<WordResult> words;
    int                    charCount  = 0;
    int                    wordCount  = 0;
    double                 processingTimeMs = 0.0;
    bool                   success    = false;
};

// ─── OCR Settings ────────────────────────────────────────────────────────────
struct OCRSettings {
    float contrastAlpha   = 1.0f;   // Feature 17: contrast (1.0 = no change)
    int   thresholdValue  = 0;      // Feature 18: 0 = Otsu auto
    bool  invertColors    = false;  // Feature 7
    bool  correctSkew     = true;   // Feature 8
};

// ─── OCR Engine Class ────────────────────────────────────────────────────────
class OCREngine {
public:
    OCREngine();
    ~OCREngine();
    bool init(const std::string& tessDataPath = "C:\\tessdata");
    bool isReady() const { return m_ready; }

    cv::Mat   preprocess(const cv::Mat& input, const OCRSettings& settings = {});
    OCRResult recognize(const cv::Mat& preprocessedImg);
    OCRResult recognizeFile(const std::string& imagePath, const OCRSettings& settings = {});
    OCRResult recognizeMat(const cv::Mat& image, const OCRSettings& s = {});

    OCRSettings settings;
    std::function<void(const std::string&)> onStatus;
    cv::Mat lastPreprocessed;
    cv::Mat lastOriginal;

private:
    tesseract::TessBaseAPI* m_tess = nullptr;
    bool                    m_ready = false;

    // Individual preprocessing steps
    cv::Mat toGrayscale     (const cv::Mat& img);                    // Feature 2
    cv::Mat applyThreshold  (const cv::Mat& gray, int threshVal);    // Feature 3
    cv::Mat applyGaussianBlur(const cv::Mat& img);                   // Feature 4
    cv::Mat applyDilation   (const cv::Mat& img);                    // Feature 5
    cv::Mat applyErosion    (const cv::Mat& img);                    // Feature 6
    cv::Mat invertImage     (const cv::Mat& img);                    // Feature 7
    cv::Mat correctSkew     (const cv::Mat& img);                    // Feature 8
    cv::Mat resizeToDPI     (const cv::Mat& img, int targetDPI=300); // Feature 9
    cv::Mat applyContrast   (const cv::Mat& img, float alpha);       // Feature 17

    void status(const std::string& msg);
};
