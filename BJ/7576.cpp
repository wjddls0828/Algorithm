#include <iostream>
#include <queue>

using namespace std;

int N, M; //N: 세로, M: 가로
int nx, ny;
int grid[1010][1010];
int dx[] = { 1,0,-1,0 };
int dy[] = { 0, -1,0,1 };

struct xy {
	int x, y;
};

queue<struct xy> q;

int main() {
	//input
	scanf("%d %d", &M, &N);
	for (int i = 1; i <= N; i++) {
		for (int j = 1; j <= M; j++) {
			scanf("%d", &grid[i][j]);
			if (grid[i][j] == 1)
				q.push({ i,j });
		}
	}

	//bfs
	while (q.size() > 0) {
		//방문
		auto c = q.front();
		q.pop();
		//탐색
		for (int i = 0; i < 4; i++) {
			nx = c.x + dx[i];
			ny = c.y + dy[i];

			if (!(nx >= 1 && nx <= N && ny >= 1 && ny <= M))
				continue;

			if (grid[nx][ny] == 0) {
				grid[nx][ny] = grid[c.x][c.y] + 1;
				q.push({ nx,ny });
			}
		}
	}
	

	//gird 최대값
	int max = -987654;
	for (int i = 1; i <= N; i++) {
		for (int j = 1; j <= M; j++) {
			if (grid[i][j] == 0) {
				printf("-1");
				return 0;
			}
			if (max < grid[i][j])
				max = grid[i][j];
		}
	}

	printf("%d", max-1);
	return 0;

}