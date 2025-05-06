# This file provides overrides to the Doxyfile configuration
PROJECT_NAME = lib_camera
PROJECT_BRIEF = "XCORE Camera Library"

# Camera module
INPUT = ../lib_camera/api ../lib_camera/src/sensors

FILE_PATTERNS = *.h *.hpp *.c *.cpp
RECURSIVE              = YES
PREDEFINED             = C_API= EXTERN_C= DWORD_ALIGNED= __cplusplus
USE_MATHJAX            = YES
MATHJAX_FORMAT         = HTML-CSS
MATHJAX_RELPATH        = https://cdnjs.cloudflare.com/ajax/libs/mathjax/2.7.5/
