Handwriting Recognition Tool Workflow

1. Application Initialization
File: src/main.cpp
Process: Window created and GLUT environment initialized. Start page invoked as the first module.

2. Welcome Page Execution
Files: src/starting-page/start.cpp, src/starting-page/animation.cpp, src/starting-page/characters.cpp
Process: Welcome screen displayed with animated background and custom-drawn title. Transitions managed via button click or keyboard shortcuts.
Shortcuts:
- NEXT Button: Clicked to exit start page and launch drawing tool.
- Ctrl + Right Arrow: Transitioned to the drawing tool.
- 'w': Animation speed set to 0.02.
- 'a': Animation stopped.
- Up Arrow: Animation speed increased.
- Down Arrow: Animation speed decreased.
- '+': Zoomed in on the welcome page.
- '-': Zoomed out on the welcome page.

3. Drawing Canvas Operation
Files: src/drawing-canvas/drawing_main.cpp, src/drawing-canvas/canvas.cpp, src/drawing-canvas/ui_panel.cpp
Process: Interactive canvas provided for handwriting input. Drawing strokes captured and UI elements rendered.
Shortcuts:
- Left Mouse Button: Strokes drawn on the canvas.
- Middle Mouse Button: Canvas panned.
- Mouse Scroll: Canvas zoomed in/out.
- Ctrl + R: Recognition process triggered.
- Ctrl + Z: Last stroke undone.
- Ctrl + S: Canvas exported as PNG.
- Ctrl + T: OCR re-run with current settings.
- '1' or 'b': Pen color changed to Black.
- '2' or 'u': Pen color changed to Blue.
- '3' or 'r': Pen color changed to Red.
- 'e': Eraser mode toggled.
- 'c': Canvas cleared completely.
- 'g': Background grid toggled.
- 'z': Bezier smoothing toggled (ON/OFF).
- 'f': Result panel font size cycled.
- '+' / '-': Pen thickness increased/decreased.
- '[' / ']': Entire drawing rotated.
- Shift + '+': Entire drawing scaled up.
- Shift + '-': Entire drawing scaled down.
- Arrow Keys: Entire drawing translated (moved).
- ESC: Application closed.

4. Image Processing and Character Recognition
File: src/drawing-canvas/ocr_engine.cpp
Process: Framebuffer captured and cropped to the handwriting bounding box. Tesseract engine utilized to recognize text from the processed image.

5. Data Export and Final Output
Folders: src/canvas-exports, src/ocr-exports
Process: 
- Processed drawing exported as a PNG file to src/canvas-exports/.
- Recognized text saved as a TXT file to src/ocr-exports/.
- Results displayed in the application's result panel and printed to the terminal.

6. Animation and Transformation Implementation
Modules: Welcome Page, Drawing Canvas
Process:
- Mathematical Waves: Sine and Cosine functions applied to a grid of points in src/starting-page/animation.cpp to create a dynamic wave effect on the start screen.
- Character Construction: Linear transformations (translations and rotations) used in src/starting-page/characters.cpp to manually construct alphanumeric characters from basic geometric primitives.
- Viewport Transformations: Zooming and Panning implemented via glScalef and glTranslatef in src/drawing-canvas/canvas.cpp to allow flexible navigation of the drawing area.
- Object-Space Transformations: Rotation, Scaling, and Translation applied directly to stroke point coordinates in src/drawing-canvas/canvas.cpp, enabling modification of the drawn content.
- Curve Smoothing: Cubic Bezier interpolation performed on raw input points to generate smooth, aesthetically pleasing strokes.
