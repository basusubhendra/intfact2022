#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gmp.h>
#include <string>
#include <algorithm>
#include <boost/lexical_cast.hpp>
#include "primes.hpp"
#include "zeros.hpp"
using namespace std;
using namespace boost;

std::string _bin(int x, int param) {
	if (x == 0) {
		char* r = (char*) calloc(2, sizeof(char));
		r[0] = '0';
		r[1] = '\0';
		return r;
	}
	std::string bstring = "";
	while (x > 0) {
		int rem = x % 2;
		x /= 2;
		bstring += boost::lexical_cast<std::string>(rem);
	}
	if (param == 0) {
		std::reverse(bstring.begin(), bstring.end());
	}
	return bstring;
}

//convert a binary string to its 
//arbitrary precision integer equivalent
char* _int(std::string b) {
	mpz_t sum;
	mpz_init(sum);
	mpz_set_si(sum, 0);
	mpz_t term;
	mpz_init(term);
	mpz_t prod;
	mpz_init(prod);
	mpz_set_ui(prod, 1);
	long l = b.size();
	for (int i = l - 1; i >= 0; --i) {
		int bk = b[i] - '0';
		mpz_mul_ui(term, prod, bk);
		mpz_add(sum, sum, term);
		mpz_mul_ui(prod, prod, 2);
	}
	mpz_clear(term);
	mpz_clear(prod);
	char* int_num = strdup(mpz_get_str(0, 10, sum));
	mpz_clear(sum);
	return int_num;
}

//return the product of two
//arbitrary precision integers
//x and y
char* _product(char* x, char* y) {
	mpz_t xz;
	mpz_init(xz);
	mpz_set_str(xz, x, 10);
	mpz_t yz;
	mpz_init(yz);
	mpz_set_str(yz, y, 10);
	mpz_t prod;
	mpz_init(prod);
	mpz_mul(prod, xz, yz);
	mpz_clear(xz);
	mpz_clear(yz);
	char* product =  strdup(mpz_get_str(0, 10, prod));
	mpz_clear(prod);
	return product;
}

bool disambiguate(char* num, long l, char* tmpfile1, char* tmpfile2, long fpos, vector<long> prime_posits) {
	FILE* tmp2 = fopen64(tmpfile2, "r");
	char tmpfile3[L_tmpnam + 1];
	tmpnam(tmpfile3);
	FILE* tmp3 = fopen64(tmpfile3, "w");
	//read from tmpfile2 and write it
	//in reverse to tmpfile3.
	char pp = 0, ee = 0;
	long cnt = 1;
	fseek(tmp2, -cnt, SEEK_END);
	while (cnt <= fpos) {
		fscanf(tmp2, "%c", &pp);
		fprintf(tmp3, "%c", pp);
		++cnt;
		fseek(tmp2, -cnt, SEEK_END);
	}
	fclose(tmp2);
	fclose(tmp3);
	tmp3 = fopen64(tmpfile3, "r");
	FILE* tmp1 = fopen64(tmpfile1, "r");
	long ctr = 0, pos = 0;
	std::string factor1 = "";
	std::string factor2 = "";
	while (1) {
		int ret1 = fscanf(tmp1, "%c", &pp);
		int ret2 = fscanf(tmp3, "%c", &ee);
		if (ret1 == EOF) {
			break;
		}
		if (pos == prime_posits[ctr]) {
			std::string b_pp = _bin(pp-'0', 0);
			std::string b_ee = _bin(ee-'0', 1);
			factor1 += b_pp;
			factor2 += b_ee;
			++ctr;
		}
		++pos;
	}
	fclose(tmp1);
	fclose(tmp3);
	unlink(tmpfile3);
	char* _int_factor1 = _int(factor1);
	char* _int_factor2 = _int(factor2);
	char* prod = _product(_int_factor1, _int_factor2);
	if (strcmp(prod, num) == 0) {
                free(prod);
		free(_int_factor1);
		free(_int_factor2);
		return true;
	} else {
                free(prod);
		free(_int_factor1);
		free(_int_factor2);
		return false;
	}
}

int main(int argc, char* argv[]) {
	char* num = strdup(argv[1]);
	long l = strlen(num);
	long pos = 0;
	printf("\nNumber entered was %s\n", num);
	char tmpfile1[L_tmpnam + 1];
	tmpnam(tmpfile1);
	char tmpfile2[L_tmpnam + 1];
	tmpnam(tmpfile2);
	FILE* fp = fopen64("./pi.txt","r");
	FILE* fe = fopen64("./e.txt","r");
	FILE* tmp1 = fopen64(tmpfile1, "w");
	FILE* tmp2 = fopen64(tmpfile2, "w");
	vector<long> prime_posits;
	while (1) {
		char pp = 0, ee = 0;
		fscanf(fp, "%c", &pp);
		fscanf(fe, "%c", &ee);
		fprintf(tmp1, "%c", pp);
		fprintf(tmp2, "%c", ee);
		char nn = num[pos % l];
		char try1[4];
		try1[0] = pp;
		try1[1] = nn;
		try1[2] = ee;
		try1[3] = '\0';
		if (primes[atoi(try1)] == 1) {
			prime_posits.push_back(pos);
			long fpos = ftello(tmp1);
			fclose(tmp1);
			fclose(tmp2);
			bool success = disambiguate(num, l, tmpfile1, tmpfile2, fpos, prime_posits);
			tmp1 = fopen64(tmpfile1, "a");
			tmp2 = fopen64(tmpfile2, "a");
			fseek(tmp1, fpos, SEEK_SET);
		        fseek(tmp2, fpos, SEEK_SET);	
			if (success) {
				break;
			}
		}
		pos++;
	}
	fclose(tmp1);
	fclose(tmp2);
	unlink(tmpfile1);
	unlink(tmpfile2);
	fclose(fp);
	fclose(fe);
	return 0;
}

