# This file provides overrides to the Doxyfile configuration
PROJECT_NAME = lib_camera
PROJECT_BRIEF = "XCORE Camera Library"

# Camera module
INPUT = ../lib_camera

RECURSIVE              = YES
PREDEFINED             = C_API= EXTERN_C= DWORD_ALIGNED=
USE_MATHJAX            = YES
MATHJAX_FORMAT         = HTML-CSS
MATHJAX_RELPATH        = https://cdnjs.cloudflare.com/ajax/libs/mathjax/2.7.5/
