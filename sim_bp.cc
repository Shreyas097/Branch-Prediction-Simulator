#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "sim_bp.h"

/*  argc holds the number of command line arguments
    argv[] holds the commands themselves

    Example:-
    sim bimodal 6 gcc_trace.txt
    argc = 4
    argv[0] = "sim"
    argv[1] = "bimodal"
    argv[2] = "6"
    ... and so on
*/
int main (int argc, char* argv[])
{
    FILE *FP;               // File handler
    char *trace_file;       // Variable that holds trace file name;
    bp_params params;       // look at sim_bp.h header file for the the definition of struct bp_params
    char outcome;           // Variable holds branch outcome
    unsigned long int addr; // Variable holds the address read from input file
    
    if (!(argc == 4 || argc == 5 || argc == 7))
    {
        printf("Error: Wrong number of inputs:%d\n", argc-1);
        exit(EXIT_FAILURE);
    }
    
    params.bp_name  = argv[1];
    
    // strtoul() converts char* to unsigned long. It is included in <stdlib.h>
    if(strcmp(params.bp_name, "bimodal") == 0)              // Bimodal
    {
        if(argc != 4)
        {
            printf("Error: %s wrong number of inputs:%d\n", params.bp_name, argc-1);
            exit(EXIT_FAILURE);
        }
        params.M2       = strtoul(argv[2], NULL, 10);
        trace_file      = argv[3];
        printf("COMMAND\n%s %s %lu %s\n", argv[0], params.bp_name, params.M2, trace_file);

		bp_type = bimodal;	
		M2 = params.M2;
		b_table_size = pow(2,params.M2);
		b_table = (int *)malloc((b_table_size)*sizeof(int));
		for(int i = 0; i<b_table_size; i++)
		{
			b_table[i] = 2;
		}
    }
    else if(strcmp(params.bp_name, "gshare") == 0)          // Gshare
    {
        if(argc != 5)
        {
            printf("Error: %s wrong number of inputs:%d\n", params.bp_name, argc-1);
            exit(EXIT_FAILURE);
        }
        params.M1       = strtoul(argv[2], NULL, 10);
        params.N        = strtoul(argv[3], NULL, 10);
        trace_file      = argv[4];
        printf("COMMAND\n%s %s %lu %lu %s\n", argv[0], params.bp_name, params.M1, params.N, trace_file);

		bp_type = gshare;
		M1 = params.M1;
		N = params.N;		
		g_table_size = pow(2,params.M1);
		g_table = (int *)malloc((g_table_size)*sizeof(int));
		for(int i = 0; i<g_table_size; i++)
		{
			g_table[i] = 2;
		}
		
		bhr = 0;
    }
    else if(strcmp(params.bp_name, "hybrid") == 0)          // Hybrid
    {
        if(argc != 7)
        {
            printf("Error: %s wrong number of inputs:%d\n", params.bp_name, argc-1);
            exit(EXIT_FAILURE);
        }
        params.K        = strtoul(argv[2], NULL, 10);
        params.M1       = strtoul(argv[3], NULL, 10);
        params.N        = strtoul(argv[4], NULL, 10);
        params.M2       = strtoul(argv[5], NULL, 10);
        trace_file      = argv[6];
        printf("COMMAND\n%s %s %lu %lu %lu %lu %s\n", argv[0], params.bp_name, params.K, params.M1, params.N, params.M2, trace_file);

		bp_type = hybrid;
		M2 = params.M2;
		M1 = params.M1;
		N = params.N;
		K = params.K;
		
		b_table_size = pow(2,params.M2);
		b_table = (int *)malloc((b_table_size)*sizeof(int));
		for(int i = 0; i<b_table_size; i++)
		{
			b_table[i] = 2;
		}
		
		g_table_size = pow(2,params.M1);
		g_table = (int *)malloc((g_table_size)*sizeof(int));
		for(int i = 0; i<g_table_size; i++)
		{
			g_table[i] = 2;
		}
		bhr = 0;
		
		c_table_size = pow(2,params.K);
		c_table = (int *)malloc((c_table_size)*sizeof(int));
		for(int i = 0; i<c_table_size; i++)
		{
			c_table[i] = 1;
		}
    }
    else
    {
        printf("Error: Wrong branch predictor name:%s\n", params.bp_name);
        exit(EXIT_FAILURE);
    }
    
    // Open trace_file in read mode
    FP = fopen(trace_file, "r");
    if(FP == NULL)
    {
        // Throw error and exit if fopen() failed
        printf("Error: Unable to open file %s\n", trace_file);
        exit(EXIT_FAILURE);
    }
	
	
    char str[2];
    while(fscanf(FP, "%lx %s", &addr, str) != EOF)
    {
		total++;
        outcome = str[0];

		
		if(outcome == 't')
			original_prediction = taken;
		else if(outcome == 'n')
			original_prediction = not_taken;
		
		pc_addr = addr >> 2;
		sprintf(address, "%lx", pc_addr);

		binary = change_to_binary(address);		


		if(bp_type == bimodal)
		{
			b_index = get_b_index(M2);										//Getting index
			b_predict = do_predict(b_table,b_index);						// Making prediction
			update_index(b_table,b_index,original_prediction,b_predict);	// Updating index
		}
		else if(bp_type == gshare)
		{
			g_index = get_g_index(M1,N);									//Getting index
			g_predict = do_predict(g_table,g_index);						// Making prediction
			update_index(g_table,g_index,original_prediction,g_predict);	// Updating index
			bhr_place_original = (original_prediction<<(N-1));
			bhr = (bhr_place_original)|(bhr>>1);							// Updating BHR
		}
		else if(bp_type == hybrid)
		{
			// Getting chooser table index
			c_index = get_c_index(K);
			
			// Getting Bimodal table index
			b_index = get_b_index(M2);
			b_predict = do_predict(b_table,b_index);
			
			// Getting Gshare table index
			g_index = get_g_index(M1,N);
			g_predict = do_predict(g_table,g_index);
			
			if(c_table[c_index]>=2)
				update_index(g_table,g_index,original_prediction,g_predict);
			else
				update_index(b_table,b_index,original_prediction,b_predict);
			
			if((original_prediction==g_predict)&&(original_prediction!=b_predict))
			{
				if(c_table[c_index]<3)
					c_table[c_index]++;
			}
			else if((original_prediction!=g_predict)&&(original_prediction==b_predict))
			{
				if(c_table[c_index]>0)
					c_table[c_index]--;
			}
		
			bhr_place_original = (original_prediction<<(N-1));
			bhr = (bhr_place_original)|(bhr>>1);								// Updating BHR
		}
    }
	
	float tot,mp;
	char percent_symbol = '%';
	tot = total;
	mp = mis_predict;
	printf("OUTPUT");
	printf("\n number of predictions: %d", total);
	printf("\n number of mispredictions: %d", mis_predict);
	printf("\n misprediction rate: %.2f", mp/tot*100);
	printf("%c",percent_symbol);
	if(bp_type == bimodal)
	{
		printf("\nFINAL BIMODAL CONTENTS");
		for(int i = 0; i<b_table_size; i++)
			printf("\n %d\t%d", i, b_table[i]);
		printf("\n");
	}
	else if(bp_type == gshare)
	{
		printf("\nFINAL GSHARE CONTENTS");
		for(int i = 0; i<g_table_size; i++)
			printf("\n %d\t%d", i, g_table[i]);
		printf("\n");
	}
	else if(bp_type == hybrid)
	{
		printf("\nFINAL CHOOSER CONTENTS");
		for(int i = 0; i<c_table_size; i++)
			printf("\n %d\t%d", i, c_table[i]);
		printf("\nFINAL GSHARE CONTENTS");
		for(int i = 0; i<g_table_size; i++)
			printf("\n %d\t%d", i, g_table[i]);
		printf("\nFINAL BIMODAL CONTENTS");
		for(int i = 0; i<b_table_size; i++)
			printf("\n %d\t%d", i, b_table[i]);
		printf("\n");
	}
    return 0;
}

