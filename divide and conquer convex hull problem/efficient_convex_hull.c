#include <stdlib.h> // atoi, rand, qsort, malloc
#include <stdio.h>
#include <assert.h> // assert
#include <time.h> //time
#include <math.h>

#define RANGE 10000

typedef struct
{
	int x;
	int y;
} t_point;

typedef struct
{
	t_point from;
	t_point to;
} t_line;

////////////////////////////////////////////////////////////////////////////////

t_line* upper_hull(t_point* points, int num_point, t_point p1, t_point pn, t_line* lines, int* num_line, int* capacity);


float distance(float a, float b, float c, t_point p);


void separate_points(t_point* points, int num_point, t_point from, t_point to, t_point* s1, t_point* s2, int* n1, int* n2);

////////////////////////////////////////////////////////////////////////////////
void print_header(char* filename)
{
	printf("#! /usr/bin/env Rscript\n");
	printf("png(\"%s\", width=700, height=700)\n", filename);

	printf("plot(1:%d, 1:%d, type=\"n\")\n", RANGE, RANGE);
}

////////////////////////////////////////////////////////////////////////////////
void print_footer(void)
{
	printf("dev.off()\n");
}

////////////////////////////////////////////////////////////////////////////////
// qsort를 위한 비교 함수
int cmp_x(const void* p1, const void* p2)
{
	t_point* p = (t_point*)p1;
	t_point* q = (t_point*)p2;

	float diff = p->x - q->x;

	return ((diff >= 0.0) ? ((diff > 0.0) ? +1 : 0) : -1);
}

////////////////////////////////////////////////////////////////////////////////
void print_points(t_point* points, int num_point)
{
	int i;
	printf("\n#points\n");

	for (i = 0; i < num_point; i++)
		printf("points(%d,%d)\n", points[i].x, points[i].y);
}

////////////////////////////////////////////////////////////////////////////////
void print_line_segments(t_line* lines, int num_line)
{
	int i;

	printf("\n#line segments\n");

	for (i = 0; i < num_line; i++)
		printf("segments(%d,%d,%d,%d)\n", lines[i].from.x, lines[i].from.y, lines[i].to.x, lines[i].to.y);
}

////////////////////////////////////////////////////////////////////////////////
// [input] points : set of points
// [input] num_point : number of points
// [output] num_line : number of lines
// return value : pointer of set of line segments that forms the convex hull
t_line* convex_hull(t_point* points, int num_point, int* num_line)
{
	int capacity = 10; //라인을 저장할 수 있는 공간을 10개로 할당

	t_line* lines = (t_line*)malloc(capacity * sizeof(t_line));
	*num_line = 0; //현재까지 저장된 선은 없으니까


	// s1: set of points
	t_point* s1 = (t_point*)malloc(sizeof(t_point) * num_point);
	assert(s1 != NULL);

	// s2: set of points
	t_point* s2 = (t_point*)malloc(sizeof(t_point) * num_point);
	assert(s2 != NULL);

	int n1, n2; // number of points in s1, s2, respectively

	// x 좌표에 따라 정렬된 점들의 집합이 입력된 경우
	// points[0] : leftmost point (p1)
	// points[num_point-1] : rightmost point (pn)

	// 점들을 분리
	separate_points(points, num_point, points[0], points[num_point - 1], s1, s2, &n1, &n2);

	// upper hull을 구한다.
	lines = upper_hull(s1, n1, points[0], points[num_point - 1], lines, num_line, &capacity);
	lines = upper_hull(s2, n2, points[num_point - 1], points[0], lines, num_line, &capacity);

	free(s1);
	free(s2);

	return lines;
}

