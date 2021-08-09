#include <stdlib.h>
#include <string.h>
#include <stdio.h>

//연산자 테이블에, 셀에 어떤 연산자로부터 내가 왔는지 저장할때 사용
//중복 가능하게 하려고 비트 단위로 16진수로 
#define INSERT_OP      0x01 
#define DELETE_OP      0x02 //10
#define SUBSTITUTE_OP  0x04 //100
#define MATCH_OP       0x08 //1000
#define TRANSPOSE_OP   0x10 //10000

#define INSERT_COST	1
#define DELETE_COST	1
#define SUBSTITUTE_COST	1
#define TRANSPOSE_COST	1

// 재귀적으로 연산자 행렬을 순회하며, 두 문자열이 최소편집거리를 갖는 모든 가능한 정렬(alignment) 결과를 출력한다.
// op_matrix : 이전 상태의 연산자 정보가 저장된 행렬 (1차원 배열임에 주의!)
// op[i][j]=op[i*col_size +j]
// col_size : op_matrix의 열의 크기
// str1 : 문자열 1
// str2 : 문자열 2
// n : 문자열 1의 길이
// m : 문자열 2의 길이
// level : 재귀호출의 레벨 (0, 1, 2, ...)
// align_str : 정렬된 문자쌍들의 정보가 저장된 문자열 배열 예) "a - a", "a - b", "* - b", "ab - ba" --> 8개(null문자 공백, - 포함)
// 편집 거리 계산 끝나고, 깊이 우선 탐색하는 함수
static void backtrace_main(int* op_matrix, int col_size, char* str1, char* str2, int n, int m, int level, char align_str[][8],int* casenum);

// 강의 자료의 형식대로 op_matrix를 출력 (좌하단(1,1) -> 우상단(n, m))
// i 의 시작부터 끝이 어딘지 잘 생각해서 출력
// 각 연산자를  괄호 안의 기호로 표시한다. 삽입(I), 삭제(D), 교체(S), 일치(M), 전위(T)
void print_matrix(int* op_matrix, int col_size, int n, int m);

// 두 문자열 str1과 str2의 최소편집거리를 계산한다.
// return value : 최소편집거리
// 이 함수 내부에서 print_matrix 함수와 backtrace 함수를 호출함
int min_editdistance(char* str1, char* str2);

////////////////////////////////////////////////////////////////////////////////
// 세 정수 중에서 가장 작은 값을 리턴한다. -->최소 편집 거리 구할때 사용
static int __GetMin3(int a, int b, int c)
{
	int min = a;
	if (b < min)
		min = b;
	if (c < min)
		min = c;
	return min;
}

////////////////////////////////////////////////////////////////////////////////
// 네 정수 중에서 가장 작은 값을 리턴한다.
static int __GetMin4(int a, int b, int c, int d)
{
	int min = __GetMin3(a, b, c);
	return (min > d) ? d : min;
}

////////////////////////////////////////////////////////////////////////////////
// 정렬된 문자쌍들을 출력
void print_alignment(char align_str[][8], int level)
{
	int i;

	for (i = level; i >= 0; i--)
	{
		printf("%s\n", align_str[i]);
	}
}

////////////////////////////////////////////////////////////////////////////////
// backtrace_main을 호출하는 wrapper 함수
// str1 : 문자열 1
// str2 : 문자열 2
// n : 문자열 1의 길이
// m : 문자열 2의 길이
void backtrace(int* op_matrix, int col_size, char* str1, char* str2, int n, int m)
{
	char align_str[n + m][8]; // n+m strings
	int casenumber=0;

	backtrace_main(op_matrix, col_size, str1, str2, n, m, 0, align_str,&casenumber);

}

////////////////////////////////////////////////////////////////////////////////
int main()
{
	char str1[30];
	char str2[30];

	int distance;

	fprintf(stderr, "INSERT_COST = %d\n", INSERT_COST);
	fprintf(stderr, "DELETE_COST = %d\n", DELETE_COST);
	fprintf(stderr, "SUBSTITUTE_COST = %d\n", SUBSTITUTE_COST);
	fprintf(stderr, "TRANSPOSE_COST = %d\n", TRANSPOSE_COST);

	while (fscanf(stdin, "%s\t%s", str1, str2) != EOF)
	{
		printf("\n==============================\n");
		printf("%s vs. %s\n", str1, str2);
		printf("==============================\n");

		distance = min_editdistance(str1, str2); //연산자 정보 찍고, align 정보 찍고, 최소 편집 거리 구하기

		printf( "\nMinEdit(%s, %s) = %d\n", str1, str2, distance);
	}
	return 0;
}

