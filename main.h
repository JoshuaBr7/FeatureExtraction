/*
Header file for declaring functions
*/
struct coordinate
{
    int x;
    int y;
};
void readFromFile(FILE * fp, unsigned char **arr, int dim_x, int dim_y);
unsigned char ** alloc2DArray(int x_size, int y_size);
void free2DArray(unsigned char** in, int x_size);
void writeFile(unsigned char** arrData, size_t sizeOfElement, size_t sizeOfEachRow, size_t sizeOfEachCol, FILE * filePointer);
void distanceTransform(unsigned char** input, unsigned char** output, int dim_x, int dim_y);
void coOccurrenceMatrix(unsigned char ** in, unsigned char ** out, struct coordinate *off, int in_x, int in_y, int out_x, int out_y);
