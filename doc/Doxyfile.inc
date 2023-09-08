# This file provides overrides to the Doxyfile configuration
PROJECT_NAME = FWK_CAMERA
PROJECT_BRIEF = "Camera interface for Xcore AI"
PREDEFINED = __DOXYGEN__=1 
PREDEFINED += DWORD_ALIGNED=
PREDEFINED += __attribute__((weak))=
PREDEFINED += C_API= MA_C_API= C_API_START= C_API_END= EXTERN_C=
INPUT += camera/
ALIASES += "beginrst=^^\verbatim embed:rst^^"
ALIASES += "endrst=\endverbatim"