////////////////////////////////////////////////////////////////////////////////
int main(int argc, char** argv)
{
	float x, y;
	int num_point; // number of points

	if (argc != 2)
	{
		printf("%s number_of_points\n", argv[0]);
		return 0;
	}

	num_point = atoi(argv[1]);
	if (num_point <= 0)
	{
		printf("The number of points should be a positive integer!\n");
		return 0;
	}

	t_point* points; //점들을 위한 구조체 선언
	points = (t_point*)malloc(sizeof(t_point) * num_point); //numpoint만큼 메모리 할당
	assert(points != NULL);

	// making points
	srand(time(NULL));
	for (int i = 0; i < num_point; i++)
	{
		x = rand() % RANGE + 1; // 1 ~ RANGE random number
		y = rand() % RANGE + 1;

		points[i].x = x;
		points[i].y = y;
	}

	fprintf(stderr, "%d points created!\n", num_point);

	// sort the points by their x coordinate
	qsort(points, num_point, sizeof(t_point), cmp_x); //점 정렬

	print_header("convex.png");

	print_points(points, num_point);

	// convex hull algorithm
	int num_line;
	t_line* lines = convex_hull(points, num_point, &num_line);

	fprintf(stderr, "%d lines created!\n", num_line);

	print_line_segments(lines, num_line);

	print_footer();

	free(points);
	free(lines);

	return 0;
}

// t_point *points 점의 집합, int num_point 점의 개수
// 직선이 있으면, upper에 속하는 점들을 s1에 저장하고 lower에 속하는 점들을 s2에 저장
// s1과 s2를 위한 메모리를 잡은 후 거기에 일부는 s1에 보내고 일부는 s2에 보내기
// 두 점(from, to)을 연결하는 직선(ax + by - c = 0)으로 n개의 점들의 집합 s(점의 수 num_point)를 s1(점의 수 n1)과 s2(점의 수 n2)로 분리하는 함수
// [output] s1 : 직선의 upper(left)에 속한 점들의 집합 (ax+by-c < 0)
// [output] s2 : lower(right)에 속한 점들의 집합 (ax+by-c > 0)
// [output] n1 : s1 집합에 속한 점의 수
// [output] n2 : s2 집합에 속한 점의 수
// from, to 자신의 점은 s1이나 s2에 속하지 않도록 하기

void separate_points(t_point* points, int num_point, t_point from, t_point to, t_point* s1, t_point* s2, int* n1, int* n2) {

	*n1 = 0;
	*n2 = 0;

	for (int i = 0; i < num_point; i++) {
		if ((points[i].x == from.x) && (points[i].y == from.y))
			continue;
		else if ((points[i].x == to.x) && (points[i].y == to.y))
			continue;
		else {
			if ((to.y - from.y) * points[i].x + (from.x - to.x) * points[i].y - (from.x) * (to.y) + (from.y) * (to.x) < 0) {
				*n1 = (*n1) + 1;
				s1[(*n1)-1].x = points[i].x;
				s1[(*n1)-1].y = points[i].y;
				
			}
			else if( (to.y - from.y) * points[i].x + (from.x - to.x) * points[i].y - (from.x) * (to.y) + (from.y) * (to.x) > 0){
				*n2 = (*n2) + 1;
				s2[(*n2)-1].x = points[i].x;
				s2[(*n2)-1].y = points[i].y;
				
			}
		}
	}

}

