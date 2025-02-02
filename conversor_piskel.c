#include <stdio.h>

#define MATRIX_ROWS 5
#define MATRIX_COLS 5
#define MATRIX_DEPTH 3
#define NUM_SCREENS 3

// Função para converter valores ARGB (0xAARRGGBB) para RGB
void convertToRGB(int argb, int rgb[3]) {
    rgb[0] = argb & 0xFF;          // Blue
    rgb[2] = (argb >> 16) & 0xFF;  // Red
    rgb[1] = (argb >> 8) & 0xFF;   // Green
}

int main() {
    // Matrizes de entrada com os valores ARGB para duas telas
    int argb_values[NUM_SCREENS][MATRIX_ROWS * MATRIX_COLS] = {
      {
0xff0000ff, 0xff0000ff, 0xff0000ff, 0x00000000, 0x00000000, 
0xff0000ff, 0x00000000, 0x00000000, 0xff0000ff, 0x00000000, 
0xff0000ff, 0x00000000, 0x00000000, 0xff0000ff, 0x00000000, 
0xff0000ff, 0x00000000, 0x00000000, 0xff0000ff, 0x00000000, 
0xff0000ff, 0xff0000ff, 0xff0000ff, 0x00000000, 0x00000000
},
{
0x00000000, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0x00000000, 
0x00000000, 0xff0000ff, 0x00000000, 0xff0000ff, 0x00000000, 
0x00000000, 0xff0000ff, 0x00000000, 0xff0000ff, 0x00000000, 
0x00000000, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0x00000000, 
0x00000000, 0xff0000ff, 0x00000000, 0xff0000ff, 0x00000000
},
{
0xff0000ff, 0x00000000, 0x00000000, 0x00000000, 0xff0000ff, 
0x00000000, 0xff0000ff, 0x00000000, 0xff0000ff, 0x00000000, 
0x00000000, 0x00000000, 0xff0000ff, 0x00000000, 0x00000000, 
0x00000000, 0x00000000, 0xff0000ff, 0x00000000, 0x00000000, 
0x00000000, 0x00000000, 0xff0000ff, 0x00000000, 0x00000000
}
    };

    // Matrizes 5x5x3 para armazenar os valores RGB para duas telas
    int rgb_matrices[NUM_SCREENS][MATRIX_ROWS][MATRIX_COLS][MATRIX_DEPTH];

    // Preencher as matrizes RGB com a conversão dos valores ARGB
    for (int screen = 0; screen < NUM_SCREENS; screen++) {
        for (int i = 0; i < MATRIX_ROWS * MATRIX_COLS; i++) {
            int rgb[3];
            convertToRGB(argb_values[screen][i], rgb);

            int row = i / MATRIX_COLS;    // Cálculo da linha
            int col = i % MATRIX_COLS;    // Cálculo da coluna

            // Armazenar os valores RGB na matriz 5x5x3 correspondente
            rgb_matrices[screen][row][col][0] = rgb[0];  // Blue
            rgb_matrices[screen][row][col][1] = rgb[1];  // Green
            rgb_matrices[screen][row][col][2] = rgb[2];  // Red
        }
    }

    // Exibir as matrizes RGB resultantes
    for (int screen = 0; screen < NUM_SCREENS; screen++) {
        printf("Tela %d:\n{\n", screen + 1);
        for (int i = 0; i < MATRIX_ROWS; i++) {
            printf("    {");
            for (int j = 0; j < MATRIX_COLS; j++) {
                printf("{%d, %d, %d}", rgb_matrices[screen][i][j][0], rgb_matrices[screen][i][j][1], rgb_matrices[screen][i][j][2]);
                if (j < MATRIX_COLS - 1) {
                    printf(", ");
                }
            }
            printf("}");
            if (i < MATRIX_ROWS - 1) {
                printf(",");
            }
            printf("\n");
        }
        printf("}\n");
    }

    return 0;
}
