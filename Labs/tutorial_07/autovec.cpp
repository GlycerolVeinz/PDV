#include <iostream>
#include <chrono>
#include <cstdlib>
#include <cmath>
#include <random>

#include <immintrin.h>

using namespace std::chrono;

// Pocet prvku v poli. Pro jednoduchost alokujeme pole na stacku.
// Pokud byste chteli experimentovat s vyrazne vetsim poctem prvku
// (ktere se uz nevejdou na stack), muzete kod prepsat na dynamickou
// alokaci pameti (std::vector / new).
constexpr size_t N = 65536 * 4;


void runSequential(float b[], float c[]){
	float a[N];
	auto begin = steady_clock::now();

//	 #pragma loop(no_vector)
	for (unsigned int i = 0; i < N; i++){
		a[i] = b[i]*b[i] + c[i]*c[i] + b[i]*c[i] + b[i]/c[i] + c[i]/b[i] + b[i]/(b[i]+c[i]) + c[i]/(b[i]-c[i]) + c[i]/(b[i]+c[i]);
		//a[i] = sqrt(b[i]*b[i] + c[i]*c[i] + b[i]*c[i] + b[i]/c[i] + c[i]/b[i] + b[i]/(b[i]+c[i]) + c[i]/(b[i]-c[i]) + c[i]/(b[i]+c[i]));
	}
	auto end = steady_clock::now();

	float acc = 0.0f;
	for(unsigned int i = 0 ; i < N ; i++) {
		acc += a[i];
		if(a[i] > 1000) {
			//printf("%d: %f\n",i , a[i]);
		}
	}

	std::cout << "Scalar (auto-vectorized?) produced result " << acc << " in " << duration_cast<microseconds>(end-begin).count() << " us" << std::endl;
}

void runVectorized(float b[], float c[]) {
	float a[N];
	auto begin = steady_clock::now();
	for (unsigned int i = 0; i < N; i += 8){
		__m256 B = _mm256_loadu_ps(&b[i]);
		__m256 C = _mm256_loadu_ps(&c[i]);

		__m256 A = _mm256_mul_ps(B, B);
		A = _mm256_fmadd_ps(C, C, A);
		A = _mm256_fmadd_ps(B, C, A);
		A = _mm256_add_ps(A, _mm256_div_ps(B, C));
		A = _mm256_add_ps(A, _mm256_div_ps(C, B));
		A = _mm256_add_ps(A, _mm256_div_ps(B, _mm256_add_ps(B, C)));
		A = _mm256_add_ps(A, _mm256_div_ps(C, _mm256_add_ps(B, C)));
		A = _mm256_add_ps(A, _mm256_div_ps(C, _mm256_sub_ps(B, C)));
		//A = _mm256_sqrt_ps(A);

		_mm256_storeu_ps(&a[i], A);
	}
	auto end = steady_clock::now();

	float acc = 0.0f;
	for(unsigned int i = 0 ; i < N ; i++) {
		acc += a[i];
	}

	std::cout << "Manually vectorized code produced result  " << acc << " in " << duration_cast<microseconds>(end-begin).count() << " us" << std::endl;
}

int main() {
	float b[N], c[N];

	std::default_random_engine random_engine;

	std::uniform_real_distribution<float> dist_b(1, 2);
	std::uniform_real_distribution<float> dist_c(0.0000000001f, 1);

	for(unsigned int i = 0 ; i < N ; i++) {
		b[i] = dist_b(random_engine);
		c[i] = dist_c(random_engine);
	}

	// Spocitame dostatecne slozitou funkci
	// Sekvencne
	runSequential(b,c);

	// Vektorove
	runVectorized(b,c);

	return 0;
}


