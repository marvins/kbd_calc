---
description: CMake formatting guidelines
---

# CMakeLists.txt Formatting Guidelines

When editing CMakeLists.txt files, follow these rules:

## Source File Lists

- Source files in `add_executable()` and `add_library()` calls should be **alphabetized**
- Group files by directory structure first, then alphabetize within each group
- This can be skipped if there is a technical reason, but add a comment explaining why.
- Example:
  ```cmake
  add_executable(my_app
      src/core/first.cpp
      src/core/second.cpp
      src/io/file.cpp
      src/io/network.cpp
      src/utils/helper.cpp
  )
  ```

