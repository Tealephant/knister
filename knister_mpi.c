#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <mpi.h>

#define N 5
#define M 11
#define ROOT 0

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

void print_board(int* board)
{
    for(int i = 0; i < N; i++) {
        print_seperator_line(N);
        for(int j = 0; j < N; j++) {
            printf("|");
            printf("%2d", board[i * N + j]);
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

/*
int max_occurencies(int* row)
{
    int max = 0;
    for(int i = 0; i < N; i++) {
        int check = row[i];
        int count = 1;
        for(int j = i + 1; j < N; j++) {
            if(row[j] == check) {
                count++;
            }
        }
        if(count > max) {
            max = count;
        }
        if(max >= N - i - 1)
        {
            //besseres Maximum kann nicht gefunden werden
            break;
        }
    }
    return max;
}
*/

void print_row(int* row)
{
    for(int i = 0; i < N; i++) {
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

int get_board_value(int* board)
{
    int board_value = 0;
    //count rows
    for(int i = 0; i < N; i++) {
        board_value += get_row_value(board + i * N);
    }
    //count columns
    int buffer[N];
    for(int j = 0; j < N; j++) {
        for(int i = 0; i < N; i++) {
            buffer[i] = board[i * N + j];
        }
        board_value += get_row_value(buffer);
    }
    //count diagonals
    for(int i = 0; i < N; i++) {
        buffer[i] = board[i * N + i];
    }
    board_value += 2 * get_row_value(buffer);
    for(int i = 0; i < N; i++)
    {
        buffer[i] = board[(N - i - 1) * N + i];
    }
    board_value += 2 * get_row_value(buffer);
    return board_value;
}

int board1[N][N] = {
    {9,2,10,8,10},
    {8,5,8,8,7},
    {7,7,7,7,7},
    {6,6,7,6,6},
    {10,2,4,8,8}
};

int board2[N][N] = {
    {7,6,9,5,8},
    {4,6,6,8,4},
    {10,8,8,10,10},
    {10,8,10,5,5},
    {8,6,7,10,9}
};

void copy_board(int* target, int source[N][N])
{
    for(int i = 0; i < N; i++) {
        for(int j = 0; j < N; j++) {
            target[i * N + j] = source[i][j];
        }
    }
}

void copy_verfuegbar(int* target, int source[M])
{
    for(int i = 0; i < M; i++) {
        target[i] = source[i];
    }
}

void find_solution(int* board, int pos, int* verfuegbar, int* best_yet, int* best_board)
{
    if(pos == N * N) {
        int new_points = get_board_value(board);
        if(new_points > *best_yet) {
            *best_yet = new_points;
            for(int i = 0; i < N; i++) {
                for(int j = 0; j < N; j++) {
                    best_board[i * N + j] = board[i * N + j];
                }
            }
        }
    }
    else {
        for(int k = 0; k < M; k++) {
            if(verfuegbar[k] > 0) {
                board[(pos / N) * N + pos % N] = k + 2;
                verfuegbar[k]--;
                find_solution(board, pos + 1, verfuegbar, best_yet, best_board);
                verfuegbar[k]++;
                
            }
        }
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

void find_best_rest(int* board, int pos, int* verfuegbar, int* verfuegbar_diagonal, int* best_yet, int* best_board, int rank)
{
    if(pos == N * N) {
        int new_points = get_board_value(board);
        if(new_points > *best_yet) {
            *best_yet = new_points;
            printf("Beste bisherige Lösung von Prozess %d: %d\n", rank, *best_yet);
            print_board(best_board);
            for(int i = 0; i < N; i++) {
                for(int j = 0; j < N; j++) {
                    best_board[i * N + j] = board[i * N + j];
                }
            }
        }
    }
    else if(pos / N == pos % N || N - pos / N - 1 == pos % N) {
        //Feld liegt auf Diagonalen
        for(int k = 0; k < M; k++) {
            if(verfuegbar_diagonal[k] > 0) {
                board[(pos / N) * N + pos % N] = k + 2;
                verfuegbar_diagonal[k]--;
                find_best_rest(board, pos + 1, verfuegbar, verfuegbar_diagonal, best_yet, best_board, rank);
                verfuegbar_diagonal[k]++;                
            }
        }
    }
    else {
        for(int k = 0; k < M; k++) {
            if(verfuegbar[k] > 0) {
                board[(pos / N) * N + pos % N] = k + 2;
                verfuegbar[k]--;
                find_best_rest(board, pos + 1, verfuegbar, verfuegbar_diagonal, best_yet, best_board, rank);
                verfuegbar[k]++;
            }
        }
    }
}

/*
void find_solution2(int* board, int* verfuegbar, int* best_yet, int* best_board)
{
    int* new_nine = calloc(2 * N - 1, sizeof(int));
    int* best_nine = calloc(2 * N - 1, sizeof(int));
    int best_diagonal_points = 0;
    find_best_diagonals(new_nine, 0, verfuegbar, &best_diagonal_points, best_nine);
    printf("Besten 9:\n");
    for(int i = 0; i < 9; i++) {
        printf("%d ", best_nine[i]);
    }
    printf("\n");
    int* verfuegbar_diagonal = calloc(M, sizeof(int));
    for(int i = 0; i < 2 * N - 1; i++) {
        verfuegbar_diagonal[best_nine[i] - 2]++;
        verfuegbar[best_nine[i] - 2]--;
    }
    find_best_rest(board, 0, verfuegbar, verfuegbar_diagonal, best_yet, best_board);
}
*/

double faculty(int n)
{
    double r = 1;
    for(int i = 2; i <= n; i++) {
        r *= i;
    }
    return r;
}

int main(int argc, char** argv)
{
    int numprocs, rank;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if(numprocs > 8) {
        exit(EXIT_FAILURE);
    }

    int* board = calloc(N * N, sizeof(int));
    int* new_board = calloc(N * N, sizeof(int));
    int* best_board = calloc(N * N, sizeof(int));

    int* verfuegbar = calloc(M, sizeof(int));
    int* for_rank = calloc(M, sizeof(int));
    for(int i = 0; i < M; i++) {
        for_rank[i] = -1;
    }
    int* verfuegbar_diagonal = calloc(M, sizeof(int));

    if(rank == ROOT) {
        print_board(board);
        printf("Wert dieses Boards: %d\n", get_board_value(board));
        copy_board(board, board2);

        for(int i = 0; i < N; i++) {
            for(int j = 0; j < N; j++) {
                verfuegbar[board[i * N + j] - 2]++;
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

        //Diagonalen Ausrechnen

        int* new_nine = calloc(2 * N - 1, sizeof(int));
        int* best_nine = calloc(2 * N - 1, sizeof(int));
        int best_diagonal_points = 0;
        find_best_diagonals(new_nine, 0, verfuegbar, &best_diagonal_points, best_nine);
        printf("Besten 9:\n");
        for(int i = 0; i < 9; i++) {
            printf("%d ", best_nine[i]);
        }
        printf("\n");
        for(int i = 0; i < 2 * N - 1; i++) {
            verfuegbar_diagonal[best_nine[i] - 2]++;
            verfuegbar[best_nine[i] - 2]--;
        }

        //Zahl für erstes Feld bestimmen je nach rank
        int c = 0;
        for(int i = 0; i < M; i++) {
            if(verfuegbar_diagonal[i] > 0) {
                for_rank[i] = c;                    
                c = (c + 1) % numprocs;             
            }
        } 
    }

    //Daten verteilen
    MPI_Bcast(board, N * N, MPI_INT, ROOT, MPI_COMM_WORLD);
    MPI_Bcast(verfuegbar, M, MPI_INT, ROOT, MPI_COMM_WORLD);
    MPI_Bcast(verfuegbar_diagonal, M, MPI_INT, ROOT, MPI_COMM_WORLD);
    MPI_Bcast(for_rank, M, MPI_INT, ROOT, MPI_COMM_WORLD);

    //Startzahl empfangen
    int best = 0;
    //Eigenes Feld mit Startzahl belegen
    for(int i = 0; i < M; i++) {
        if(for_rank[i] == rank) {
            new_board[0] = i + 2;
            verfuegbar_diagonal[i]--;
            //rechnen
            find_best_rest(new_board, 1, verfuegbar, verfuegbar_diagonal, &best, best_board, rank);     
        }
    }

    
    //root sollte theoretisch empfangen, aber bis dahin kommt man eh nicht   
    
    
    
    printf("Best possible value: %d\n", best);

    MPI_Finalize();
    return 0;
}
 
