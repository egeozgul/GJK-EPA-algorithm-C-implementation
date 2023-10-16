# Collision Detection and Response for 2D Objects

Welcome to the simple yet impressive demo showcasing collision detection and response for 2D objects using the Separating Axis Theorem (SAT) algorithm in a Windows environment. This project not only performs collision checks between two objects but also provides visual representation and diagnostics to better understand the underlying mechanisms.

## 🌟 Features:

1. **Separating Axis Theorem (SAT)**: Reliable method to check if two convex polygons are intersecting.
2. **Dynamic Frame Buffer Management**: Reallocates the buffer memory as the window size changes, ensuring the application remains adaptive and responsive.
3. **Intuitive Input Handling**: Use your mouse and keyboard to interact with and move the objects.
4. **FPS & Duration Diagnostics**: Real-time FPS and frame duration display for performance checks.
5. **Font Loading**: Load custom fonts from directories for on-screen text.
6. **Concave & Convex Hull Generation**: Convert object shapes to their concave and convex representations.

## ⚙️ Prerequisites:

- Ensure you have a Windows development environment.
- Properly set up a C++ compiler and linker.

## 🔍 Code Overview:

- **Memory Stack and Heap**:
  - Customize the limit of the stack and heap memory using `#pragma comment`.

- **Libraries**:
  - Windows: Basic functionality for window creation and handling.
  - Standard Libraries: Data handling and mathematical calculations.

- **Utility Headers**:
  - Window: Functions and structures related to window generation and handling.
  - Input: Functions for input processing.
  - Graphics: Painting and rendering graphics on the screen.
  - Physics: Collision detection and response algorithms.

- **Main Application Loop**:
  - Window generation.
  - Memory allocation for the display buffer.
  - Object image loading, hull generation, and collision checks.
  - Input handling and object movement.
  - Painting and rendering.

- **Message Handling**:
  - Processes various window messages such as close, resize, and input events.

## 🚀 Getting Started:

1. **Setup**:
   - Clone this repository.
   - Make sure you have the required headers `commonStuff.h`, `Window.h`, `Input.h`, `Graphics.h`, and `Physics.h`.

2. **Compilation**:
   - Compile the code using your preferred C++ compiler.
   - Link against `winmm.lib` if you plan to add audio functionalities in the future.

3. **Run**:
   - Execute the program.
   - Interact with the objects using your mouse (to position one object) and the keys 'W', 'A', 'S', 'D' to move the second object.

4. **Observations**:
   - Notice the real-time collision detection and response between the objects.
   - The FPS and frame duration details will be displayed on the screen.

## 📌 Future Improvements:

- **Audio Integration**: With the provided placeholder for the `winmm.lib`, you can easily integrate sound effects or background music to enhance the experience.
- **More Complex Shapes**: Extend the functionality to support more complex shapes and multiple objects.
- **Enhanced Physics**: Implement momentum, restitution, and friction for more realistic interactions.

## 🤝 Contributions:

Feel free to fork this repository and submit your pull requests for enhancements and bug fixes. Your contributions are highly appreciated!

## 📜 License:

This project is open-source under the MIT license.

---

👏 Thank you for checking out this demo. For any issues or suggestions, please [open an issue](https://github.com/your-github-username/repo-name/issues/new) on GitHub.

