# Quick reminders for best setup for file structures
## NOTE: As this is a small project space, most stuff can be kept outside but more complex projects would want specific directories to organise workflow and files
1. CMakeLists.txt in the root or source directory is the main one and every other CMakeLists.txt should be only be available for their own subdirectory


REFERENCEs: 
Basic setup for CMAKE: https://www.dominikgrabiec.com/posts/2022/11/11/setting_up_basic_project_cmake_visual_studio.html
Standard set-up for SDL with locally vendored lib: https://github.com/libsdl-org/SDL/blob/main/docs/INTRO-cmake.md
Using CMake to install SDL3 (and others) only when build is called (using CPM.cmake): https://www.youtube.com/watch?v=T32B7nf6B-8