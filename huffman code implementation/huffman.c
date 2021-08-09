//#define BINARY_MODE

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

typedef struct Node 
{ 
	unsigned char	data;	// 문자	(알파벳)
	int		freq; 			// 빈도
	struct	Node *left;		// 왼쪽 서브트리 포인터
	struct	Node *right; 	// 오른쪽 서브트리 포인터
} tNode;

////////////////////////////////////////////////////////////////////////////////
typedef struct
{
	int	last;		// 힙에 저장된 마지막 element의 index
	int	capacity;	// heapArr의 크기
	tNode **heapArr;
} HEAP;

// 힙 생성
// 배열을 위한 메모리 할당 (capacity)
// last = -1 
//-->힙에 저장된 마지막 element의 인덱스 알려주는 것. 비어있는 경우 last==-1, element 하나 들어있으면 last==0
//하나씩 채워질때마다 last값 증가
HEAP *heapCreate( int capacity); //heapcreate(100) 이면 100개짜리 element 저장하는 빈 힙 만들기

// 최소힙 유지
static void _reheapUp( HEAP *heap, int index);

// 힙에 원소 삽입
// _reheapUp 함수 호출
int heapInsert( HEAP *heap, tNode *data);

// 최소힙 유지
static void _reheapDown( HEAP *heap, int index);

// 최소값 제거
// _reheapDown 함수 호출
tNode *heapDelete( HEAP *heap);

// 힙 메모리 해제
void heapDestroy( HEAP *heap);

////////////////////////////////////////////////////////////////////////////////
// 파일에 속한 각 문자(바이트)의 빈도 저장
// return value : 파일에서 읽은 바이트 수
int read_chars( FILE *fp, int *ch_freq);

// 허프만 코드에 대한 메모리 해제
void free_huffman_code( char *codes[]);

// 새로운 노드를 생성
// 좌/우 subtree가 NULL이고 문자(data)와 빈도(freq)가 저장됨
// return value : 노드의 포인터
tNode *newNode(char data, int freq);

// 허프만 트리를 생성
// 1. capacity 256 짜리 빈(empty) 힙 생성
// 2. 개별 알파벳에 대한 노드 생성
// 3. 힙에 삽입 (minheap 구성)
// 4. 2개의 최소값을 갖는 트리 추출
// 5. 두 트리를 결합 후 새 노드에 추가
// 6. 새 트리를 힙에 삽입
// 7. 힙에 한개의 노드가 남을 때까지 반복
// return value: 트리의 root 노드의 포인터
tNode *make_huffman_tree( int *ch_freq);

// 허프만 트리를 순회하며 허프만 코드를 생성하여 codes에 저장
// leaf 노드에서만 코드를 생성
// strdup 함수 사용함
void traverse_tree( tNode *root, char *code, int depth, char *codes[]);

// 허프만 트리로부터 허프만 코드를 생성
// traverse_tree 함수 호출
void make_huffman_code( tNode *root, char *codes[]);

// 트리 메모리 해제
void destroyTree( tNode *root);

// 텍스트 파일을 허프만 코드를 이용하여 바이너리 파일로 인코딩
// return value : 인코딩된 파일의 바이트 수
int encoding( char *codes[], FILE *infp, FILE *outfp);
int encoding_binary( char *codes[], FILE *infp, FILE *outfp);

// 바이너리 파일을 허프만 트리를 이용하여 텍스트 파일로 디코딩
void decoding( tNode *root, FILE *infp, FILE *outfp);
void decoding_binary( tNode *root, FILE *infp, FILE *outfp);

////////////////////////////////////////////////////////////////////////////////
// 문자별 빈도 출력 (for debugging)
void print_char_freq( int *ch_freq)
{
	int i;

	for (i = 0; i < 256; i++)
	{
		printf( "%d\t%d\n", i, ch_freq[i]); // 문자인덱스, 빈도
	}
}

////////////////////////////////////////////////////////////////////////////////
// 문자별 빈도를 이용하여 허프만 트리와 허프만 코드를 생성
// return value : 허프만 트리의 root node
tNode *run_huffman( int *ch_freq, char *codes[])
{
	tNode *root;
	root = make_huffman_tree( ch_freq);
	
	make_huffman_code( root, codes);
	
	return root;
}

