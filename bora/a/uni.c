#include <stdio.h>

const int mod = 13;

void produt (int n, int mat1[][n], int mat2[][n], int k, int prodt[][n]) {

	int i, j, y, l, prod=0;

	if (k==0){
		for (i=0; i<n; i++) {
			for (j=0; j<n; j++) {
				if (i==j) {
					prodt[i][j] = 1;
				}
				else {
					prodt[i][j] = 0;
				}
			}
		}
	}

	else if(k==1) {
		for (i=0; i<n; i++) {
			for (j=0;j<n;j++) {
				prodt[i][j] = (mat1[i][j] % mod);
			}
		}
	}

	else if (k >1) {
		for (y = 2; y < k; ++y) {
			for (i=0; i<n; i++){
				for (j=0; i<n; j++) {
					for(l=0; l<n; l++) {
						prod = (( (prod%mod) +  ( ( (mat2[l][j]%mod) * (mat1[i][l]%mod)  )%mod ) )%mod)%mod;
					}

					l=0;
					prodt[i][j] = prod;
					prod = 0;
				}
			}
			for (i=0; i<n; i++){
				for (j=0; j<n; j++) {
					mat2[i][j] = prodt[i][j];
				}
			}
		}
	}
}

int main() {
	
	int n, k, i, j;

	scanf("%d %d", &n, &k);

	int mat1[n][n], mat2[n][n], prodt[n][n];

	for (i=0; i<n; i++) {
		for (j=0;j<n;j++) {
			scanf("%d", &mat1[i][j]);
			mat2[i][j] = mat1[i][j];
		}
	}

	produt(n, mat1, mat2, k, prodt);

	for (i=0; i<n; i++) {
		for (j=0;j<n;j++) {
			printf("%d", prodt[i][j]);
		}
		printf("\n");
	}
	return 0;
}