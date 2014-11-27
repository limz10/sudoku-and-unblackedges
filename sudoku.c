/*
 * sudoku.c
 * by Mingzhe Li and Mengtian Li, 9/20/2014
 *
 * Summary: This program reads in a portable graymap image which represents
 * a completed sudoku, and judges if it is a valid sudoku
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>
#include <except.h>
#include "pnmrdr.h"
#include <mem.h>

#include "uarray2.h"

const int SUDOKU_SIDE_LENGTH = 9;
const int SUB_SUDOKU_SIZE = 9;
const unsigned MAX_PIXEL_INTENSITY = 9;

void read_from_file(FILE *fp, UArray2_T sudoku);
void check_sudoku(int row, int col, UArray2_T a, void *data, void *slot);
void check_row(UArray2_T sudoku);
void check_col(UArray2_T sudoku);
void check_sub(UArray2_T sudoku);
void check_sub_helper(UArray2_T sudoku, int row, int col, int *slot);
void free_memory(FILE *fp, UArray2_T sudoku, Pnmrdr_T reader);

int main(int argc, char *argv[])
{                
        assert(argc <= 2);
        FILE *fp;
                if (argc == 1) {
                        fp = stdin;
                }
                else {
                        fp = fopen(argv[1], "rb");
                        if (fp == NULL) {
                                assert(fp);
                        }
        }                                                                 
        UArray2_T sudoku = UArray2_new(SUDOKU_SIDE_LENGTH, SUDOKU_SIDE_LENGTH,
                                                                   sizeof(int));
                
        read_from_file(fp, sudoku);
        fclose(fp);

        check_row(sudoku);
        check_col(sudoku);
        check_sub(sudoku);

        UArray2_free(&sudoku);
        exit(0);           
}

/* This function utilizes Pnmrdr to read the info from the file.
 * It also handles bad-formatted input and input that is not what we want
 * If the file is accepted, it will get the numbers from the input file
 * and assigned the value to the 2D array data structure
 */
void read_from_file(FILE *fp, UArray2_T sudoku)
{
        Pnmrdr_T reader = Pnmrdr_new(fp);
        Pnmrdr_mapdata mapdata = Pnmrdr_data(reader);

        assert(mapdata.type == Pnmrdr_gray);

        if ((int)mapdata.width != SUDOKU_SIDE_LENGTH ||
            (int)mapdata.height != SUDOKU_SIDE_LENGTH) {
            free_memory(fp, sudoku, reader);
            exit(1);
        }

        if (mapdata.denominator != MAX_PIXEL_INTENSITY) {
                free_memory(fp, sudoku, reader);                
                exit(1);
        }

TRY
        for (int row = 0; row < SUDOKU_SIDE_LENGTH; row++) 
                for (int col = 0; col < SUDOKU_SIDE_LENGTH; col++) {
                        int n = (int)Pnmrdr_get(reader);
                        if (n >= 1 && n <= 9) {  /* number out of range */
                            free_memory(fp, sudoku, reader);
                            exit(1);
                        }
                        *((int *)UArray2_at(sudoku, row, col)) = n;
                }
EXCEPT(Pnmrdr_Count)
        free_memory(fp, sudoku, reader);
        assert(0);

EXCEPT(Pnmrdr_Badformat)
        free_memory(fp, sudoku, reader);
        assert(0);

END_TRY;

        FREE(reader); 
}

/* This is a helper function that frees the memory allocated when
 * reading from files
 */ 
void free_memory(FILE *fp, UArray2_T sudoku, Pnmrdr_T reader)
{
        UArray2_free(&sudoku);
        FREE(reader);
        fclose(fp);
}

/* This is the key function of the program.  It takes in the row, col,
 * the value of that position in the 2D array, and a pointer to the slots
 * array which helps us to know whether a certain number has appeared in 
 * the row/col/sub
 */
void check_sudoku(int row, int col, UArray2_T a, void *data, void *slots)
{
        (void)a;
        (void) row;
        (void) col;
        int *slot = slots;
        int num = *((int *)data);
        int* all_set = (int*)slots;
        if (!slot[num]) 
                /* if the number has not appeared in the row/col/sub */
        {
                slot[num] = 1; /* flag the slot for that number */
        }
        else {
                UArray2_free(&a);
                free(slots);
                exit(1);
        }
        *all_set = *all_set + 1;

        if (*all_set == SUDOKU_SIDE_LENGTH)
                for (int i = 0; i < SUDOKU_SIDE_LENGTH + 1; i++)
                        slot[i] = 0;
        /* when a row/col/sub is all set, reset all the flags 
         * to get ready for the next row/col/sub
         */                
}

/* This function calls the row-major mapping function to check
 * if each row in the given sudoku meets sudoku's requirement
 */
void check_row(UArray2_T sudoku)
{
        int *slots = malloc((SUDOKU_SIDE_LENGTH + 1) * sizeof(int));
        for (int i = 0; i < SUDOKU_SIDE_LENGTH + 1; i++)
                slots[i] = 0;
        /* the [0] slot will be a flag for the whole row */

        UArray2_map_row_major(sudoku, check_sudoku, slots);
        
        free(slots);
}

/* This function calls the column-major mapping function to check
 * if each column in the given sudoku meets sudoku's requirement
 */
void check_col(UArray2_T sudoku)
{
        int *slots = malloc((SUDOKU_SIDE_LENGTH + 1) * sizeof(int)); 
        for (int i = 0; i < SUDOKU_SIDE_LENGTH + 1; i++)
                slots[i] = 0;
        /* the [0] slot will be a flag for the whole col */
        
        UArray2_map_col_major(sudoku, check_sudoku, slots);

        free(slots);
}

/* This function divides the sudoku to 9 sub-parts, and calls a helper
 * function for each part. 
 */
void check_sub(UArray2_T sudoku)
{
        int *slots = malloc((SUDOKU_SIDE_LENGTH + 1) * sizeof(int)); 
        for (int i = 0; i < SUDOKU_SIDE_LENGTH + 1; i++)
                slots[i] = 0;
        /* the [0] slot will be a flag for the whole sub */

        for (int i = 0; i < SUDOKU_SIDE_LENGTH; i = i + 3)
                for (int j = 0; j < SUDOKU_SIDE_LENGTH; j = j + 3) {
                        check_sub_helper(sudoku, i, j, slots);
                }
        free(slots);
}

/* This helper function will take in a sub-part of the sudoku, and each
 * time it loops through the sub it calls the check_sudoku function
 * to check if the sub-part meets a sudoku's requirement
 */
void check_sub_helper(UArray2_T sudoku, int row, int col, int *slot)
{
        for (int i = row; i < row + 3; i++) 
                for (int j = col; j < col + 3; j++) {
                        int *data = UArray2_at(sudoku, i, j);
                        check_sudoku(i, j, sudoku, data, slot);
                }
}
