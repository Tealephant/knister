#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#define N 5
#define M 11
#define T 8

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

enum points{
    zweier = 1,
    dreier = 3,
    vierer = 6,
    funfer = 10,
    zweizweier = 3,
    fullhouse = 8,
    strmit = 8,
    strohne = 12,
};

void print_seperator_line(int n)
{
    for(int i = 0; i < N; i++) {
        printf("+--");
    }
    printf("+\n");
}

void print_board(int** board)
{
    for(int i = 0; i < N; i++) {
        print_seperator_line(N);
        for(int j = 0; j < N; j++) {
            printf("|");
            printf("%2d", board[i][j]);
        }
        printf("|\n");
    }
    print_seperator_line(N);
}

int get_min(int* row)
{
    int min = row[0];
    for(int i = 1; i < N; i++) {
        if(row[i] < min) {
            min = row[i];
        }
    }
    return min;
}

bool contains_number(int* row, int n)
{
    for(int i = 0; i < N; i++) {
        if(row[i] == n) {
            return true;
        }
    }
    return false;
}

bool has_strohne(int* row)
{
    int min = get_min(row);
    if(min < 3 || min > 7) {
        //Strasse enthält nicht 7
        for(int i = 1; i < N; i++) {
            if(!contains_number(row, min + i)) {
                return false;
            }
        }
        return true;
    }
    else {
        return false;
    }    
}

bool has_strmit(int* row)
{
    int min = get_min(row);
    if(min >= 3 && min <= 7) {
        //Strasse enthält 7
        for(int i = 1; i < N; i++) {
            if(!contains_number(row, min + i)) {
                return false;
            }
        }
        return true;
    }
    else {
        return false;
    }    
}

int fullhouse_or_vierer(int* row)
{
    //prüft ob fullhouse oder vierer und gibt direkt passende Punktzahl zurück
    //darf nur mit zeilen verwendet werden, von denen bekannt ist,
    //dass sie nur zwei verschiedene Zahlen beinhalten
    int count = 1;
    for(int i = 1; i < N; i++) {
        if(row[i] == row[0]) {
            count++;
        }
    }
    if(count == 2 || count == 3) {
        return fullhouse;
    }
    else {
        return vierer;
    }
}

/* 
Optimierung: zweizweier und dreier geben jeweil 3 Punkte und es gilt
(Zeile hat zweizweier oder einen dreier) <=> Zeile hat genau 3 verschiedene Zahlen
Das ist deutlich leichter erkennbar
*/

int count_different_numbers(int* row)
{
    int count = 0;
    int buffer[N];
    for(int i = 0; i < N; i++) {
        //check if already occured
        bool occured = false;
        for(int j = 0; j < count; j++) {
            if(row[i] == buffer[j]) {
                occured = true;
                break;
            }
        }
        if(!occured) {
            buffer[count] = row[i];
            count++;
        }
    }
    return count;
}

void print_row(int* row)
{
    for(int i = 0; i < N; i++) {
        printf("%2d ", row[i]);
    }
    printf("\n");
}

void print_array(int* row, int n)
{
    for(int i = 0; i < n; i++) {
        printf("%2d ", row[i]);
    }
    printf("\n");
}


int get_row_value(int* row)
{
    switch (count_different_numbers(row))
    {
    case 1:
        return funfer;
        break;
    case 2:
        //4er und fullhouse möglich
        return fullhouse_or_vierer(row);
        break;
    case 3:
        //dreier und zweizweier möglich, geben aber gleiche Punktzahl
        return dreier;
        break;
    case 4:
        return zweier;
        break;
    case 5:
        if(has_strmit(row)) {
            return strmit;
        }
        else if(has_strohne(row)) {
            return strohne;
        }
        else {
            return 0;
        }
        break;
    
    default:
        break;
    }
    return 0;
}

