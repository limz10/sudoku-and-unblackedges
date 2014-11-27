/*
 * bit2
 * This is an implementation of Bit2_T
 * by Mingzhe Li and Mengtian Li, 9/18/14
 *
 */

#include <bit.h>
#include "bit2.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <mem.h>

#define T Bit2_T

/* We defined the struct with width and height based on a single Bit_T */
struct T {
        int width;
        int height;
        Bit_T bbb;
};

/* takes in the width, height of the 2D Bit, initializes the Bit_T
 * and returns it.
 */
T Bit2_new(int width, int height)
{
        T toRtn = malloc(sizeof(*toRtn));
        assert(toRtn != NULL);

        toRtn->width = width;
        toRtn->height = height;
        toRtn->bbb = Bit_new(width * height); 
        return toRtn;
}

/* Calls Bit_free() to free the memory */
void Bit2_free(T *vector)
{
        Bit_free(&(*vector)->bbb);
        FREE(*vector);
}

/* returns the width of the Bit2 */
int Bit2_width(T vector)
{
        return vector->width;
}

/* returns the height of the Bit2 */
int Bit2_height(T vector)
{
        return vector->height;
}

/* calls Bit_get() to return the element at the given row and col */
int Bit2_get(T vector, int row, int col)
{
        return Bit_get(vector->bbb, col * (vector->width) + row);
}

/* calls Bit_put() to change the element at the given row and col with
 * the bit n passed in, returns the previous bit
 */
int Bit2_put(T vector, int row, int col, int n)
{
        int toRtn = Bit2_get(vector, row, col);
        Bit_put(vector->bbb, col * (vector->width) + row, n);
        return toRtn;
}

/* Similar to Bit_map(), this function loops through the data and calls
 * apply function each time
 * 
 * The only difference between _row_major() and _col_major() is choosing the
 * priority between row or column.  This is the column-major version of the 
 * mapping function
 */
void Bit2_map_col_major(T vector, 
                        void apply(int i, int j, Bit2_T a, int bit, void *p1),
                        void *cl)
{
        assert(vector->bbb);
        for (int row = 0; row < vector->width; row++) {
                for (int col = 0; col < vector->height; col++) {
                        apply(row, col, vector, Bit2_get(vector, row, col), cl);
                }
        }
}

/* As specified above, this is the row-major version of the mapping function */
void Bit2_map_row_major(T vector, 
                        void apply(int i, int j, Bit2_T a, int bit, void *p1),
                        void *cl)
{
        assert(vector->bbb);
        for (int col = 0; col < vector->height; col++) {
                for (int row = 0; row < vector->width; row++) {
                        apply(row, col, vector, Bit2_get(vector, row, col), cl);
                }
        }
}


