#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <mpi.h>

#define T 8
#define M 11

int* apply_permutation(int* belegung, int* permutation)
{
	int* ergebnis = calloc(T, sizeof(int));
	for(int i = 0; i < T; i++) {
		ergebnis[i] = belegung[permutation[i] - 1];
	}
	return ergebnis;
}

bool cmp_belegungen(int* belegung1, int* belegung2) {
	//true <=> belegung1 = belegung2
	for(int i = 0; i < T; i++) {
		if(belegung1[i] != belegung2[i]) {
			return false;
		}
	}
	return true;
}

int* convert_array(int source[T]) {
	int* target = calloc(T, sizeof(int));
	for(int i = 0; i < T; i++) {
		target[i] = source[i];
	}
	return target;
}

int* copy_array(int* source) {
	int* target = calloc(T, sizeof(int));
	for(int i = 0; i < T; i++) {
		target[i] = source[i];
	}
	return target;
}

void find_belegungen(int* belegung, int* verfuegbar1, int* verfuegbar2, int pos, int** belegungen, int* beleg_pos, int** permutationen)
{
	if(pos == 8) {
		//check if found belegung can be created by permuting previous ones
		bool unique = true;
		for(int i = 0; i < *beleg_pos; i++) {
			for(int j = 0; j < 64; j++) {
				int* tmp = apply_permutation(belegungen[i], permutationen[j]);
				if(cmp_belegungen(belegung, tmp)) {
					unique = false;
					break;
				}
			}
		}
		if(unique) {
			belegungen[*beleg_pos] = copy_array(belegung);
			for(int i = 0; i < T; i++) {
				printf("%d ", belegung[i]);
			}
			printf("\n");
			(*beleg_pos)++;
		}
	}
	else if(pos < T/2){
		for(int k = 0; k < M; k++) {
            if(verfuegbar1[k] > 0) {
                belegung[pos] = k + 2;
                verfuegbar1[k]--;
                find_belegungen(belegung, verfuegbar1, verfuegbar2, pos + 1, belegungen, beleg_pos, permutationen);
                verfuegbar1[k]++;                
            }
        }
	}
	else {
		for(int k = 0; k < M; k++) {
            if(verfuegbar2[k] > 0) {
                belegung[pos] = k + 2;
                verfuegbar2[k]--;
                find_belegungen(belegung, verfuegbar1, verfuegbar2, pos + 1, belegungen, beleg_pos, permutationen);
                verfuegbar2[k]++;                
            }
        }
	}
}

int main(int argc, char** argv)
{
	//Permutationen einlesen
	FILE* file = fopen("permutations.txt", "r");
	int** permutationen = calloc(64, sizeof(int*));

	for(int i = 0; i < 64; i++) {
		permutationen[i] = calloc(8, sizeof(int));
		for(int j = 0; j < 8; j++) {
			fscanf(file, "%d", &permutationen[i][j]);
		}
	}
	int input[9] = {4, 5, 7, 8, 6, 6, 6, 6, 6};
	//int* converted = convert_array(input);



	int* verfuegbar1 = calloc(M, sizeof(int));
	int* verfuegbar2 = calloc(M, sizeof(int));
	for(int i = 0; i < T/2; i++) {
        verfuegbar1[input[i] - 2]++;
        verfuegbar2[input[i + T/2 + 1] - 2]++;
    }

    int** belegungen = calloc(630, sizeof(int*));
    int* belegung = calloc(T, sizeof(int));
    int beleg_pos = 0;
    find_belegungen(belegung, verfuegbar1, verfuegbar2, 0, belegungen, &beleg_pos, permutationen);
    for(int i = 0; i < 630; i++) {
    	if(belegungen[i] != NULL) {
    		for(int j = 0; j < T; j++) {
    			printf("%d ", belegungen[i][j]);
    		}
    		printf("\n");
    	}
    }

    int* permu = apply_permutation(belegungen[0], permutationen[1]);
    printf("\n");
    for(int i = 0; i < T; i++) {
    	printf("%d ", permu[i]);
    }
    printf("\n");

	return 0;
}
 
