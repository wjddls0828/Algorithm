#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INSERT_OP      0x01 
#define DELETE_OP      0x02 //10
#define SUBSTITUTE_OP  0x04 //100
#define MATCH_OP       0x08 //1000
#define TRANSPOSE_OP   0x10 //10000

#define INSERT_COST	1
#define DELETE_COST	1
#define SUBSTITUTE_COST	1
#define TRANSPOSE_COST	1

typedef struct Node 
{ 
	char	word[100];	// 단어
	int	editdistance;	// 편집거리
	int freq;
	struct	Node *left;		// 왼쪽 서브트리 포인터
	struct	Node *right; 	// 오른쪽 서브트리 포인터
} tNode;


typedef struct
{
	int	last;		// 힙에 저장된 마지막 element의 index
	int	capacity;	// heapArr의 크기
	tNode **heapArr;
} HEAP;

int min_editdistance(char* str1, char* str2);
int min_editdistance(char* str1, char* str2);
static int __GetMin3(int a, int b, int c);
static int __GetMin4(int a, int b, int c, int d);
int compare(const void* itemptr1, const void* itemptr2);
tNode *newNode(char* data, int distance);
HEAP *heapCreate( int capacity);
static void _reheapUp( HEAP *heap, int index);
int heapInsert( HEAP *heap, tNode *data);
static void _reheapDown( HEAP *heap, int index);
tNode *heapDelete( HEAP *heap);
void heapDestroy( HEAP *heap);
void destroyTree( tNode *root);

int main(){
	
	char user_input_word[100];
	int length_of_user_input;
	printf("단어를 입력하세요\n");
	scanf("%s", user_input_word);
	length_of_user_input = strlen(user_input_word);

	//2gram 배열 만들기
	char grams_arr[100][3];
	for (int i = 0; i < length_of_user_input - 1; i++) {
		grams_arr[i][0] = user_input_word[i];
		grams_arr[i][1] = user_input_word[i + 1];
		grams_arr[i][2] = '\0';
	}
	
	//txt 파일 찾기
	char text_txt[] = ".txt";
	char filename[10];
	
	HEAP *theap; 
	theap=heapCreate(3001);
    tNode* deletenode;
	
	for (int j = 0; j < length_of_user_input - 1; j++) {
		//파일명 만들기
		filename[0] = grams_arr[j][0];
		filename[1] = grams_arr[j][1];
		filename[2] = '\0';
		strcat(filename, text_txt);

		//파일 열기
		FILE* infp;
		infp = fopen(filename, "rt");
		printf("%s파일을 여는 중입니다.\n",filename);
		
		while (!feof(infp)) {
			//다음 파일에서 단어 하나 읽기
			char read_one_word[100];
			fscanf(infp, "%s", read_one_word);

			int inOK=1;
			for(int w=0;w<=theap->last;w++)
				if(strcmp(read_one_word,theap->heapArr[w]->word)==0){ //같은 단어이면
					inOK=0;
					theap->heapArr[w]->freq+=1; //빈도수 증가
				}
					
			if(inOK==0)
				continue;
				
			//편집 거리 계산
			int distance = min_editdistance(user_input_word,read_one_word);
			
			//단어를 노드로 만들기
			tNode *node;
		    node=newNode(read_one_word,-distance);
			
			//노드를 힙에 넣기
			heapInsert(theap, node);
			
			//힙의 개수가 3000개 이상이면 heap delete
			if(theap->last >=3000){
				deletenode=heapDelete(theap);
				free(deletenode);
			}
		}
		fclose(infp);
	}
	//heapPrint->arr->qsort->select 10 
	qsort(theap->heapArr, 3000, sizeof(tNode*), compare);
	for(int z=0;z<10;z++){
		printf("유사 단어: %s 편집거리: %d\n",theap->heapArr[z]->word,-theap->heapArr[z]->editdistance);
	}
	for(int y=0;y<3000;y++){
		free(theap->heapArr[y]);
	}
	heapDestroy(theap);

	return 0;
	
}
	
int compare(const void* itemptr1, const void* itemptr2) {
	tNode* item1 = *(tNode**)itemptr1;
	tNode* item2 = *(tNode**)itemptr2;

	if(item1->editdistance > item2->editdistance)
		return -1;
	else if(item1->editdistance < item2->editdistance)
		return 1;
	else{
		if(item1->freq > item2->freq)
			return  1;
		else if(item1->freq < item2->freq)
			return -1;
		
	}
	return 0;
}	