int get_board_value(int** board)
{
    int board_value = 0;
    //count rows
    for(int i = 0; i < N; i++) {
        board_value += get_row_value(board[i]);
    }
    //count columns
    int buffer[N];
    for(int j = 0; j < N; j++) {
        for(int i = 0; i < N; i++) {
            buffer[i] = board[i][j];
        }
        board_value += get_row_value(buffer);
    }
    //count diagonals
    for(int i = 0; i < N; i++) {
        buffer[i] = board[i][i];
    }
    board_value += 2 * get_row_value(buffer);
    for(int i = 0; i < N; i++)
    {
        buffer[i] = board[N - i - 1][i];
    }
    board_value += 2 * get_row_value(buffer);
    return board_value;
}

void copy_board(int** target, int source[N][N])
{
    for(int i = 0; i < N; i++) {
        for(int j = 0; j < N; j++) {
            target[i][j] = source[i][j];
        }
    }
}

void copy_verfuegbar(int* target, int source[M])
{
    for(int i = 0; i < M; i++) {
        target[i] = source[i];
    }
}


//besten Zahlen für die Diagonalen finden
void find_best_diagonals(int* long_row, int pos, int* verfuegbar, int* best_yet, int* best_nine)
{
    if(pos == 2 * N - 1) {
        int new_points = get_row_value(long_row) + get_row_value(long_row + N - 1);
        if(new_points > *best_yet) {
            *best_yet = new_points;
            for(int i = 0; i < 2 * N - 1; i++) {
                best_nine[i] = long_row[i];
            }
        }
    }
    else {
        for(int k = 0; k < M; k++) {
            if(verfuegbar[k] > 0) {
                long_row[pos] = k + 2;
                verfuegbar[k]--;
                find_best_diagonals(long_row, pos + 1, verfuegbar, best_yet, best_nine);
                verfuegbar[k]++;            
            }
        }
    }    
}

void find_best_rest(int** board, int pos, int* verfuegbar, int* best_yet, int** best_board, clock_t starttime)
{
    static unsigned long long combination_count = 0;
    if(pos >= N * N) {
        combination_count++;
        if(combination_count % 1000000 == 0) {
            clock_t time_now = clock();
            int cps = (double)combination_count / ((double)(time_now - starttime) / CLOCKS_PER_SEC);
            printf("%lld Kombinationen berechnet. %d Kombinationen pro Sekunde.\n", combination_count, cps);
        }
        int new_points = get_board_value(board);
        if(new_points > *best_yet) {
            *best_yet = new_points;
            for(int i = 0; i < N; i++) {
                for(int j = 0; j < N; j++) {
                    best_board[i][j] = board[i][j];
                }
            }
            printf("Beste bisherige Lösung: %d\n", *best_yet);
            print_board(best_board);
        }
    }
    else {
        for(int k = 0; k < M; k++) {
            if(verfuegbar[k] > 0) {
                board[pos / N][pos % N] = k + 2;
                verfuegbar[k]--;
                int new_pos = pos + 1;
                if(new_pos / N == new_pos % N || N - new_pos / N - 1 == new_pos % N) {
                    new_pos++;
                }
                find_best_rest(board, new_pos, verfuegbar, best_yet, best_board, starttime);
                verfuegbar[k]++;
            }
        }
    }
}

