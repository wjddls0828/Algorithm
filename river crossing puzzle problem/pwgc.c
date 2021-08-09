#include <stdio.h>
#include <stdlib.h>

#define PEASANT 0x08
#define WOLF	0x04
#define GOAT	0x02
#define CABBAGE	0x01

// 주어진 상태 state의 이름(마지막 4비트)을 화면에 출력
// 예) state가 7(0111)일 때, "<0111>"을 출력
static void print_statename(FILE* fp, int state);

// 주어진 상태 state에서 농부, 늑대, 염소, 양배추의 상태를 각각 추출하여 p, w, g, c에 저장
// 예) state가 7(0111)일 때, p = 0, w = 1, g = 1, c = 1
static void get_pwgc(int state, int* p, int* w, int* g, int* c);

// 허용되지 않는 상태인지 검사
// 예) 농부없이 늑대와 염소가 같이 있는 경우 / 농부없이 염소와 양배추가 같이 있는 경우
// return value: 1 허용되지 않는 상태인 경우, 0 허용되는 상태인 경우
static int is_dead_end(int state);

// state1 상태에서 state2 상태로의 전이 가능성 점검
// 농부 또는 농부와 다른 하나의 아이템이 강 반대편으로 이동할 수 있는 상태만 허용
// 허용되지 않는 상태(dead-end)로의 전이인지 검사
// return value: 1 전이 가능한 경우, 0 전이 불이가능한 경우 
static int is_possible_transition(int state1, int state2);

// 상태 변경: 농부 이동
// return value : 새로운 상태
static int changeP(int state);

// 상태 변경: 농부, 늑대 이동
// return value : 새로운 상태, 상태 변경이 불가능한 경우: -1
static int changePW(int state);

// 상태 변경: 농부, 염소 이동
// return value : 새로운 상태, 상태 변경이 불가능한 경우: -1
static int changePG(int state);

// 상태 변경: 농부, 양배추 이동
// return value : 새로운 상태, 상태 변경이 불가능한 경우: -1 
static int changePC(int state);

// 주어진 state가 이미 방문한 상태인지 검사
// return value : 1 visited, 0 not visited
static int is_visited(int visited[], int level, int state);

// 방문한 상태들을 차례로 화면에 출력
static void print_states(int visited[], int count);

// recursive function
static void dfs_main(int state, int goal_state, int level, int visited[]);

////////////////////////////////////////////////////////////////////////////////
// 상태들의 인접 행렬을 구하여 graph에 저장
// 상태간 전이 가능성 점검
// 허용되지 않는 상태인지 점검 
void make_adjacency_matrix(int graph[][16]);

// 인접행렬로 표현된 graph를 화면에 출력
void print_graph(int graph[][16], int num);

// 주어진 그래프(graph)를 .net 파일로 저장
// pgwc.net 참조
void save_graph(char* filename, int graph[][16], int num);

////////////////////////////////////////////////////////////////////////////////
// 깊이 우선 탐색 (초기 상태 -> 목적 상태)
void depth_first_search(int init_state, int goal_state);


////////////////////////////////////////////////////////////////////////////////
int main(int argc, char** argv)
{
	int graph[16][16] = { 0, };

	// 인접 행렬 만들기
	// 각 상태에서 다른 상태로의 전이 가능성에 따라 0이나 1로 값을 정하기
	make_adjacency_matrix(graph);

	// 인접 행렬 출력 (only for debugging)
	// 제출할때는 주석처리해서 올리기
	print_graph(graph, 16);

	// .net 파일 만들기
	save_graph("pwgc.net", graph, 16);

	// 깊이 우선 탐색
	depth_first_search(0, 15); // initial state, goal state

	return 0;
}

// 주어진 상태 state의 이름(마지막 4비트)을 화면에 출력
// 예) state가 7(0111)일 때, "<0111>"을 출력
static void print_statename(FILE* fp, int state) {
	/*int statename[4];
	for (int i = 0; i < 4; i++) {
		statename[3 - i] = (state >> i) & 1;
	}
	fprintf(fp, "%s", "<");
	for (int k = 0; k < 4; k++) {
		fprintf(fp, "%d", statename[k]);
	}
	fprintf(fp, "%s\n", ">");*/
	
	int p,w,g,c;
	get_pwgc(state,&p,&w,&g,&c);
	fprintf(fp, "<");
	fprintf(fp, "%d",p);
	fprintf(fp,"%d",w);
	fprintf(fp,"%d",g);
	fprintf(fp,"%d",c);
	fprintf(fp, ">");
	
}

