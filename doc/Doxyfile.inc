# This file provides overrides to the Doxyfile configuration
PROJECT_NAME = lib_camera
PROJECT_BRIEF = "Camera interface for xcore AI"

# Camera module
INPUT = ../camera
INPUT += ../sensors

PREDEFINED             = C_API= EXTERN_C= DWORD_ALIGNED=

USE_MATHJAX            = YES
MATHJAX_FORMAT         = HTML-CSS
MATHJAX_RELPATH        = https://cdnjs.cloudflare.com/ajax/libs/mathjax/2.7.5/
