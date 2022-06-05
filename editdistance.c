#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define INSERT_OP      0x01
#define DELETE_OP      0x02
#define SUBSTITUTE_OP  0x04
#define MATCH_OP       0x08
#define TRANSPOSE_OP   0x10

#define INSERT_COST	1
#define DELETE_COST	1
#define SUBSTITUTE_COST	1
#define TRANSPOSE_COST	1

void backtrace_main(int *op_matrix, char *str1, char *str2, int n, int m, int level, char **align_str, int *path);
void print_matrix( int *op_matrix, char *str1, char *str2, int n, int m);
int min_editdistance(char *str1, char *str2);
int __GetMin3(int a, int b, int c);
int __GetMin4(int a, int b, int c, int d);
void print_alignment(char **align_str, int level);
void backtrace(int *op_matrix, char *str1, char *str2, int n, int m);

// 재귀적으로 연산자 행렬을 순회하며, 두 문자열이 최소편집거리를 갖는 모든 가능한 정렬(alignment) 결과를 출력한다.
// op_matrix : 이전 상태에 대한 연산자 정보(backtrace)가 저장된 행렬 (1차원 배열임에 주의!)
// str1 : 문자열 1
// str2 : 문자열 2
// n : 문자열 1의 길이
// m : 문자열 2의 길이
// level : 재귀호출의 레벨 (0, 1, 2, ...)
// align_str : 정렬된 문자쌍들의 정보가 저장된 문자열 배열 예) "a - a", "a - b", "* - b", "ab - ba"
// path : 몇 가지의 경로가 존재하는지 보여줌
void backtrace_main(int *op_matrix, char *str1, char *str2, int n, int m, int level, char** align_str, int* path){
	int L = strlen(str2);
	int node = (L+1)*n + m;
	// op_matrix[0][0]까지 도달
	if( node <= 0){
		printf("\n[%d] ==============================\n", *path + 1);
		print_alignment(align_str, level-1);
		*path += 1;
		return;
	}
	
	else{
		int current_node = op_matrix[node];
		char str[5] = " - ";
		if((current_node & MATCH_OP) || (current_node & SUBSTITUTE_OP)){
			align_str[level][0] = str1[n-1];
			align_str[level][1] = '\0';
			strcat(align_str[level], str);
			align_str[level][4] = str2[m-1];
			align_str[level][5] = '\0';
			backtrace_main(op_matrix, str1, str2, n-1, m-1, level+1, align_str, path);
		}

		if(current_node & INSERT_OP){
			align_str[level][0] = '*';
			align_str[level][1] = '\0';
			strcat(align_str[level], str);
			align_str[level][4] =  str2[m-1];
			align_str[level][5] = '\0';
			backtrace_main(op_matrix, str1, str2, n, m-1, level+1, align_str, path);
		}
		if(current_node & DELETE_OP){
			align_str[level][0] = str1[n-1];
			align_str[level][1] = '\0';
			strcat(align_str[level], str);
			align_str[level][4] = '*';
			align_str[level][5] = '\0';
			backtrace_main(op_matrix, str1, str2, n-1, m, level+1, align_str, path);
		}
		if(current_node & TRANSPOSE_OP){
			align_str[level][0] = str1[n-2];
			align_str[level][1] = str1[n-1];
			align_str[level][2] = '\0';
			strcat(align_str[level], str);
			align_str[level][5] = str2[m-2];
			align_str[level][6] = str2[m-1];
			align_str[level][7] = '\0';
			backtrace_main(op_matrix, str1, str2, n-2, m-2, level+1, align_str, path);
		}
	}
}

// 강의 자료의 형식대로 op_matrix를 출력 (좌하단(1,1) -> 우상단(n, m))
// 각 연산자를 다음과 같은 기호로 표시한다. 삽입:I, 삭제:D, 교체:S, 일치:M, 전위:T
void print_matrix( int *op_matrix, char *str1, char *str2, int n, int m){
	
	int o;
	for(int i=1; i<=n; i++){
		printf("%c\t", str1[n-i]);
		for(int j=1; j<=m; j++){
			o = op_matrix[(m+1)*(n+1-i) + j];
			char op[5] = "";
			if(o & SUBSTITUTE_OP){
				strcat(op, "S");
			}
			if((o & MATCH_OP) == 8){
				strcat(op, "M");
			}
			if((o & INSERT_OP) == 1){
				strcat(op, "I");
			}
			if((o & DELETE_OP) == 2){
				strcat(op, "D");
			}
			if((o & TRANSPOSE_OP) == 16){
				strcat(op, "T");
			}
			printf("%s\t", op);
		}	
		printf("\n");		
	}
	printf("\t");

	for(int i=0; i<m; i++){
		printf("%c\t", str2[i]);
	}
	printf("\n");
}

