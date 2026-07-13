# install-update-antigravity-windows
Does Exactly What It Says: Install/Update Antigravity

This tool has been migrated to a native C++ Windows console application for improved performance, portability, and native execution.

## Requirements
* Windows (to run the executable)
* Winget (installed on Windows)
* (Optional) MinGW-w64 (to compile from source)

## Compilation Instructions
To build the executable yourself (for example on a Linux machine with MinGW-w64 installed):

```bash
x86_64-w64-mingw32-g++ -O3 -Wall -Wextra -static -static-libgcc -static-libstdc++ -o install_antigravity.exe install_antigravity.cpp
```

This compiles a fully statically-linked native 64-bit Windows executable (`install_antigravity.exe`).

## Features
* **Process Management:** Enumerates, checks session username ownership, and safely terminates the target processes (`antigravity`, `google-antigravity`, `Antigravity`, `winget`).
* **Silent Winget Installation:** Automatically executes `winget` with identical arguments to install Google Antigravity seamlessly in user mode.