// 두 문자열 str1과 str2의 최소편집거리를 계산한다.
// return value : 최소편집거리
// 이 함수 내부에서 print_matrix 함수와 backtrace 함수를 호출함
int min_editdistance(char* str1, char* str2) {
	int n = strlen(str1); //인덱스 0부터 시작, i
	int m = strlen(str2); //j
	int i, j; //D[i][j] --> i column, j row
	int d[n + 1][m + 1]; //편집거리 저장하는(숫자들 저장하는) -->n+1, m+1인 이유는 실제로는 한줄씩 더 있어서, 인덱스 1부터
	int op_matrix[(n + 1) * (m + 1)]; //col_size =m+1 , 인덱스 1부터 
	int min_edit_d;
	// op[i][j]=op[i*col_size +j]


		//base condition
	for (int i = 0; i < n + 1; i++)
		d[i][0] = i;
	for (int j = 0; j < m + 1; j++)
		d[0][j] = j;

	//recurrence relation
	for (i = 1; i < n + 1; i++) {
		for (j = 1; j < m + 1; j++) {
			int frominsertcost, fromdeletecost, fromsubcost, fromtranscost = -123456;
			frominsertcost = d[i][j-1] + INSERT_COST;
			fromdeletecost = d[i-1][j] + DELETE_COST;
			if (str1[i - 1] == str2[j - 1])
				fromsubcost = d[i - 1][j - 1];
			else
				fromsubcost = d[i - 1][j - 1] + SUBSTITUTE_COST;

			if ((str1[i - 1] == str2[j - 2] && str1[i - 2] == str2[j - 1]) && (i > 1) && (j > 1)) {
				fromtranscost = d[i - 2][j - 2] + TRANSPOSE_COST;
				d[i][j] = __GetMin4(frominsertcost, fromdeletecost, fromsubcost, fromtranscost);
			}
			else
				d[i][j]=__GetMin3(frominsertcost, fromdeletecost, fromsubcost);
			op_matrix[i * (m + 1) + j] = 0;
			if (d[i][j] == frominsertcost)
				op_matrix[i * (m + 1) + j] |= INSERT_OP;
			if (d[i][j] == fromdeletecost)
				op_matrix[i * (m + 1) + j] |= DELETE_OP;
			if (d[i][j] == fromsubcost) {
				if (str1[i - 1] == str2[j - 1])
					op_matrix[i * (m + 1) + j] |= MATCH_OP;
				else
					op_matrix[i * (m + 1) + j] |= SUBSTITUTE_OP;
			}
			if (fromtranscost == d[i][j])
				op_matrix[i * (m + 1) + j] |= TRANSPOSE_OP;



		}
	}
	min_edit_d = d[n][m];
	print_matrix(op_matrix, m + 1, n, m);
	backtrace(op_matrix, m + 1, str1, str2, n, m);
	return min_edit_d;

}




