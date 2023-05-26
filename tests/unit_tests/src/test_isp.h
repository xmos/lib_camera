#include "unity.h"
#include "isp.h"

#define INV_DELTA 30  // error allowed in YUV RGB color conversion 
#define CT_INT    127 // int conversion

// Store the RGB color and corresponding values
typedef struct
{
    int R, G, B;
    int Y, U, V;
} color_table_t;

void printColorTable(color_table_t* table) {
    printf("Color Table:\n");
    printf("R: %d, G: %d, B: %d\n", table->R, table->G, table->B);
    printf("Y: %d, U: %d, V: %d\n", table->Y, table->U, table->V);
}

void yuv_to_rgb_ct(color_table_t* ct_ref, color_table_t* ct_res){
    *ct_res = *ct_ref;
    uint32_t result = yuv_to_rgb(ct_ref->Y - CT_INT, ct_ref->U - CT_INT, ct_ref->V - CT_INT);
    ct_res -> R =  (uint8_t)(GET_R(result) + CT_INT);
    ct_res -> G =  (uint8_t)(GET_G(result) + CT_INT);
    ct_res -> B =  (uint8_t)(GET_B(result) + CT_INT);
}

void rgb_to_yuv_ct(color_table_t* ct_ref, color_table_t* ct_res){
    *ct_res = *ct_ref;
    uint32_t result = rgb_to_yuv(ct_ref->Y - CT_INT, ct_ref->U - CT_INT, ct_ref->V - CT_INT);
    ct_res -> Y =  (uint8_t)(GET_Y(result) + CT_INT);
    ct_res -> U =  (uint8_t)(GET_U(result) + CT_INT);
    ct_res -> V =  (uint8_t)(GET_V(result) + CT_INT);
}