// 주어진 상태 state에서 농부, 늑대, 염소, 양배추의 상태를 각각 추출하여 p, w, g, c에 저장
// 예) state가 7(0111)일 때, p = 0, w = 1, g = 1, c = 1
static void get_pwgc(int state, int* p, int* w, int* g, int* c) {
	/**c = (state >> 0) & 1;
	*g = (state >> 1) & 1;
	*w = (state >> 2) & 1;
	*p = (state >> 3) & 1;*/
	
	*c=state%2;
	state=state/2;
	*g=state%2;
	state=state/2;
	*w=state%2;
	state=state/2;
	*p=state%2;
}

// 허용되지 않는 상태인지 검사
// 예) 농부없이 늑대와 염소가 같이 있는 경우 / 농부없이 염소와 양배추가 같이 있는 경우
// return value: 1 허용되지 않는 상태인 경우,         0 허용되는 상태인 경우 
static int is_dead_end(int state) {
	int p, w, g, c;
	get_pwgc(state, &p, &w, &g, &c);

	//허용되지 않는 상태인 경우
	//농부가 0에 있음
	if (p == 0) {
		if (w ==1 && g == 1) //늑대, 염소 1
			return 1;
		if (g ==1 && c == 1) //염소 양배추 1
			return 1;
	}

	//농부가 1에 있음
	if (p == 1) {
		if (w ==0 &&  g == 0) //늑대, 염소 0
			return 1;
		if (g == 0 && c == 0) //염소 양배추 0
			return 1;

	}
	//나머지는 허용
	return 0;

}

// state1 상태에서 state2 상태로의 전이 가능성 점검
// 농부 또는 농부와 다른 하나의 아이템이 강 반대편으로 이동할 수 있는 상태만 허용
// 허용되지 않는 상태(dead-end)로의 전이인지 검사: 물리적으로 농부와 양배추가 건너편으로 가면, 염소와 늑대가 남으니까 dead end니까 0 리턴
// return value: 1 전이 가능한 경우,        0 전이 불이가능한 경우 
static int is_possible_transition(int state1, int state2) {
	//전이 불가능한 경우
	if (is_dead_end(state1) == 1)
		return 0;
	
	if (is_dead_end(state2) == 1)
		return 0;
		
	int p1, w1, g1, c1;
	int p2, w2, g2, c2;
	get_pwgc(state1, &p1, &w1, &g1, &c1);
	get_pwgc(state2, &p2, &w2, &g2, &c2);
	
	//농부만 이동
	if ((p1 != p2) && (w1 == w2) && (g1 == g2) && (c1 == c2))
		return 1;
		
	//농부&늑대만 이동
	if(p1==w1){
		if ((p1 != p2) && (w1 != w2) && (g1 == g2) && (c1 == c2))
		return 1;
	}
	
	//농부 염소만 이동
	if(p1==g1){
		if ((p1 != p2) && (w1 == w2) && (g1 != g2) && (c1 == c2))
		return 1;
	}
	
	//농부 양배추만 이동
	if(p1==c1){
		if ((p1 != p2) && (w1 == w2) && (g1 == g2) && (c1 != c2))
		return 1;
	}
	
	return 0;
}

// 상태 변경: 농부 이동
// return value : 새로운 상태
static int changeP(int state) {
	int p, w, g, c;
	get_pwgc(state, &p, &w, &g, &c);
	if (p == 0)
		p = 1;
	else if (p == 1)
		p = 0;
	return c + (2 * g) + (4 * w) + (8 * p);
}