// 재귀적으로 연산자 행렬을 순회하며, 두 문자열이 최소편집거리를 갖는 모든 가능한 정렬(alignment) 결과를 출력한다.
// op_matrix : 이전 상태의 연산자 정보가 저장된 행렬 (1차원 배열임에 주의!)
// op[i][j]=op[i*col_size +j]
// col_size : op_matrix의 열의 크기
// str1 : 문자열 1
// str2 : 문자열 2
// n : 문자열 1의 길이
// m : 문자열 2의 길이
// level : 재귀호출의 레벨 (0, 1, 2, ...)
// align_str : 정렬된 문자쌍들의 정보가 저장된 문자열 배열 예) "a - a", "a - b", "* - b", "ab - ba" --> 8개(null문자 공백, - 포함)
// 편집 거리 계산 끝나고, 깊이 우선 탐색하는 함수
static void backtrace_main(int* op_matrix, int col_size, char* str1, char* str2, int n, int m, int level, char align_str[][8],int* casenum) {

	//int casenum = 0;

	//level까지 이용해서 alin_Str 출력
	if (n == 0 || m == 0) {
		if (n == 0 && m == 0) {
			(*casenum) = (*casenum) + 1;
			printf("[%d] ==============================\n", *casenum);
			print_alignment(align_str, level-1);
		}
		else if (m==0) {
			//if (DELETE_OP & op_matrix[n * col_size + m]) {
				align_str[level][0] = str1[n - 1];
				align_str[level][1] = ' ';
				align_str[level][2] = '-';
				align_str[level][3] = ' ';
				align_str[level][4] = '*';
				align_str[level][5] = '\0';
				backtrace_main(op_matrix, col_size, str1, str2, n - 1, m, level + 1, align_str,casenum);

			//}
		}
		else if(n==0) {
			//if (INSERT_OP & op_matrix[n * col_size + m]) {
				align_str[level][0] = '*';
				align_str[level][1] = ' ';
				align_str[level][2] = '-';
				align_str[level][3] = ' ';
				align_str[level][4] = str2[m - 1];
				align_str[level][5] = '\0';
				backtrace_main(op_matrix, col_size, str1, str2, n, m-1 , level + 1, align_str, casenum);

			//}
		}
			
		return;
	}

	//교체
	if (SUBSTITUTE_OP & op_matrix[n * col_size + m]) {
		align_str[level][0] = str1[n - 1];
		align_str[level][1] = ' ';
		align_str[level][2] = '-';
		align_str[level][3] = ' ';
		align_str[level][4] = str2[m - 1];
		align_str[level][5] = '\0';
		backtrace_main(op_matrix, col_size, str1, str2, n - 1, m - 1, level + 1, align_str, casenum);

	}
	//일치
	else if (MATCH_OP & op_matrix[n * col_size + m]) {
		align_str[level][0] = str1[n - 1];
		align_str[level][1] = ' ';
		align_str[level][2] = '-';
		align_str[level][3] = ' ';
		align_str[level][4] = str2[m - 1];
		align_str[level][5] = '\0';
		backtrace_main(op_matrix, col_size, str1, str2, n - 1, m - 1, level + 1, align_str, casenum);

	}
	//삽입
	if (INSERT_OP & op_matrix[n * col_size + m]) {
		align_str[level][0] = '*';
		align_str[level][1] = ' ';
		align_str[level][2] = '-';
		align_str[level][3] = ' ';
		align_str[level][4] = str2[m - 1];
		align_str[level][5] = '\0';
		backtrace_main(op_matrix, col_size, str1, str2, n, m - 1, level + 1, align_str, casenum);

	}
	//삭제
	if (DELETE_OP & op_matrix[n * col_size + m]) {
		align_str[level][0] = str1[n - 1];
		align_str[level][1] = ' ';
		align_str[level][2] = '-';
		align_str[level][3] = ' ';
		align_str[level][4] = '*';
		align_str[level][5] = '\0';
		backtrace_main(op_matrix, col_size, str1, str2, n - 1, m, level + 1, align_str, casenum);

	}
	//전위
	if (TRANSPOSE_OP & op_matrix[n * col_size + m]) {
		align_str[level][0] = str1[n - 2];
		align_str[level][1] = str1[n - 1];
		align_str[level][2] = ' ';
		align_str[level][3] = '-';
		align_str[level][4] = ' ';
		align_str[level][5] = str2[m - 2];
		align_str[level][6] = str2[m - 1];
		align_str[level][7] = '\0';
		backtrace_main(op_matrix, col_size, str1, str2, n - 2, m - 2, level + 1, align_str, casenum);

	}

}

// 강의 자료의 형식대로 op_matrix를 출력 (좌하단(1,1) -> 우상단(n, m))
// i 의 시작부터 끝이 어딘지 잘 생각해서 출력
// 각 연산자를  괄호 안의 기호로 표시한다. 삽입(I), 삭제(D), 교체(S), 일치(M), 전위(T)

void print_matrix(int* op_matrix, int col_size, int n, int m) {
	for (int i = n; i >= 1; i--) {
		for (int j = 1; j <= m; j++) {
			int op = op_matrix[i * col_size + j];
			if (MATCH_OP & op)
				printf("M");
			if (SUBSTITUTE_OP &op)
				printf("S");
			if (INSERT_OP & op)
				printf("I");
			if (DELETE_OP & op)
				printf("D");
			if (TRANSPOSE_OP & op)
				printf("T");
			printf("\t");
		}
		printf("\n");
	}
	
}
	