int min_editdistance(char* str1, char* str2) {
	int n = strlen(str1); //인덱스 0부터 시작, i
	int m = strlen(str2); //j
	int d[100][100]; //편집거리 저장하는(숫자들 저장하는) -->n+1, m+1인 이유는 실제로는 한줄씩 더 있어서, 인덱스 1부터
	int min_edit_d;
	
	//base condition
	for (int i = 0; i < n + 1; i++)
		d[i][0] = i;
	for (int j = 0; j < m + 1; j++)
		d[0][j] = j;

	//recurrence relation
	for (int i = 1; i < n + 1; i++) {
		for (int j = 1; j < m + 1; j++) {
			int frominsertcost= -123456;
			int fromdeletecost= -123456;
			int fromsubcost= -123456;
            int	fromtranscost = -123456;
			frominsertcost = d[i][j - 1] + INSERT_COST;
			fromdeletecost = d[i - 1][j] + DELETE_COST;
			if (str1[i - 1] == str2[j - 1])
				fromsubcost = d[i - 1][j - 1];
			else
				fromsubcost = d[i - 1][j - 1] + SUBSTITUTE_COST;
			
			if ((i > 1) && (j > 1)&&(str1[i - 1] == str2[j - 2] && str1[i - 2] == str2[j - 1])) {
				fromtranscost = d[i - 2][j - 2] + TRANSPOSE_COST;
				d[i][j] = __GetMin4(frominsertcost, fromdeletecost, fromsubcost, fromtranscost);
			}
			
			
			else
				d[i][j] = __GetMin3(frominsertcost, fromdeletecost, fromsubcost);
		}
	}
	min_edit_d = d[n][m];
	return min_edit_d;

}

static int __GetMin3(int a, int b, int c)
{
	int min = a;
	if (b < min)
		min = b;
	if (c < min)
		min = c;
	return min;
}


static int __GetMin4(int a, int b, int c, int d)
{
	int min = __GetMin3(a, b, c);
	return (min > d) ? d : min;
}

tNode *newNode(char* data, int distance){
	
	tNode* pNODE = (tNode*)malloc(sizeof(tNode));

	if (pNODE == NULL)
		return NULL;

	pNODE->left = NULL;
	pNODE->right = NULL;
	strcpy(pNODE-> word, data);
	pNODE->editdistance=distance;
	pNODE->freq=0;
	
	return pNODE;
}


HEAP *heapCreate( int capacity)
{
	HEAP *heap;
	
	heap = (HEAP *)malloc( sizeof(HEAP));
	if (!heap)
		return NULL;

	heap->last  = -1;
	heap->capacity = capacity;
	heap->heapArr = (tNode **)malloc( sizeof(tNode *) * capacity);
	if (heap->heapArr == NULL)
	{
		fprintf( stderr, "memory out\n");
		free( heap);
		return NULL;
	}
	return heap;
}

static void _reheapUp( HEAP *heap, int index)
{
	tNode **arr = heap->heapArr;
	int parent;
	
	while(1)
	{
		if (index == 0) return; // root node
		
		parent = (index - 1) / 2;
		
		if (arr[index]->editdistance < arr[parent]->editdistance) // exchange (for minheap) Also consider freq
		{
			tNode *temp = arr[index];
			arr[index] = arr[parent];
			arr[parent] = temp;
			
			index = parent;
		}
		else if(arr[index]->editdistance == arr[parent]->editdistance){
			if (arr[index]->freq < arr[parent]->freq) // exchange (for minheap) Also consider freq
			{
				tNode *temp = arr[index];
				arr[index] = arr[parent];
				arr[parent] = temp;
				
				index = parent;
			}
			return;
		}
		else return;
	}
}


int heapInsert( HEAP *heap, tNode *data)
{
	if (heap->last == heap->capacity - 1)
		return 0;
	
	(heap->last)++;
	(heap->heapArr)[heap->last] = data;
	
	_reheapUp( heap, heap->last);
	
	return 1;
}


static void _reheapDown( HEAP *heap, int index)
{
	tNode **arr = heap->heapArr;
	tNode *leftData;
	tNode *rightData;
	int noright = 0;
	int largeindex; // index of left or right child with large key
	
	while(1)
	{
		if ((index * 2 + 1) > heap->last) return; // leaf node (there is no left subtree)
		
		leftData = arr[index * 2 + 1];
		if (index * 2 + 2 <= heap->last) rightData = arr[index * 2 + 2];
		else noright = 1;
		
		if (noright || leftData->editdistance < rightData->editdistance) largeindex = index * 2 + 1; // left child
		else largeindex = index * 2 + 2; // right child
		
		if (arr[index]->editdistance > arr[largeindex]->editdistance) // exchange (for minheap)//Also consider freq
		{
			tNode *temp = arr[index];
			arr[index] = arr[largeindex];
			arr[largeindex] = temp;
			
			index = largeindex;
			
			noright = 0;
		}
		else if(arr[index]->editdistance == arr[largeindex]->editdistance){
			if (arr[index]->freq > arr[largeindex]->freq) // exchange (for minheap)//Also consider freq
			{
				tNode *temp = arr[index];
				arr[index] = arr[largeindex];
				arr[largeindex] = temp;
				
				index = largeindex;
				
				noright = 0;
			}
			return;
		}
		else return;
	}
}


tNode *heapDelete( HEAP *heap)
{
	if (heap->last == -1) return NULL; // empty heap
	
	tNode *data = heap->heapArr[0];
	heap->heapArr[0] = heap->heapArr[heap->last];
	
	(heap->last)--;
	
	_reheapDown( heap, 0);
	
	return data;
}


void heapDestroy( HEAP *heap)
{
	free(heap->heapArr);
	free(heap);
}

/*void destroyTree( tNode *root){
	if(root!=NULL)
	{
		destroyTree(root->left);
		destroyTree(root->right);
		free(root);
	}
}*/