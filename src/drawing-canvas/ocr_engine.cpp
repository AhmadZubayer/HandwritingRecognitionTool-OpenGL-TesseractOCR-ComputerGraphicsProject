#define _CRT_SECURE_NO_WARNINGS
#include "ocr_engine.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>
#include <cmath>

// ─── Constructor / Destructor ────────────────────────────────────────────────
OCREngine::OCREngine() {}

OCREngine::~OCREngine() {
    if (m_tess) {
        m_tess->End();
        delete m_tess;
        m_tess = nullptr;
    }
}

// ─── Feature 10: Init Tesseract ──────────────────────────────────────────────
bool OCREngine::init(const std::string& tessDataPath) {
    status("Initializing Tesseract OCR engine...");
    m_tess = new tesseract::TessBaseAPI();
    if (m_tess->Init(tessDataPath.c_str(), "eng")) {
        status("ERROR: Failed to initialize Tesseract. Check tessdata path.");
        m_ready = false;
        return false;
    }
    m_ready = true;
    status("Tesseract initialized successfully.");
    return true;
}

void OCREngine::status(const std::string& msg) {
    std::cout << "[OCR] " << msg << "\n";
    if (onStatus) onStatus(msg);
}

// ─── Feature 17: Contrast enhancement ───────────────────────────────────────
cv::Mat OCREngine::applyContrast(const cv::Mat& img, float alpha) {
    if (std::abs(alpha - 1.0f) < 0.01f) return img.clone();
    cv::Mat result;
    img.convertTo(result, -1, alpha, 0);
    return result;
}

// ─── Feature 2: Grayscale ────────────────────────────────────────────────────
cv::Mat OCREngine::toGrayscale(const cv::Mat& img) {
    if (img.channels() == 1) return img.clone();
    cv::Mat gray;
    cv::cvtColor(img, gray, cv::COLOR_BGR2GRAY);
    return gray;
}

// ─── Feature 3: Threshold (Otsu) ─────────────────────────────────────────────
cv::Mat OCREngine::applyThreshold(const cv::Mat& gray, int threshVal) {
    cv::Mat binary;
    if (threshVal == 0) {
        // Feature 3: Otsu's automatic thresholding
        cv::threshold(gray, binary, 0, 255,
                      cv::THRESH_BINARY | cv::THRESH_OTSU);
    } else {
        // Feature 18: Manual threshold from slider
        cv::threshold(gray, binary, threshVal, 255, cv::THRESH_BINARY);
    }
    return binary;
}

// ─── Feature 4: Gaussian blur ────────────────────────────────────────────────
cv::Mat OCREngine::applyGaussianBlur(const cv::Mat& img) {
    cv::Mat blurred;
    cv::GaussianBlur(img, blurred, cv::Size(3, 3), 0);
    return blurred;
}

// ─── Feature 5: Dilation ─────────────────────────────────────────────────────
cv::Mat OCREngine::applyDilation(const cv::Mat& img) {
    cv::Mat dilated;
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(2, 2));
    cv::dilate(img, dilated, kernel, cv::Point(-1,-1), 1);
    return dilated;
}

// ─── Feature 6: Erosion ──────────────────────────────────────────────────────
cv::Mat OCREngine::applyErosion(const cv::Mat& img) {
    cv::Mat eroded;
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(2, 2));
    cv::erode(img, eroded, kernel, cv::Point(-1,-1), 1);
    return eroded;
}

// ─── Feature 7: Invert colors ────────────────────────────────────────────────
cv::Mat OCREngine::invertImage(const cv::Mat& img) {
    cv::Mat inverted;
    cv::bitwise_not(img, inverted);
    return inverted;
}

// ─── Feature 8: Skew correction ──────────────────────────────────────────────
cv::Mat OCREngine::correctSkew(const cv::Mat& img) {
    // Use Hough line transform to detect dominant angle
    cv::Mat edges;
    cv::Canny(img, edges, 50, 150, 3);

    std::vector<cv::Vec4i> lines;
    cv::HoughLinesP(edges, lines, 1, CV_PI / 180, 50, 50, 10);

    if (lines.empty()) return img.clone();

    double angleSum = 0;
    int    count    = 0;
    for (auto& line : lines) {
        double angle = atan2((double)(line[3] - line[1]),
                             (double)(line[2] - line[0])) * 180.0 / CV_PI;
        if (std::abs(angle) < 30.0) {  // ignore near-vertical lines
            angleSum += angle;
            count++;
        }
    }
    if (count == 0) return img.clone();

    double skewAngle = angleSum / count;
    if (std::abs(skewAngle) < 0.5) return img.clone();  // skip tiny corrections

    status("Correcting skew by " + std::to_string(skewAngle) + " degrees");

    cv::Point2f center((float)img.cols / 2.0f, (float)img.rows / 2.0f);
    cv::Mat     rot = cv::getRotationMatrix2D(center, skewAngle, 1.0);
    cv::Mat     corrected;
    cv::warpAffine(img, corrected, rot, img.size(),
                   cv::INTER_LINEAR, cv::BORDER_CONSTANT,
                   cv::Scalar(255, 255, 255));
    return corrected;
}