// function declaration
// 재귀함수(내부적으로 upper hull 또 호출)
// 결과물은 lines, num_line에 저장됨. lines는 어퍼 헐 구해지면 걔네를 여기에 넣음. num line은 기존의 저장된 선이 다섯개면 새로 세개가 추가되면 8개로 바꿔줘야.
// capacity는 lines를 위해서 할당한 메모리 용량, 현재까지 할당된 메모리 사이즈 t_line 구조체에 할당된 사이즈. 처음엔 10개 선을 저장할 수 있게 사이즈를 잡고, 하다보면 10개 넘어갈 수 있으니까
// capcity 수정하고 메모리 재할당하고 라인 추가해주기.
// 내부에서 메모리 재할당하면 lines가 가르키는 주소가 바뀔 수 있어서 내부적으로 재할당한거를 주소를 다시 넘겨서 얘가 리턴받아서 사용하도록 해야 한다. 그래서 리턴형이 t_line
// 점들의 집합(points; 점의 수 num_point)에서 점 p1과 점 pn을 잇는 직선의 upper hull을 구하는 함수 (재귀호출)
// [output] lines: convex hull을 이루는 선들의 집합
// [output] num_line: 선의 수
// [output] capacity: lines에 할당된 메모리의 용량 (할당 가능한 선의 수)
// return value: 선들의 집합(lines)에 대한 포인터
// 여기 안에서 distance 함수 쓴다
// 이거의 base case(재귀 종료 조건)은 num_point==0일 떄, p1, pn을 잇는 선 자체가 upperhull에 포함되야
t_line* upper_hull(t_point* points, int num_point, t_point p1, t_point pn, t_line* lines, int* num_line, int* capacity) {
	//재귀 함수 종료
	if ((*num_line) >= (*capacity)) {
			(*capacity) = (*capacity) + 10;
			lines = (t_line*)realloc(lines, sizeof(t_line) * (*capacity));
		}
		
	if (num_point == 0) {
		lines[*num_line].from.x = p1.x;
		lines[*num_line].from.y = p1.y;
		lines[*num_line].to.x = pn.x;
		lines[*num_line].to.y = pn.y;
		(*num_line) = (*num_line) + 1;
		return lines;
	}

	//점들 중에서 직선과 거리가 제일 먼 점 찾기
	float max = 0.0;
	float outa = (float)pn.y - (float)p1.y;
	float outb = (float)p1.x - (float)pn.x;
	int k= (p1.x) * (pn.y) - (p1.y) * (pn.x);
	float outc = (float)k;
	t_point maxdistancepoint;
	for (int i = 0; i < num_point; i++) {
		//if ((points[i].x == p1.x) && (points[i].y == p1.y))
			//continue;
		//else if ((points[i].x == pn.x) && (points[i].y == pn.y))
			//continue;
		if (max < distance(outa, outb, outc, points[i])) {
			max=distance(outa, outb, outc, points[i]);
			maxdistancepoint.x = points[i].x;
			maxdistancepoint.y = points[i].y;
		}
	}
	
	t_point* s11 = (t_point*)malloc(sizeof(t_point) * num_point);
	assert(s11 != NULL);


	t_point* s12 = (t_point*)malloc(sizeof(t_point) * num_point);
	assert(s12 != NULL);
	
	t_point* s111 = (t_point*)malloc(sizeof(t_point) * num_point);
	assert(s111 != NULL);


	t_point* s112 = (t_point*)malloc(sizeof(t_point) * num_point);
	assert(s112 != NULL);

	int n11,n12;
	int n111,n112;
	
	separate_points(points, num_point, p1, maxdistancepoint, s11, s12, &n11, &n12);
	separate_points(points, num_point, maxdistancepoint, pn, s111, s112, &n111, &n112);
	
	lines = upper_hull(s11, n11, p1, maxdistancepoint, lines, num_line, capacity);
    lines = upper_hull(s111, n111, maxdistancepoint, pn, lines, num_line, capacity);
	
	free(s11);
	free(s12);
	free(s111);
	free(s112);
	
	return lines;
}

// 직선(ax+by-c=0)과 주어진 점 p(x1, y1) 간의 거리
// distance = |ax1+by1-c| / sqrt(a^2 + b^2)
// 실제로는 sqrt는 계산하지 않음
// return value: 직선과 점 사이의 거리 (분모 제외) -->상대적 거리 찾고자 하는거라서 분모는 제외하기
// 가장 거리가 큰 점을 찾고자 하는 것.
float distance(float a, float b, float c, t_point p) {
	return fabs(a * (p.x) + b * (p.y) - c);
}