// 두 문자열 str1과 str2의 최소편집거리를 계산한다.
// return value : 최소편집거리
// 이 함수 내부에서 print_matrix 함수와 backtrace 함수를 호출함
int min_editdistance( char *str1, char *str2){

	// 문자열 1,2의 길이 n, m에 저장
	int n = strlen(str1);
	int m = strlen(str2);

	// 가능한 동작을 저장하는 행렬(1차원 배열로 표현) op_matrix 선언
	int* op_matrix = (int*)malloc((n+1)*(m+1) * sizeof(int));
	assert(op_matrix != NULL);

	// editdistance 값을 저장하는 행렬 D 선언
	int** D = (int**)malloc((n+1) * sizeof(int*));
	assert(D != NULL);
	for(int i=0; i<n+1; i++){
		D[i] = (int*)malloc((m+1) * sizeof(int));
		assert(D[i] != NULL);
	}

	// D 행렬 가로세로 0번째 줄 초기화
	for(int i=0; i<n+1; i++){
		D[i][0] = i;
	}
	for(int j=0; j<m+1; j++){
		D[0][j] = j;
	}

	// op_matrix 행렬 가로세로 0번째 줄 초기화
	for(int i=1; i<n+1; i++){
		op_matrix[(m+1)*i] = DELETE_OP;
	}
	for(int j=1; j<m+1; j++){
		op_matrix[j] = INSERT_OP;
	}
	op_matrix[0] = 0;

	// D랑 op_matrix에 값 채우기
	for(int i=1; i<=n; i++){
		for(int j=1; j<=m; j++){

			// op_matirx[i][j] = 0 초기화
			op_matrix[(m+1)*i+j] = 0;

			// i, j >=2인 경우부터는 [(M / S) / I / D / T] 4가지 연산 가능
			if (i>=2 && j>=2){

				// 현재 위치에서 대응하는 각 문자열의 문자가 서로 같은 경우
				// [M / I / D] 중에서 가능 (S, T는 불가능)
				if(str1[i-1] == str2[j-1]){

					// D[i][j] 가능한 최솟값으로 초기화
					D[i][j] = __GetMin3(D[i-1][j-1], D[i-1][j]+DELETE_COST, D[i][j-1]+INSERT_COST);

					// 가능한 OP 탐색 후 op_matrix에 저장
					if(D[i][j] == D[i-1][j-1]){
						op_matrix[(m+1)*i+j] += MATCH_OP;
					}

					if(D[i][j] == D[i-1][j]+DELETE_COST){
						op_matrix[(m+1)*i+j] += DELETE_OP;
					}

					if(D[i][j] == D[i][j-1]+INSERT_COST){
						op_matrix[(m+1)*i+j] += INSERT_OP;
					}
				}

				// 현재 위치에서 대응하는 각 문자열의 문자가 서로 다른 경우
				// [S / I / D / T] 중에서 가능 (M은 불가능)
				else{

					// 이 때, 만약 Transpose가 가능하다면, [I / D / T] 중에서 가능
					if(str1[i-1] == str2[j-2] && str1[i-2] == str2[j-1]){

						// D[i][j] 가능한 최솟값으로 초기화
						D[i][j] = __GetMin3(D[i-1][j]+DELETE_COST, D[i][j-1]+INSERT_COST, D[i-2][j-2]+TRANSPOSE_COST);

						// 가능한 OP 탐색 후 op_matrix에 저장
						if(D[i][j] == D[i-1][j]+DELETE_COST){
							op_matrix[(m+1)*i+j] += DELETE_OP;
						}

						if(D[i][j] == D[i][j-1]+INSERT_COST){
							op_matrix[(m+1)*i+j] += INSERT_OP;
						}

						if(D[i][j] == D[i-2][j-2]+TRANSPOSE_COST){
							op_matrix[(m+1)*i+j] += TRANSPOSE_OP;
						}
					}

					// 이 때, 만약 Transpose가 불가능하다면, [S / I / D] 중에서 가능
					else{

						// D[i][j] 가능한 최솟값으로 초기화
						D[i][j] = __GetMin3(D[i-1][j-1]+SUBSTITUTE_COST, D[i-1][j]+DELETE_COST, D[i][j-1]+INSERT_COST);

						// 가능한 OP 탐색 후 op_matrix에 저장
						if(D[i][j] == D[i-1][j-1]+SUBSTITUTE_COST){
							op_matrix[(m+1)*i+j] += SUBSTITUTE_OP;
						}

						if(D[i][j] == D[i-1][j]+DELETE_COST){
							op_matrix[(m+1)*i+j] += DELETE_OP;
						}

						if(D[i][j] == D[i][j-1]+INSERT_COST){
							op_matrix[(m+1)*i+j] += INSERT_OP;
						}
					}
				}
			}

			// i, j 중 하나라도 2보다 작은 경우부터는 [(M / S) / I / D] 3가지 연산 가능 (T 불가능)
			else{

				// 현재 위치에서 대응하는 각 문자열의 문자가 서로 같은 경우
				// [M / I / D] 중에서 가능 (S 불가능)
				if(str1[i-1] == str2[j-1]){

					// D[i][j] 가능한 최솟값으로 초기화
					D[i][j] = __GetMin3(D[i-1][j-1], D[i-1][j]+DELETE_COST, D[i][j-1]+INSERT_COST);

					// 가능한 OP 탐색 후 op_matrix에 저장
					if(D[i][j] == D[i-1][j-1]){
						op_matrix[(m+1)*i+j] += MATCH_OP;
					}

					if(D[i][j] == D[i-1][j]+DELETE_COST){
						op_matrix[(m+1)*i+j] += DELETE_OP;
					}

					if(D[i][j] == D[i][j-1]+INSERT_COST){
						op_matrix[(m+1)*i+j] += INSERT_OP;
					}
				}

				// 현재 위치에서 대응하는 각 문자열의 문자가 서로 다른 경우
				// [S / I / D] 중에서 가능 (M 불가능)
				else{

					// D[i][j] 가능한 최솟값으로 초기화
					D[i][j] = __GetMin3(D[i-1][j-1]+SUBSTITUTE_COST, D[i-1][j]+DELETE_COST, D[i][j-1]+INSERT_COST);

					// 가능한 OP 탐색 후 op_matrix에 저장
					if(D[i][j] == D[i-1][j-1]+SUBSTITUTE_COST){
						op_matrix[(m+1)*i+j] += SUBSTITUTE_OP;
					}

					if(D[i][j] == D[i-1][j]+DELETE_COST){
						op_matrix[(m+1)*i+j] += DELETE_OP;
					}

					if(D[i][j] == D[i][j-1]+INSERT_COST){
						op_matrix[(m+1)*i+j] += INSERT_OP;
					}

				}
			}
		}
	}
		
	// op_matrix 출력
	print_matrix(op_matrix, str1, str2, n, m);

	// 이제 backtrace 할 차례
	backtrace(op_matrix, str1, str2, n, m);
	free(op_matrix);


	// 최소편집거리 반환
	int d = D[n][m];
	free(D);
	return d;
}