// Converting hex to binary
char *change_to_binary(char address[])
{
	char *binary = (char *)malloc(sizeof(char) * 10000);
	unsigned long int i;
	for(i=0; i < strlen(address); ++i)
	{
		if (address[i] == '0')
			strcat(binary, "0000");
		else if (address[i] == '1')
			strcat(binary, "0001");
		else if (address[i] == '2')
			strcat(binary, "0010");
		else if (address[i] == '3')
			strcat(binary, "0011");
		else if (address[i] == '4')
			strcat(binary, "0100");
		else if (address[i] == '5')
			strcat(binary, "0101");
		else if (address[i] == '6')
			strcat(binary, "0110");
		else if (address[i] == '7')
			strcat(binary, "0111");
		else if (address[i] == '8')
			strcat(binary, "1000");
		else if (address[i] == '9')
			strcat(binary, "1001");
		else if (address[i] == 'a' || address[i] == 'A')
			strcat(binary, "1010");
		else if (address[i] == 'b' || address[i] == 'B')
			strcat(binary, "1011");
		else if (address[i] == 'c' || address[i] == 'C')
			strcat(binary, "1100");
		else if (address[i] == 'd' || address[i] == 'D')
			strcat(binary, "1101");
		else if (address[i] == 'e' || address[i] == 'E')
			strcat(binary, "1110");
		else if (address[i] == 'f' || address[i] == 'F')
			strcat(binary, "1111");
	}
	return binary;
}

