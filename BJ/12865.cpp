#define _CRT_SECURE_NO_WARNINGS
#include <algorithm>
#include <cstdio>

using namespace std;
// N개 물건: 무게 W, 가치 V
// 배낭에 최대 K 무게

int N, K;
int W[110];
int V[110];
int dp[110][100010];

int main() {
	scanf("%d %d", &N, &K);
	for (int i = 1; i <= N; i++) 
		scanf("%d %d", &W[i], &V[i]);

	for (int i = 1; i <= N; i++) {
		for (int j = 1; j <= K; j++) {
			if (j < W[i]) {
				dp[i][j] = dp[i - 1][j];
			}
			else {
				dp[i][j] = max(dp[i - 1][j], V[i] + dp[i -1][j-W[i]]);
			}
		}
	}

	printf("%d", dp[N][K]);
	
}