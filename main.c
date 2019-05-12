#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "./main.h"

#define TRUE    1
#define FALSE   0

//Directions for distance transform
#define NONE    0
#define LEFT    1
#define UP      2
#define RIGHT   3
#define DOWN    4

#define SQUARE_PATH "./Images/square.raw"
#define CIRCUIT_PATH "./Images/cktboard.raw"

#define CIRCUIT_OUTPATH "./Images/cktboard_texture.raw"
#define SQUARE_OUTPATH "./Images/square_disttrans.raw"

void readFromFile(FILE * fp, unsigned char **arr, int dim_x, int dim_y)
{
    for (int i = 0; i < dim_y; i++)
    {
        fread(arr[i], sizeof(unsigned char), dim_x, fp);
    }
}

unsigned char ** alloc2DArray(int x_size, int y_size)
{
    unsigned char ** in = (unsigned char **)calloc(y_size, sizeof(unsigned char *));
    for (int i = 0; i < y_size; i++)
    {
        in[i] = (unsigned char *)calloc(x_size, sizeof(unsigned char));
    }

    return in;
}

void free2DArray(unsigned char** in, int y_size)
{
    for (int i = 0; i < y_size; i++)
    {
        free(in[i]);
    }
    free(in);
}

void writeFile(unsigned char** arrData, size_t sizeOfElement, size_t sizeOfEachRow, size_t sizeOfEachCol, FILE * filePointer)
{
    for (int n = 0; n < sizeOfEachRow; n++)
    {
        fwrite(arrData[n], sizeOfElement, sizeOfEachCol, filePointer);
    }
}

void distanceTransform(unsigned char** input, unsigned char** output, int dim_x, int dim_y)
{
    int dir = LEFT; //Direction traveling
    int travel = 0; //For moving the point until reaching background pixel (0 intensity).
    int min = 0;    //On the first while loop we will set this value appropriately.
    int max = 0;    //Only going to be used to divide against min value for final output.
    int tempDist = 0;

    for (int row = 0; row < dim_y; row++)
    {
        for (int col = 0; col < dim_x; col++)
        {
            //At every step we need to check a direction we are going AND if we have hit a 0 intensity
            if (input[row][col] != 0)
            {
                while (dir == LEFT)
                {
                    travel++; //To ensure we are always moving along our direction.
                    if (input[row][col - travel] == 0)
                    {
                        min = abs(row - row) + abs(col - (col - travel));
                        max = min;
                        dir = UP;
                        travel = 0; //reset travel distance.
                    }
                }
                while (dir == UP)
                {
                    travel++;
                    if (input[row - travel][col] == 0)
                    {
                        tempDist = abs(row - (row - travel)) + abs(col - col);
                        if (tempDist < min)
                        {
                            min = tempDist;
                        }
                        if (tempDist > max)
                        {
                            max = tempDist;
                        }
                        dir = RIGHT;
                        travel = 0;
                    }
                }
                while (dir == RIGHT)
                {
                    travel++;
                    if (input[row][col + travel] == 0)
                    {
                        tempDist = abs(row - row) + abs(col - (col + travel));
                        if (tempDist < min)
                        {
                            min = tempDist;
                        }
                        if (tempDist > max)
                        {
                            max = tempDist;
                        }
                        dir = DOWN;
                        travel = 0;
                    }
                }
                while (dir == DOWN)
                {
                    travel++;
                    if (input[row + travel][col] == 0)
                    {
                        tempDist = abs(row - (row + travel)) + abs(col - col);
                        if (tempDist < min)
                        {
                            min = tempDist;
                        }
                        if (tempDist > max)
                        {
                            max = tempDist;
                        }
                        dir = NONE;
                        travel = 0;
                    }
                }
                //Record results into new image's 2D array and reset direction.
                output[row][col] = input[row][col] * ((float)min / (float)max);
                dir = LEFT;
            }
            //If we get here then original image had a zero intensity so make output image have zero intensity there.
            else
            {
                output[row][col] = 0;
            }
        }
    }
}