////////////////////////////////////////////////////////////////////////////////
// 허프만 코드를 화면에 출력
void print_huffman_code( char *codes[])
{
	int i;
	
	for (i = 0; i < 256; i++)
	{
		printf( "%d\t%s\n", i, codes[i]);
	}
	//fflush(stdout); 
	//printf("out of for mun");
}

////////////////////////////////////////////////////////////////////////////////
// argv[1] : 입력 텍스트 파일
// argv[2] : 바이너리 코드 (encoding 결과) 
// argv[3] : 출력 텍스트 파일 (decoding 결과)
int main( int argc, char **argv)
{
	FILE *fp;
	FILE *infp, *outfp;
	int ch_freq[256] = {0,}; // 문자별 빈도
	char *codes[256]; // 문자별 허프만 코드 (ragged 배열)
	tNode *huffman_tree; // 허프만 트리
	
	if (argc != 4)
	{
		fprintf( stderr, "%s input-file encoded-file decoded-file\n", argv[0]);
		return 1;
	}

	////////////////////////////////////////
	// 입력 텍스트 파일
	fp = fopen( argv[1], "rt");
	if (fp == NULL)
	{
		fprintf( stderr, "Error: cannot open file [%s]\n", argv[1]);
		return 1;
	}

	// 텍스트 파일로부터 문자별 빈도 저장
	int num_bytes = read_chars( fp, ch_freq); //ok

	fclose( fp);

	// 문자별 빈도 출력 (for debugging)
	// 나중에 코멘트 처리해서 제출
	print_char_freq( ch_freq); //ok
	
	// 허프만 코드/트리 생성
	// codes에 허프만 코드의 결과가 저장됨
	// huffman_tree는 루트를 가르키는 노드의 포인터
	huffman_tree = run_huffman( ch_freq, codes);//ok
	
	// 허프만 코드 출력 (stdout)
	print_huffman_code(codes); //ok
	

	////////////////////////////////////////
	// 입력: 텍스트 파일
	infp = fopen( argv[1], "rt");
	
#ifdef BINARY_MODE
	// 출력: 바이너리 코드
	outfp = fopen( argv[2], "wb");
#else
	// 출력: 바이너리 코드
	outfp = fopen( argv[2], "wt");
#endif

	// 허프만코드를 이용하여 인코딩(압축)
#ifdef BINARY_MODE
	int encoded_bytes = encoding_binary( codes, infp, outfp);
#else
	int encoded_bytes = encoding( codes, infp, outfp);
#endif

	// 허프만 코드 메모리 해제
	free_huffman_code(codes);
	
	fclose( infp);
	fclose( outfp);

	////////////////////////////////////////
	// 입력: 바이너리 코드
#ifdef BINARY_MODE
	infp = fopen( argv[2], "rb");
#else
	infp = fopen( argv[2], "rt");
#endif

	// 출력: 텍스트 파일
	outfp = fopen( argv[3], "wt");

	// 허프만 트리를 이용하여 디코딩
#ifdef BINARY_MODE
	decoding_binary( huffman_tree, infp, outfp);
#else
	decoding( huffman_tree, infp, outfp);
#endif

	// 허프만 트리 메모리 해제
	destroyTree( huffman_tree);

	fclose( infp);
	fclose( outfp);

	////////////////////////////////////////
	printf( "# of bytes of the original text = %d\n", num_bytes);
	printf( "# of bytes of the compressed text = %d\n", encoded_bytes);
	printf( "compression ratio = %.2f\n", ((float)num_bytes - encoded_bytes) / num_bytes * 100);
	
	return 0;
}

////////////////////////////////////////////////////////////////////////////////
// 힙의 내용 출력 (for debugging)
void heapPrint( HEAP *heap)
{
	int i;
	tNode **p = heap->heapArr;
	int last = heap->last;
	
	for( i = 0; i <= last; i++)
	{
		printf("[%d]%c(%6d)\n", i, p[i]->data, p[i]->freq);
	}
	printf( "\n");
}

////////////////////////////////////////////////////////////////////////////////
// 힙 생성
// 배열을 위한 메모리 할당 (capacity)
// last = -1
HEAP *heapCreate( int capacity)
{
	HEAP *heap;
	
	heap = (HEAP *)malloc( sizeof(HEAP));
	if (!heap) return NULL;

	heap->last = -1;
	heap->capacity = capacity;
	heap->heapArr = (tNode **)malloc( sizeof(tNode *) * capacity);
	if (heap->heapArr == NULL)
	{
		fprintf( stderr, "Error : not enough memory!\n");
		free( heap);
		return NULL;
	}
	return heap;
}