// 상태 변경: 농부, 늑대 이동
// 강의 같은 쪽에 농부와 늑대가 같이 있어야 가능 
// return value : 새로운 상태, 상태 변경이 불가능한 경우: -1
static int changePW(int state) {
	int p, w, g, c;
	get_pwgc(state, &p, &w, &g, &c);
	//상태 변경이 불가능한 경우(농부와 늑대가 다른쪽에 있는 경우)
	
	if (p != w)
		return -1;
	//농부와 늑대 이동
	if ((p ==0) &&  (w == 0)) {
		p = 1;
		w = 1;
	}
	else if ((p ==1) &&  (w == 1)) {
		p = 0;
		w = 0;
	}
	return c + (2 * g) + (4 * w) + (8 * p);
}

// 상태 변경: 농부, 염소 이동
// return value : 새로운 상태, 상태 변경이 불가능한 경우: -1
static int changePG(int state) {
	int p, w, g, c;
	get_pwgc(state, &p, &w, &g, &c);
	//상태 변경이 불가능한 경우(농부와 염소가 다른쪽에 있는 경우)
	if (p != g)
		return -1;

	//농부와 염소 이동
	if ((p == 0) &&( g == 0)) {
		p = 1;
		g = 1;
	}
	else if ((p == 1)&&(g == 1)) {
		p = 0;
		g = 0;
	}

	return c + (2 * g) + (4 * w) + (8 * p);

}

// 상태 변경: 농부, 양배추 이동
// return value : 새로운 상태, 상태 변경이 불가능한 경우: -1 
static int changePC(int state) {
	int p, w, g, c;
	get_pwgc(state, &p, &w, &g, &c);

	//상태 변경이 불가능한 경우(농부와 양배추가 다른쪽에 있는 경우)
	if (p != c)
		return -1;

	//농부와 염소 이동
	if ((p == 0)&& (c == 0)) {
		p = 1;
		c = 1;
	}
	else if ((p == 1)&&(c == 1)) {
		p = 0;
		c = 0;
	}

	return c + (2 * g) + (4 * w) + (8 * p);
}

// 주어진 state가 이미 방문한 상태인지 검사
// return value : 1 visited, 0 not visited
// 깊이 우선 탐색에서 이미 방문했는지 확인, 방문한 상태들을 저장할 공간이 필요한데 이걸 배열로 선언
// 초기상태 레벨 0, 얼마나 깊이 들어왔느냐, 그 레벨에 따라 검사해야할 visited,배열, 전체를 다 비교할 필요 없이 해당 레벨 까지만 비교
// state는 이 스테이트를 방문한 적이 있는지, 입력
static int is_visited(int visited[], int level, int state) {
	for (int i = 0; i <= level; i++) {
		if (visited[i] == state) {
			return 1;
		}
	}
	return 0;
}

// 방문한 상태들을 차례로 화면에 출력
// 목적 상태에 도달했을 때 내가 어떤 경로를 따라서 왔는지 0000부터 1111까지 그 정보가 visted에 있겠죠, count는 level과 같음
static void print_states(int visited[], int count) {
	int statename[4];
	for (int e = 0; e <= count; e++) {
		for (int i = 0; i < 4; i++) {
			statename[3 - i] = (visited[e] >> i) & 1;
		}
		printf("%s", "<");
		for (int k = 0; k < 4; k++) {
			fprintf(stdout, "%d", statename[k]);
		}
		printf("%s\n", ">");
	}
}