// Getting Bimodal index
unsigned long int get_b_index(unsigned long int M2)
{
	char *index = (char *)malloc(sizeof(char) * M2);
	char temp[M2];
	int k = 0;
	for(unsigned long int i = strlen(binary)-M2; i < strlen(binary); i++)
	{
		temp[k] = binary[i];
		++k;
	}
	char temp1[M2];
	int j = 0;
	for(unsigned long int i = 0; i < M2; i++)
	{
		temp1[j] = temp[j];
		++j;
	}
	strcat(index,temp1);
	unsigned long int b_index = strtoul(index,NULL,2);
	return b_index;
}

// Getting Gshare index
unsigned long int get_g_index(unsigned long int M1, unsigned long int N)
{
	unsigned long int g_index;
	int diff = M1-N;
	int m1_bits = pow(2,M1) - 1;
	int n_bits = pow(2,N) - 1;
	int diff_bits = pow(2,diff) - 1;
	int m1 = (pc_addr & m1_bits);
	int n = (bhr & n_bits);
	int m1_n = (m1 & diff_bits);
	int temp1 = (m1>>diff) ^ n;
	int temp2 = temp1<<diff;
	
	g_index = temp2|m1_n;
//	printf("\n Gshare Index = %lx", g_index);

	return g_index;
	
}

// Getting Hybrid index
unsigned long int get_c_index(unsigned long int K)
{
	char *index = (char *)malloc(sizeof(char) * K);
	char temp[K];
	int k = 0;
	for(unsigned long int i = strlen(binary)-K; i < strlen(binary); i++)
	{
		temp[k] = binary[i];
		++k;
	}
	strcat(index,temp);
	unsigned long int c_index = strtoul(index,NULL,2);
	return c_index;
}
	
// Perform prediction
int do_predict(int *table, unsigned long int index)
{
	int prediction;
	if(table[index] >= 2)
		prediction = taken;
	else
		prediction = not_taken;
	return prediction;
}

// Updating Index
void update_index(int *table, unsigned long int index, int original, int prediction)
{
		if(prediction != original)
			mis_predict++;
		
		if(original == taken)
		{
			if(table[index] < 3)
				table[index]++;
		}
		else if(original == not_taken)
		{
			if(table[index] > 0)
				table[index]--;
		}
		
}