////////////////////////////////////////////////////////////////////////////////
// 세 정수 중에서 가장 작은 값을 리턴한다.
int __GetMin3( int a, int b, int c)
{
	int min = a;
	if (b < min)
		min = b;
	if(c < min)
		min = c;
	return min;
}

////////////////////////////////////////////////////////////////////////////////
// 네 정수 중에서 가장 작은 값을 리턴한다.
int __GetMin4( int a, int b, int c, int d)
{
	int min = __GetMin3( a, b, c);
	return (min > d) ? d : min;
}

////////////////////////////////////////////////////////////////////////////////
// 정렬된 문자쌍들을 출력
void print_alignment( char** align_str, int level)
{
	int i;
	
	for (i = level; i >= 0; i--)
	{
		printf( "%s\n", align_str[i]);
	}
}

////////////////////////////////////////////////////////////////////////////////
// backtrace_main을 호출하는 wrapper 함수
// str1 : 문자열 1
// str2 : 문자열 2
// n : 문자열 1의 길이
// m : 문자열 2의 길이
void backtrace(int *op_matrix, char *str1, char *str2, int n, int m)
{
	char** align_str = (char**)malloc((m+n) * sizeof(char*));
	assert(align_str != NULL);
	for(int i=0; i<m+n; i++){
		align_str[i] = (char*)malloc(8 * sizeof(char));
		assert(align_str[i] != NULL);
	}
	
	// path의 개수
	int path = 0;

	backtrace_main(op_matrix, str1, str2, n, m, 0, align_str, &path);
	free(align_str);	
}

////////////////////////////////////////////////////////////////////////////////
int main()
{
	char str1[30];
	char str2[30];
	
	int distance;
	
	fprintf( stderr, "INSERT_COST = %d\n", INSERT_COST);
	fprintf( stderr, "DELETE_COST = %d\n", DELETE_COST);
	fprintf( stderr, "SUBSTITUTE_COST = %d\n", SUBSTITUTE_COST);
	fprintf( stderr, "TRANSPOSE_COST = %d\n", TRANSPOSE_COST);
	
	while( fscanf( stdin, "%s\t%s", str1, str2) != EOF)
	{
		printf( "\n==============================\n");
		printf( "%s vs. %s\n", str1, str2);
		printf( "==============================\n");
		
		distance = min_editdistance(str1, str2);
		
		printf( "\nMinEdit(%s, %s) = %d\n", str1, str2, distance);
	}
	return 0;
}

