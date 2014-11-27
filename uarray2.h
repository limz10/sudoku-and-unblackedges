/*
 * Header for Uarray2
 * Editor: Mengtian Li, Mingzhe Li
 * Date: 9/14/2014
 */

#ifndef UARRAY2_H
#define UARRAY2_H
#define T UArray2_T

typedef struct T *T;

extern T UArray2_new(int width, int height, int size);
extern void UArray2_free(T *array);
extern int UArray2_width(T array);
extern int UArray2_height(T array);
extern int UArray2_size(T array);

extern void *UArray2_at(T array, int row, int col);
extern void UArray2_map_col_major(T array, 
                           void apply(int i, int j, T a,
                                      void *data, void *p2),
                           void *cl);

extern void UArray2_map_row_major(T array, 
                           void apply(int i, int j, T a, 
                                      void *data, void *p2),
                           void *cl);  

#undef T
#endif
