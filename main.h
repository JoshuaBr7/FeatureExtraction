/*
Header file for declaring functions
*/

void readFromFile(FILE * fp, unsigned char **arr, int dim_x, int dim_y);
unsigned char ** alloc2DArray(int x_size, int y_size);
void free2DArray(unsigned char** in, int x_size);
void writeFile(unsigned char** arrData, size_t sizeOfElement, size_t sizeOfEachRow, size_t sizeOfEachCol, FILE * filePointer);
void distanceTransform(unsigned char** input, unsigned char** output, int dim_x, int dim_y);