// ─── Feature 9: Resize to 300 DPI ────────────────────────────────────────────
cv::Mat OCREngine::resizeToDPI(const cv::Mat& img, int targetDPI) {
    // Assume source is ~96 DPI screen capture; scale up to 300 DPI
    double scale = (double)targetDPI / 96.0;
    if (scale <= 1.0) return img.clone();
    cv::Mat resized;
    cv::resize(img, resized, cv::Size(), scale, scale, cv::INTER_CUBIC);
    return resized;
}

// ─── Features 1-9: Full preprocessing pipeline ───────────────────────────────
cv::Mat OCREngine::preprocess(const cv::Mat& input, const OCRSettings& s) {
    lastOriginal = input.clone();

    status("Step 1: Loading image...");
    cv::Mat img = input.clone();

    status("Step 2: Converting to grayscale...");
    img = toGrayscale(img);

    if (std::abs(s.contrastAlpha - 1.0f) > 0.01f) {
        status("Step 2b: Applying contrast enhancement...");
        img = applyContrast(img, s.contrastAlpha);
    }

    status("Step 4: Applying Gaussian blur...");
    img = applyGaussianBlur(img);

    status("Step 3: Applying threshold (Otsu)...");
    img = applyThreshold(img, s.thresholdValue);

    // Feature 7: Auto-invert if background is dark
    double meanVal = cv::mean(img)[0];
    if (meanVal < 127.0 || s.invertColors) {
        status("Step 7: Inverting colors (dark background detected)...");
        img = invertImage(img);
    }

    status("Step 5: Applying dilation...");
    img = applyDilation(img);

    status("Step 6: Applying erosion...");
    img = applyErosion(img);

    if (s.correctSkew) {
        status("Step 8: Correcting skew...");
        img = correctSkew(img);
    }

    status("Step 9: Resizing to 300 DPI...");
    img = resizeToDPI(img, 300);

    lastPreprocessed = img.clone();
    status("Preprocessing complete.");
    return img;
}

// ─── Features 10-16: OCR Recognition ─────────────────────────────────────────
OCRResult OCREngine::recognize(const cv::Mat& preprocessedImg) {
    OCRResult result;
    if (!m_ready) {
        status("ERROR: Tesseract not initialized.");
        return result;
    }

    // Feature 18: Show processing indicator
    status("Processing... Running OCR...");

    auto startTime = std::chrono::high_resolution_clock::now();

    // Feature 11: Pass image to Tesseract
    m_tess->SetImage(preprocessedImg.data,
                     preprocessedImg.cols,
                     preprocessedImg.rows,
                     1,  // bytes per pixel (grayscale)
                     (int)preprocessedImg.step);

    // Feature 12: Get full OCR text
    char* rawText = m_tess->GetUTF8Text();
    if (rawText) {
        result.fullText = std::string(rawText);
        delete[] rawText;
    }

    // Feature 13: Parse multi-line output
    // (fullText already has \n separators from Tesseract)

    // Feature 14: Get per-word confidence scores
    tesseract::ResultIterator* ri = m_tess->GetIterator();
    tesseract::PageIteratorLevel level = tesseract::RIL_WORD;
    if (ri) {
        do {
            const char* word = ri->GetUTF8Text(level);
            float conf = ri->Confidence(level);
            if (word) {
                WordResult wr;
                wr.text       = std::string(word);
                wr.confidence = conf;
                result.words.push_back(wr);
                delete[] word;

            }
        } while (ri->Next(level));
        delete ri;
    }

    auto endTime = std::chrono::high_resolution_clock::now();
    result.processingTimeMs =
        std::chrono::duration<double, std::milli>(endTime - startTime).count();

    // Feature 7 + 16: Count chars and words
    result.wordCount = (int)result.words.size();
    result.charCount = (int)result.fullText.size();
    result.success   = !result.fullText.empty();

    // Feature 17: Log raw output to file
    {
        std::ofstream log("ocr_debug_log.txt", std::ios::app);
        log << "=== OCR Result ===\n";
        log << result.fullText << "\n";
        log << "Words: " << result.wordCount
            << "  Chars: " << result.charCount
            << "  Time: " << result.processingTimeMs << "ms\n\n";
    }

    status("OCR complete in " + std::to_string((int)result.processingTimeMs) + "ms"
           + " | Words: " + std::to_string(result.wordCount));

    m_tess->Clear();
    return result;
}

// ─── Feature 19: Re-run OCR from file ────────────────────────────────────────
OCRResult OCREngine::recognizeMat(const cv::Mat& image, const OCRSettings& s) {
    status("Received image directly from framebuffer...");
    if (image.empty()) {
        status("ERROR: Empty image passed to recognizeMat");
        return {};
    }
    cv::Mat processed = preprocess(image, s);
    return recognize(processed);

}

OCRResult OCREngine::recognizeFile(const std::string& imagePath, const OCRSettings& s) {
    status("Loading image: " + imagePath);
    cv::Mat img = cv::imread(imagePath, cv::IMREAD_COLOR);
    if (img.empty()) {
        status("ERROR: Could not load image: " + imagePath);
        return {};
    }
    cv::Mat processed = preprocess(img, s);
    return recognize(processed);
}