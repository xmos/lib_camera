// -------------------------- COLOR CONVERSION -------------------------------------
// Macro arguments to get color components from packed result in the assembly program
#define GET_R(rgb) (rgb & 0xFF)
#define GET_G(rgb) ((rgb >> 8) & 0xFF)
#define GET_B(rgb) ((rgb >> 16)& 0xFF)

#define GET_Y(yuv) GET_R(yuv)
#define GET_U(yuv) GET_G(yuv)
#define GET_V(yuv) GET_B(yuv)

/**
 * @brief converts a YUV pixel to RGB
 * 
 * @param y Y component
 * @param u U component
 * @param v V component
 * @return int result of rgb conversion (need macros to decode output)
 */
int yuv_to_rgb(
    int y, 
    int u, 
    int v);

/**
 * @brief converts a RGB pixel to YUV
 * 
 * @param r red component
 * @param g green component
 * @param b blue component
 * @return int result of yuv conversion (need macros to decode output)
 */
int rgb_to_yuv(
    int r, 
    int g, 
    int b);