void coOccurrenceMatrix(unsigned char ** in, unsigned char ** out, struct coordinate *off, int in_x, int in_y, int out_x, int out_y)
{
    int firstIntensity = 0;
    int secondIntensity = 0;
    int currX = off->x;
    int currY = off->y;
    int count = 1;          //Count occurrences of matching pair of intensities for i -> x  and j -> y.
    int firstTime = TRUE;

    for (int row = 0; row < out_y; row++)
    {
        for (int col = 0; col < out_x; col++)
        {
            firstIntensity = row;
            secondIntensity = col;

            //Iterate through the entire image for each intensity check of Matrix
            for (int inRow = 0; inRow < in_y; inRow++)
            {
                for (int inCol = 0; inCol < in_x; inCol++)
                {
                    if (inRow + currY < in_y && inCol + currX < in_x)
                    {
                        if (in[inRow][inCol] == firstIntensity && in[inRow + currY][inCol + currX] == secondIntensity)
                        {
                            count++;
                        }
                    }
                }
            }
            //Reached here means we've went one cycle of checks so store count for that pair of intensity levels.
            if (count > 0)
            {
                //TODO: Fix this -- ask professor.
                out[row][col] = 255 * ((float)count / (float)3);
            }
            else
            {
                out[row][col] = 0;
            }
            count = 0; //reset counter.
        }
    }
}

int main()
{
    //Set up dimensions
    int square_x = 60;
    int square_y = 60;
    int cktboard_x = 365;
    int cktboard_y = 120;
    int cktboard_out_x = 256;
    int cktboard_out_y = 256;

    //Set up strings
    char square_in_filename[100], square_out_filename[100], cktboard_in_filename[100], cktboard_out_filename[100];

    strcpy(square_in_filename, SQUARE_PATH);
    strcpy(square_out_filename, SQUARE_OUTPATH);
    strcpy(cktboard_in_filename, CIRCUIT_PATH);
    strcpy(cktboard_out_filename, CIRCUIT_OUTPATH);

    //Set up file pointers and initialize
    FILE * square_in_fp = fopen(square_in_filename, "rb");
    FILE * cktboard_in_fp = fopen(cktboard_in_filename, "rb");
    FILE * square_out_fp = fopen(square_out_filename, "wb");
    FILE * cktboard_out_fp = fopen(cktboard_out_filename, "wb");

    //Set up 2D arrays for file reading/writing
    unsigned char ** square_input = alloc2DArray(square_x, square_y);
    unsigned char ** square_output = alloc2DArray(square_x, square_y);
    unsigned char ** cktboard_input = alloc2DArray(cktboard_x, cktboard_y);
    unsigned char ** cktboard_output = alloc2DArray(cktboard_out_x, cktboard_out_y);

    //Set up for distance transform
    readFromFile(square_in_fp, square_input, square_x, square_y);

    //Do Distance Transform
    distanceTransform(square_input, square_output, square_x, square_y);

    //Copy results to file
    writeFile(square_output, sizeof(unsigned char), square_y, square_x, square_out_fp);

    //Set up offset operator
    struct coordinate offset;
    offset.x = 1;
    offset.y = 0;

    //Set up for co-occurrence matrix
    readFromFile(cktboard_in_fp, cktboard_input, cktboard_x, cktboard_y);

    //Do co-occurrence operation.
    coOccurrenceMatrix(cktboard_input, cktboard_output, &offset, cktboard_x, cktboard_y, cktboard_out_x, cktboard_out_y);

    //Write results into file for co-occurrence matrix
    writeFile(cktboard_output, sizeof(unsigned char), cktboard_out_y, cktboard_out_x, cktboard_out_fp);

    //free 2D array pointers from memory
    free2DArray(square_input, square_y);
    free2DArray(square_output, square_y);
    free2DArray(cktboard_input, cktboard_y);
    free2DArray(cktboard_output, cktboard_out_y);

    //Close file pointers
    fclose(square_out_fp);
    fclose(cktboard_out_fp);
    fclose(square_in_fp);
    fclose(cktboard_in_fp);

    return 0;
}