////////////////////////////////////////////////////////////////////////////////
// 최소힙 유지
static void _reheapUp( HEAP *heap, int index)
{
	tNode **arr = heap->heapArr;
	int parent;
	
	while(1)
	{
		if (index == 0) return; // root node
		
		parent = (index - 1) / 2;
		
		if (arr[index]->freq < arr[parent]->freq) // exchange (for minheap)
		{
			tNode *temp = arr[index];
			arr[index] = arr[parent];
			arr[parent] = temp;
			
			index = parent;
		}
		else return;
	}
}

////////////////////////////////////////////////////////////////////////////////
// 힙에 원소 삽입
// _reheapUp 함수 호출
int heapInsert( HEAP *heap, tNode *data)
{
	if (heap->last == heap->capacity - 1)
		return 0;
	
	(heap->last)++;
	(heap->heapArr)[heap->last] = data;
	
	_reheapUp( heap, heap->last);
	
	return 1;
}

////////////////////////////////////////////////////////////////////////////////
// 최소힙 유지
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
		
		if (noright || leftData->freq < rightData->freq) largeindex = index * 2 + 1; // left child
		else largeindex = index * 2 + 2; // right child
		
		if (arr[index]->freq > arr[largeindex]->freq) // exchange (for minheap)
		{
			tNode *temp = arr[index];
			arr[index] = arr[largeindex];
			arr[largeindex] = temp;
			
			index = largeindex;
			
			noright = 0;
		}
		else return;
	}
}

////////////////////////////////////////////////////////////////////////////////
// 최소값 제거
// _reheapDown 함수 호출
tNode *heapDelete( HEAP *heap)
{
	if (heap->last == -1) return NULL; // empty heap
	
	tNode *data = heap->heapArr[0];
	heap->heapArr[0] = heap->heapArr[heap->last];
	
	(heap->last)--;
	
	_reheapDown( heap, 0);
	
	return data;
}

////////////////////////////////////////////////////////////////////////////////
// 힙 메모리 해제
void heapDestroy( HEAP *heap)
{
	free(heap->heapArr);
	free(heap);
}

// 허프만 트리를 생성
// 1. capacity 256 짜리 빈(empty) 힙 생성: createheap
// 2. 개별 알파벳에 대한 노드 생성 :빈노드, 한바이트짜리로 표현되는 모든 문자에 대한 것, 256개 노드 만들어짐
// 3. 힙에 삽입 (minheap 구성) :노드들을 heap에 삽입, 힙에 넣으면 알아서 빈도값이 제일 작은 노드가 루트에 올라옴.
// 4. 2개의 최소값을 갖는 트리 추출 :힙에서 두개를 꺼냄. 두번 꺼내면 됨.
// 5. 두 트리를 결합 후 새 노드에 추가 :새 노드의 데이터값은 의미 없는 값으로 초기화
// 6. 새 트리를 힙에 삽입 :루트 노드만 삽입하면 됨. 나머지는 저절로 들어감.
// 7. 힙에 한개의 노드가 남을 때까지 반복 :4번부터 6번을 반복, 마지막에 남은 하나를 꺼내면 걔가 허프만 트리가 됨.
// return value: 트리의 root 노드의 포인터
//ch_freq: 문자 빈도 정보가 들어있는 이걸 받아서 트리를 만드는 것.
tNode *make_huffman_tree( int *ch_freq){

	HEAP *theap; 
	theap=heapCreate(256);

	for(int i=0;i<256;i++){
		tNode *node;
		node=newNode((char)i, ch_freq[i]);
		heapInsert( theap, node);
	}
	
	while(theap->last!=0){
		tNode *small_1=heapDelete(theap);
		tNode *small_2=heapDelete(theap);
		int sum_of_freq=small_1->freq + small_2->freq;
		tNode *sum_node_of_two;
		sum_node_of_two=newNode('t', sum_of_freq);
		sum_node_of_two->left=small_1;
		sum_node_of_two->right=small_2;
		heapInsert( theap, sum_node_of_two);
	}
	
	tNode *last_node;
	last_node=heapDelete(theap);
	heapDestroy(theap);
	
	return last_node;
}

