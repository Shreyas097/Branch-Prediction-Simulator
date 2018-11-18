#ifndef SIM_BP_H
#define SIM_BP_H

typedef struct bp_params{
    unsigned long int K;
    unsigned long int M1;
    unsigned long int M2;
    unsigned long int N;
    char*             bp_name;
}bp_params;

// Put additional data structures here as per your requirement

#define bimodal 0
#define gshare 1
#define hybrid 2

#define taken 1
#define not_taken 0

// General variables
unsigned long int pc_addr;
char address[50];
char *binary;
int bp_type;
int original_prediction;
int mis_predict = 0;
int total = 0;

// Bimodal variables
int *b_table;
int b_table_size;
unsigned long int b_index;
unsigned long int M2;
int b_predict;

// Gshare variables
int *g_table;
int g_table_size;
unsigned long int g_index;
unsigned long int M1;
unsigned long int N;
int bhr;
int bhr_place_original;
int g_predict;

// Hybrid variables - M1,M2,N already declared
unsigned long int K;
int *c_table;
int c_table_size;
unsigned long int c_index;

char *change_to_binary(char address[]);
unsigned long int get_b_index(unsigned long int);
int binary_decimal(int);
int do_predict(int*,unsigned long int);
void update_index(int*,unsigned long int,int,int);
unsigned long int get_g_index(unsigned long int,unsigned long int);
unsigned long int get_c_index(unsigned long int);
#endif
