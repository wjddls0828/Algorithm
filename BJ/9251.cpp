#include <iostream>
#include <algorithm>
#include <string.h>
using namespace std;

char s1[1010];
char s2[1010];
int len1, len2;
int dp[1010][1010];

int main() {
	scanf("%s", &s1[1]);
	scanf("%s", &s2[1]);

	len1 = strlen(&s1[1]);
	len2 = strlen(&s2[1]);

	for (int i = 1; i <= len1; i++) {
		for (int j = 1; j <= len2; j++) {
			if (s1[i] == s2[j])
				dp[i][j] = dp[i - 1][j - 1] + 1;
			else
				dp[i][j] = max(dp[i - 1][j], dp[i][j - 1]);
		}
	}

	printf("%d", dp[len1][len2]);

	return 0;

}