void find_solution(int* verfuegbar, int** permutationen, int* best, int** best_board)
{
    int** board = calloc(N, sizeof(int*));
    for(int i = 0; i < N; i++) {
        board[i] = calloc(N, sizeof(int));
    }

    //besten Diagonalen finden
    int* new_nine = calloc(2 * N - 1, sizeof(int));
    int* best_nine = calloc(2 * N - 1, sizeof(int));
    int best_diagonal_points = 0;
    find_best_diagonals(new_nine, 0, verfuegbar, &best_diagonal_points, best_nine);

    //Belegungen für Diagonalen erzeugen
    int* verfuegbar1 = calloc(M, sizeof(int));
    int* verfuegbar2 = calloc(M, sizeof(int));
    for(int i = 0; i < T/2; i++) {
        verfuegbar1[best_nine[i] - 2]++;
        verfuegbar2[best_nine[i + T/2 + 1] - 2]++;

        verfuegbar[best_nine[i] - 2]--;
        verfuegbar[best_nine[i + T/2 + 1] - 2]--;
    }

    int** belegungen = calloc(630, sizeof(int*));
    int* belegung = calloc(T, sizeof(int));
    int beleg_pos = 0;
    
    find_belegungen(belegung, verfuegbar1, verfuegbar2, 0, belegungen, &beleg_pos, permutationen);

    //Diagonalbelegungen durchgehen und Rest auffüllen
    printf("\nFolgende Diagonalpermutationen reichen:\n");
    for(int k = 0; belegungen[k] != NULL; k++) {
        print_array(belegungen[k], T);
    }
    clock_t starttime = clock();
    for(int k = 0; belegungen[k] != NULL; k++) {
        //Diagonalen belegen
        for(int j = 0; j < N/2; j++) {
            board[j][j] = belegungen[k][j];
            board[j + N/2 + 1][j + N/2 + 1] = belegungen[k][j + N/2];

            board[j][N - j - 1] = belegungen[k][j + N];
            board[j + N/2 + 1][N/2 - j - 1] = belegungen[k][j + N/2 + N - 1];

            board[N/2][N/2] = belegungen[k][N];
        }

        printf("\nTeste Feld mit folgenden Diagonalen:\n");
        print_board(board);

        //Rest auffüllen
        find_best_rest(board, 1, verfuegbar, best, best_board, starttime);
    }

}

double faculty(int n)
{
    double r = 1;
    for(int i = 2; i <= n; i++) {
        r *= i;
    }
    return r;
}

int board1[N][N] = {
    {8,9,6,5,7},
    {9,7,6,5,8},
    {5,11,4,2,5},
    {7,8,6,5,4},
    {6,10,4,2,6}
};

int board2[N][N] = {
    {6,5,4,3,7},
    {5,7,4,8,6},
    {8,8,10,8,8},
    {7,6,4,8,5},
    {9,9,12,12,9}
};

int board3[N][N] = {
    {7,7,2,2,7},
    {8,6,8,4,11},
    {4,4,8,4,8},
    {5,5,5,9,9},
    {6,8,7,9,10}
};

int main(void)
{
    FILE* file = fopen("permutations.txt", "r");
    int** permutationen = calloc(64, sizeof(int*));

    for(int i = 0; i < 64; i++) {
        permutationen[i] = calloc(8, sizeof(int));
        for(int j = 0; j < 8; j++) {
            fscanf(file, "%d", &permutationen[i][j]);
        }
    }
    fclose(file);

    int** board = calloc(N, sizeof(int*));
    for(int i = 0; i < N; i++) {
        board[i] = calloc(N, sizeof(int));
        for(int j = 0; j < N; j++) {
            board[i][j] = 0;
        }
    }

    copy_board(board, board2);
    print_board(board);
    printf("Wert dieses Boards: %d\n", get_board_value(board));

    int* verfuegbar = calloc(M, sizeof(int));
    for(int i = 0; i < N; i++) {
        for(int j = 0; j < N; j++) {
            verfuegbar[board[i][j] - 2]++;
        }
    }

    for(int i = 0; i < M; i++) {
        printf("Anzahl %2d: %2d\n", i + 2, verfuegbar[i]);
    }

    //Mögliche Kombinationen mit den Verfügbaren Zahlen:
    double moeglichkeiten = faculty(N * N);
    for(int i = 0; i < M; i++) {
        moeglichkeiten /= faculty(verfuegbar[i]);
    }
    printf("Anzahl Möglichkeiten: %e\n", moeglichkeiten);
    int** new_board = calloc(N, sizeof(int*));
    for(int i = 0; i < N; i++) {
        new_board[i] = calloc(N, sizeof(int));
    }

    int** best_board = calloc(N, sizeof(int*));
    for(int i = 0; i < N; i++) {
        best_board[i] = calloc(N, sizeof(int));
    }

    int best = 0;
    find_solution(verfuegbar, permutationen, &best, best_board);
    printf("Best possible value: %d\n", best);
    return 0;
}
