/*
 * Header for Bit2
 * Editor: Mengtian Li, Mingzhe Li
 * Date: 9/16/2014
 */

#ifndef BIT2_H
#define BIT2_H
#define T Bit2_T

typedef struct T *T;

extern T Bit2_new(int width, int height);
extern void Bit2_free(T *vector);
extern int Bit2_width(T vector);
extern int Bit2_height(T vector);
extern int Bit2_get(T vector, int row, int col);
extern int Bit2_put(T vector, int row, int col, int n);

extern void Bit2_map_col_major(T vector, 
                           void apply(int i, int j, Bit2_T a, 
                                      int bit, void *p1),
                           void *cl);

extern void Bit2_map_row_major(T vector, 
                           void apply(int i, int j, Bit2_T a, 
                                      int bit, void *p1),
                           void *cl);   

#undef T
#endif