// recursive function
// 재귀적 호출
// level은 재귀 호출할 때 level 하나씩 증가시켜가면서 사용
static void dfs_main(int state, int goal_state, int level, int visited[]) {

	int p, w, g, c;
	get_pwgc(state, &p, &w, &g, &c);

	visited[level] = state;
	printf("cur state is ");
	print_statename(stdout, state);
	printf(" (level %d)", level);
	printf("\n");

	if (state == 15) {
		printf("Goal-state found!\n");
		print_states(visited, level);
		printf("\n");
		return;
	}


	//state에서 가능한 다음 방문 상태 확인해서 nextstate에 저장
	//가능한 방문상태로 dfs 재귀 호출
	//농부만 이동
	if (is_possible_transition(state, changeP(state)) == 0){
			printf("\t");
			printf("next state ");
			print_statename(stdout, changeP(state));
			printf(" is dead-end\n");
	}
	else if ((changeP(state)!=-1) && (is_visited(visited, level, changeP(state)) == 1)) {
			printf("\t");
			printf("next state ");
			print_statename(stdout, changeP(state));
			printf(" has been visited\n");
	}
	else {
		dfs_main(changeP(state), goal_state, (level + 1), visited);
		printf("back to ");
		print_statename(stdout, state);
		printf(" (level %d)", level);
		printf("\n");
	}


	// 농부 늑대 이동
	if (p == w) {
		if (is_possible_transition(state, changePW(state)) == 0) {
			printf("\t");
				printf("next state ");
				print_statename(stdout, changePW(state));
				printf(" is dead-end\n");
		}
		else if ((changePW(state)!=-1)&&(is_visited(visited, level, changePW(state)) == 1)) {
			printf("\t");
				printf("next state ");
				print_statename(stdout, changePW(state));
				printf(" has been visited\n");
		}
		else {
			dfs_main(changePW(state), goal_state, (level + 1), visited);
			printf("back to ");
			print_statename(stdout, state);
			printf(" (level %d)", level);
			printf("\n");
		}
	}

	// 농부 염소 이동
	if (p == g) {
		if (is_possible_transition(state, changePG(state)) == 0 ){
				printf("\t");
				printf("next state ");
				print_statename(stdout, changePG(state));
				printf(" is dead-end\n");
		}
		else if ((changePG(state)!=-1)&&(is_visited(visited, level, changePG(state)) == 1)) {
			printf("\t");
				printf("next state ");
				print_statename(stdout, changePG(state));
				printf(" has been visited\n");
		}
		else {
			dfs_main(changePG(state), goal_state, (level + 1), visited);
			printf("back to ");
			print_statename(stdout, state);
			printf(" (level %d)", level);
			printf("\n");
		}
	}

	// 농부 양배추 이동
	if (p == c) {
		if (is_possible_transition(state, changePC(state)) == 0){
				printf("\t");
				printf("next state ");
				print_statename(stdout, changePC(state));
				printf(" is dead-end\n");
		}
		else if ((changePC(state))&&(is_visited(visited, level, changePC(state)) == 1)) {
				printf("\t");
				printf("next state ");
				print_statename(stdout, changePC(state));
				printf(" has been visited\n");
		}
		else {
			dfs_main(changePC(state), goal_state, (level + 1), visited);
			printf("back to ");
			print_statename(stdout, state);
			printf(" (level %d)", level);
			printf("\n");
		}
	}

}

////////////////////////////////////////////////////////////////////////////////
// 상태들의 인접 행렬을 구하여 graph에 저장
// 상태간 전이 가능성 점검
// 허용되지 않는 상태인지 점검 
void make_adjacency_matrix(int graph[][16]) {
	for (int i = 0; i < 16; i++) {//i행
		for (int j = 0; j < 16; j++) {//j열
			//graph[i][j], 상태 i 에서 상태 j로 전이 가능성 점검
			if (is_possible_transition(i, j) == 1)
				graph[i][j] = 1;
			else
				graph[i][j] = 0;
		}
	}
}

// 인접행렬로 표현된 graph를 화면에 출력
void print_graph(int graph[][16], int num) {
	for (int i = 0; i < 16; i++) {
		for (int j = 0; j < 16; j++) {
			if(j==15)
				printf("%d\n",graph[i][j]);
			else
				printf("%d\t", graph[i][j]);
		}
	}
}

// 주어진 그래프(graph)를 .net 파일로 저장
// pgwc.net 참조
void save_graph(char* filename, int graph[][16], int num) {
	FILE* fp;
	fp = fopen(filename, "wt");

	fprintf(fp, "*Vertices 16\n");
	for (int i = 1; i <= num; i++) {
		fprintf(fp, "%d ", i);
		fprintf(fp, "\"");
		print_statename(fp, i-1);
		fprintf(fp, "\"\n");

	}

	fprintf(fp, "*Edges\n");
	for (int row = 0; row < 16; row++) {
		for (int column = row; column < 16; column++) {
			if (graph[row][column] == 1)
				fprintf(fp, "  %d  %d\n", row+1, column+1);
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
// 깊이 우선 탐색 (초기 상태 -> 목적 상태)
void depth_first_search(int init_state, int goal_state)
{
	int level = 0;
	int visited[16] = { 0, }; // 방문한 정점을 저장

	dfs_main(init_state, goal_state, level, visited);
}