// 새로운 노드를 생성
// 좌/우 subtree가 NULL이고 문자(data)와 빈도(freq)가 저장됨
// 포인터는 null로 설정
// return value : 노드의 포인터
tNode *newNode(char data, int freq){
	tNode* pNODE = (tNode*)malloc(sizeof(tNode));

	if (pNODE == NULL)
		return NULL;

	pNODE->left = NULL;
	pNODE->right = NULL;
	pNODE->data = data;
	pNODE->freq=freq;
	
	return pNODE;
}

// 텍스트 파일을 허프만 코드를 이용하여 바이너리 파일로 인코딩
// return value : 인코딩된 파일의 바이트 수(비트 수 /8)
int encoding( char *codes[], FILE *infp, FILE *outfp){
	
	int fromtext_one_ch;
	int num_of_byte=0;
	while(1){
		fromtext_one_ch=fgetc(infp);
		if(fromtext_one_ch==EOF)
			break;
		fputs(codes[fromtext_one_ch],outfp);
		num_of_byte=num_of_byte+strlen(codes[fromtext_one_ch]);
		}
	
	
	return num_of_byte/8;
}

// 허프만 트리로부터 허프만 코드를 생성
// traverse_tree 함수 호출
// 왼쪽 포인터 오른쪽 포인터가 둘다 null이면 리프
// 전위 순회
void make_huffman_code( tNode *root, char *codes[]){
	char saving_traverse_code[256]={'0',};
	
	traverse_tree(root, saving_traverse_code, 0, codes);
	
	return;
}

// 허프만 트리를 순회하며 허프만 코드를 생성하여 codes에 저장
// leaf 노드에서만 코드를 생성
// strdup 함수 사용함
// 왼쪽으로 갈 때는 0을 저장, 오른쪽으로 갈 때는 1을 저장 --> code(0또는 1), depth
void traverse_tree( tNode *root, char *code, int depth, char *codes[]){
	//리프노드이면 code를 codes에 저장
	if((root->left ==NULL) && (root->right ==NULL)){
		if(depth<256){
			code[depth]='\0';
			codes[(int)root->data]=strdup(code);
			return;
		}
		
	}
	
	//왼쪽으로 재귀 호출
	code[depth]='0';
	traverse_tree(root->left,code,depth+1,codes);
	
	//오른쪽으로 재귀 호출
	code[depth]='1';
	traverse_tree(root->right,code,depth+1,codes);
	
	
}

// 허프만 코드에 대한 메모리 해제
void free_huffman_code( char *codes[]){
	for(int i=0;i<256;i++){
		free(codes[i]);
	}

	
}

// 바이너리 파일을 허프만 트리를 이용하여 텍스트 파일로 디코딩
// 인코딩된 바이너리 파일을
// FILE *infp 인코딩된 입력파일
// FILE *outfp 출력할 텍스트 파일
void decoding( tNode *root, FILE *infp, FILE *outfp){
	char frombinary_one_bit;
	tNode *pNODE=root;
	
	while(!feof(infp)){
		frombinary_one_bit=fgetc(infp);
		if(frombinary_one_bit=='0')
			pNODE=pNODE->left;
		else 
			pNODE=pNODE->right;
		if((pNODE->left==NULL)&&(pNODE->right==NULL)){
			fputc((int)pNODE->data,outfp);
			pNODE=root;
		}	
	}	
}



// 트리 메모리 해제
// 재귀적으로 트리 순회하며 노드 삭제
// root->left가 null????????????????????????????????????????????
void destroyTree( tNode *root){
	if(root!=NULL)
	{
		destroyTree(root->left);
		destroyTree(root->right);
		free(root);
	}
}
	
	
	

// 파일에 속한 각 문자(바이트)의 빈도 저장
// return value : 파일에서 읽은 바이트 수
//int *ch_freq : int 배열 --> 크기가 256(아스키코드 문자 개수)?
int read_chars( FILE *fp, int *ch_freq){
	unsigned char now_ch;
	int total_byte=0;
	while(fscanf(fp,"%c",&now_ch)!=EOF){
		total_byte++;
		ch_freq[now_ch]+=1;
	}
	
	return total_byte;
	
}

