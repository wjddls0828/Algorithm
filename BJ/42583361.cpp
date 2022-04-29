#define _CRT_SECURE_NO_WARNINGS
#include <algorithm>
#include <cstdio>
#define X 5000
using namespace std;

int dp[5001];

int main() {
	dp[0] = 0; 
	dp[1] = X;
	dp[2] = X;
	dp[3] = 1;
	dp[4] = X;
	dp[5] = 1;

	int N;
	scanf("%d", &N);

	for (int i = 6; i <= N; i++) 
		dp[i] = min(dp[i - 3] + 1, dp[i - 5] + 1);

	if (dp[N] >= X) {
		printf("-1");
	}
	else {
		printf("%d", dp[N]);
	}

	return 0;
}