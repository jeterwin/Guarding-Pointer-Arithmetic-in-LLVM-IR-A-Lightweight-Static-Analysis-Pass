# LLVM Suspicious Pointer Pass

This repository contains a custom LLVM pass designed to detect and flag "suspicious" pointer arithmetic operations.

---

## Getting Started

### 1. Clone or Download
To get the source files, you can either clone this repository or download it as a ZIP file.

* **Clone via Git:**
    ```bash
    git clone https://github.com/jeterwin/Guarding-Pointer-Arithmetic-in-LLVM-IR-A-Lightweight-Static-Analysis-Pass.git
    ```
* **Download ZIP:**
    Click the **Code** button at the top of the repository and select **Download ZIP**. Extract the files to your local machine.

---

## Installation & Integration

To use this pass, you must integrate it into your local LLVM source tree. Follow the steps below precisely.

### 1. File Placement
Move the pass files into your `llvm-project` directory as specified:

| File | Destination Path |
| :--- | :--- |
| **`SuspiciousPtrPass.h`** | `llvm-project\llvm\include\llvm\Transforms\Utils\` |
| **`SuspiciousPtrPass.cpp`** | `llvm-project\llvm\lib\Transforms\Utils\` |

### 2. Registration in LLVM
You must register the pass within the LLVM Pass Manager to make it accessible via `opt`.

#### **A. Update the Pass Builder**
Add the header include to `llvm-project\llvm\lib\Passes\PassBuilder.cpp`:
```cpp
#include "llvm/Transforms/Utils/SuspiciousPtrPass.h"
```


#### **B. Update the Pass Registry**
Add the following line to the FUNCTION_PASS section in `llvm-project\llvm\lib\Passes\PassRegistry.def`:
```cpp
FUNCTION_PASS("suspicious-ptr", SuspiciousPtrPass())
```

#### **C. Update CMakeLists**
Add the .cpp file to `llvm-project\llvm\lib\Transforms\Utils\CMakeLists.txt`:
```cpp
add_llvm_component_library(LLVMTransformUtils
  ...
  SuspiciousPtrPass.cpp
  ...
)
```

#### **D. Building the opt tool**
Now that all of the files are in-place, after compiling `opt`, the pass can be integrated using the `opt -passes="function(suspicious-ptr)" file.ll` command.
