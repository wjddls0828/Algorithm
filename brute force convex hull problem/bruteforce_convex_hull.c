#include <stdlib.h> // atoi, rand, malloc, realloc
#include <stdio.h>
#include <time.h> //time

#define RANGE 10000 //x축 y축의 범위

typedef struct
{
	int x;//x좌표
	int y;//y좌표
} t_point; //점에 대한 구조체

typedef struct
{
	t_point from;//점 1개
	t_point to;//점 2개
} t_line;//선분에 대한 정보(점 2개에 대한 정보)

////////////////////////////////////////////////////////////////////////////////
void print_header( char *filename)
{
	printf( "#! /usr/bin/env Rscript\n");
	printf( "png(\"%s\", width=700, height=700)\n", filename);
	
	printf( "plot(1:%d, 1:%d, type=\"n\")\n", RANGE, RANGE);
}
////////////////////////////////////////////////////////////////////////////////
void print_footer( void)
{
	printf( "dev.off()\n");
}

////////////////////////////////////////////////////////////////////////////////
/*
#points
points(2247,7459)
points(616,2904)
points(5976,6539)
points(1246,8191)
*/
void print_points( t_point *points, int num_point); //프로토타입메인 함수 아래에 함수 만들어야

/*
#line segments
segments(7107,2909,7107,2909) //두점에 대한 정보 한점/한점
segments(43,8,5,38)
segments(43,8,329,2)
segments(5047,8014,5047,8014)
*/
void print_line_segments( t_line *lines, int num_line);//만들어야

// [input] points : set of points 점들의 집합
// [input] num_point : number of points 점들이 몇개 입력으로들어왔는지
// [output] num_line : number of line segments that forms the convex hull 선분이 몇개인지
// return value : set of line segments that forms the convex hull 결과가 컨벡스 홀을 이루는 선분들의 집합, 메모리 할당해서 선분에 대한 정보를 가지고 있고 그걸 나중에 t-line 포인터로 반환해주기
t_line *convex_hull( t_point *points, int num_point, int *num_line);//만들어야

////////////////////////////////////////////////////////////////////////////////
int main( int argc, char **argv)
{
	int x, y;
	int num_point; // number of points
	int num_line; // number of lines
	
	if (argc != 2)
	{
		printf( "%s number_of_points\n", argv[0]);
		return 0;
	}

	num_point = atoi( argv[1]); //문자로 받아서 숫자로 바꿔줌
	if (num_point <= 0)
	{
		printf( "The number of points should be a positive integer!\n");
		return 0;
	}

	t_point *points = (t_point *) malloc( num_point * sizeof( t_point));
		
	t_line *lines;

	// making n points
	srand( time(NULL));
	for (int i = 0; i < num_point; i++)
	{
		x = rand() % RANGE + 1; // 1 ~ RANGE random number
		y = rand() % RANGE + 1;
		
		points[i].x = x;
		points[i].y = y;
 	}

	fprintf( stderr, "%d points created!\n", num_point);

	print_header( "convex.png");
	
	print_points( points, num_point); //점들의 집합 출력
	
	lines = convex_hull( points, num_point, &num_line); 

	fprintf( stderr, "%d lines created!\n", num_line);

	print_line_segments( lines, num_line);
		
	print_footer();
	
	free( points);
	free( lines);
	
	return 0;
}


/*
#points
points(2247,7459)
points(616,2904)
points(5976,6539)
points(1246,8191)
*/
void print_points( t_point *points, int num_point){
	printf("\n#points\n");
	for(int i=0;i<num_point;i++){
		printf("points(%d,%d)\n",points[i].x,points[i].y);
	}
}

// [input] points : set of points 점들의 집합
// [input] num_point : number of points 점들이 몇개 입력으로들어왔는지
// [output] num_line : number of line segments that forms the convex hull 선분이 몇개인지
// return value : set of line segments that forms the convex hull 결과가 컨벡스 홀을 이루는 선분들의 집합, 메모리 할당해서 선분에 대한 정보를 가지고 있고 그걸 나중에 t-line 포인터로 반환해주기
//malloc해야, 초기에 10개 만들고 10개 넘어가면 10개씩 추가. realloc 사용, capacity보다 더 잡아야 할때는 capcity를 10개씩 증가
t_line *convex_hull( t_point *points, int num_point, int *num_line){
	int pastD;
	int success;
	int D;
	int numberoflines=0;
	int presentmalloclines;
	t_line* setlines=(t_line*)malloc(10*sizeof(t_line));
	presentmalloclines=10;
	for(int i=0;i<num_point;i++){ //point[i]
		for(int j=i+1;j<num_point;j++){ //point[j]
			pastD=0; //판별식초기값
			success=1; //성공여부 초기값
			for(int k=0;k<num_point;k++){ //선분에 대해서 point[k]에 대해 비교
				if(k==i||k==j) //point i와 j는 제외
					continue;
				D=((points[j].y-points[i].y)*points[k].x)+((points[i].x-points[j].x)*points[k].y)-(points[i].x*points[j].y)+(points[i].y*points[j].x); //k에 대한 판별식
				
				if(D>=0){
					if(pastD<0){
						success=-1;
						break;
					}
					pastD=1;
					
				}
				else{
					if(pastD>0){
						success=-1;
						break;
					}
					pastD=-1;
				}
			}
			if(success==1){
				numberoflines++;
				if(numberoflines>presentmalloclines){
					presentmalloclines+=10;
					setlines=(t_line*)realloc(setlines,sizeof(t_line)*presentmalloclines);
				}
				setlines[numberoflines-1].from.x=points[i].x;
				setlines[numberoflines-1].from.y=points[i].y;
				setlines[numberoflines-1].to.x=points[j].x;
				setlines[numberoflines-1].to.y=points[j].y;	
			}
		}
		
	}
	*num_line=numberoflines;
	//printf("convex 성공");
	return setlines;
}

/*
#line segments
segments(7107,2909,7107,2909) //두점에 대한 정보 한점/한점
segments(43,8,5,38)
segments(43,8,329,2)
segments(5047,8014,5047,8014)
*/
void print_line_segments( t_line *lines, int num_line){
	printf("\n#line segments\n");
	for(int t=0;t<num_line;t++){
		printf("segments(%d,%d,%d,%d)\n",lines[t].from.x,lines[t].from.y,lines[t].to.x,lines[t].to.y);
	}
}