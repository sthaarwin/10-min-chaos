# 10 Min Chaos

Welcome to the official repository of **10 Min Chaos**, an intense and fast-paced game where every second counts! Built with C++ and the powerful Raylib library, get ready for a chaotic adventure that's all about strategy, speed, and survival.

## Getting Started

Before you can build and play **10 Min Chaos**, you'll need to set up your environment by installing the Raylib library.

### Prerequisites

Make sure you have a C++ compiler installed on your system. We recommend GCC, Clang, or Visual Studio.

### Installing Raylib

Raylib is a simple and easy-to-use library to enjoy videogames programming. Follow these steps to install Raylib on your system:

#### On Windows

1. Download the Raylib installer from the official repository.
2. Run the installer and follow the on-screen instructions.

#### On Linux

1. Open a terminal window.
2. Install Raylib  with the following command:
   ```bash
   git clone https://github.com/raysan5/raylib.git raylib
   cd raylib/src/
   make PLATFORM=PLATFORM_DESKTOP # To make the static version.
   qmake PLATFORM=PLATFORM_DESKTOP RAYLIB_LIBTYPE=SHARED # To make the dynamic shared version.
   make PLATFORM=PLATFORM_WEB # To make web version.

#### On macOS

1. Install Homebrew if you haven't already, with the following command:
   ```bash
   /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
   ```
2. Install Raylib using Homebrew:
   ```bash
   brew install raylib
   ```

## Building the Game

After installing Raylib, you can build the game using the provided Makefile:

```bash
make all
```

This will compile all the necessary files and produce an executable for **10 Min Chaos**.

## Playing the Game

To play the game, simply run the executable created by the build process.

```bash
./game
```

Enjoy the game and try to survive the chaos!

## Contributing

We welcome contributions to **10 Min Chaos**. If you have an idea or suggestion, please open an issue or submit a pull request.

## License

**10 Min Chaos** is released under the MIT License. See the `LICENSE` file for more details.

## Acknowledgments

- Thanks to the Raylib team for providing such an amazing library.
- Shoutout to all the game testers and contributors who helped bring **10 Min Chaos** to life.

Happy gaming!

