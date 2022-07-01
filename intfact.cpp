#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gmp.h>
#include <string>
#include <string>
#include <algorithm>
#include <boost/lexical_cast.hpp>
using namespace std;
using namespace boost;

void* _bin(char* num, long& l_bin) {
	mpz_t nz;
	mpz_init(nz);
	mpz_set_str(nz, num, 10);
	while (mpz_cmp_si(nz, 0) > 0) {
		mpz_div_ui(nz, nz, 2);
		++l_bin;
	}
	mpz_clear(nz);
	return 0;
}

char* _bin(int x) {
	std::string bstring = "";
	if (x == 0) {
		char* r = (char*) calloc(2, sizeof(char));
		r[0] = '0';
		r[1] = '\0';
		return r;
	}
	while (x > 0) {
		int rem = x % 2;
		x /= 2;
		bstring += boost::lexical_cast<std::string>(rem);
	}
	std::reverse(bstring.begin(), bstring.end());
	return strdup((char*) bstring.c_str());
}

char* _int(char* b) {
	mpz_t sum;
	mpz_init(sum);
	mpz_set_si(sum, 0);
	mpz_t term;
	mpz_init(term);
	mpz_t prod;
	mpz_init(prod);
	mpz_set_ui(prod, 1);
	long l = strlen(b);
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

char* product(char* x, char* y) {
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

void* _copy_(char* factor2, long idx, char* bnum, long lb) {
        long cnt = 0;
	for (int i = idx; i >= idx-lb; --i) {
		factor2[idx] = bnum[cnt++];
	}
	return 0;
}

bool disambiguate(char* num, long l, long& ctr, char* tmpfile1, char* tmpfile2) {
	FILE* tmp2 = fopen64(tmpfile2, "r");
	char tmpfile3[L_tmpnam + 1];
	tmpnam(tmpfile3);
	FILE* tmp3 = fopen64(tmpfile3, "w");
	int ret = 0;
	char pp = 0, ee = 0;
	long cnt = 1;
	fseek(tmp2, -cnt, SEEK_END);
	while ((ret = fscanf(tmp2, "%c", &pp)) != EOF) {
		fprintf(tmp3, "%c", pp);
		++cnt;
		fseek(tmp2, -cnt, SEEK_END);
	}
	fclose(tmp2);
	fclose(tmp3);
	tmp3 = fopen64(tmpfile3, "r");
	FILE* tmp1 = fopen64(tmpfile1, "r");
	long _ctr = ctr;
	while (1) {
		fscanf(tmp1, "%c", &pp);
		fscanf(tmp3, "%c", &ee);
		char* ptr1 = strchr((char*)"1357", pp);
		char* ptr2 = strchr((char*)"2468", ee);
		bool isZero1 = (pp == num[ctr]);
		bool isZero2 = (ee == '0');
		if (pp == num[ctr] && ((ptr1 && ptr2) || (ptr1 && isZero2))) { 
			//match found
			++ctr;
		}
	}
	fclose(tmp1);
	fclose(tmp3);
	unlink(tmpfile3);
	if (ctr != _ctr) {
		return true;
	} else {
		return false;
	}
}

int main(int argc, char* argv[]) {
	char* num = strdup(argv[1]);
	long l = strlen(num);
	printf("\nNumber entered was %s\n", num);
	long l_bin = 0, ctr =0;
	_bin(num, l_bin);
	char* factor1 = (char*) calloc(l_bin, sizeof(char));
	char* _factor1 = factor1;
	char* factor2 = (char*) calloc(l_bin, sizeof(char));
	int t = 0, counter = 0;
	long idx1 = 0, idx2 = l_bin - 1;
	char tmpfile1[L_tmpnam + 1];
	tmpnam(tmpfile1);
	char tmpfile2[L_tmpnam + 1];
	tmpnam(tmpfile2);
	FILE* fp = fopen64("./pi.txt","r");
	FILE* fe = fopen64("./e.txt","r");
	FILE* tmp1 = fopen64(tmpfile1, "w");
	FILE* tmp2 = fopen64(tmpfile2, "w");
	while (1) {
		char pp = 0, ee = 0;
		fscanf(fp, "%c", &pp);
		fscanf(fe, "%c", &ee);
		char* ptr1 = strchr((char*)"1357", pp);
		char* ptr2 = strchr((char*)"2468", ee);
		bool isZero1 = (pp == '0');
		bool isZero2 = (ee == '0');
		if ((ptr1 && ptr2) || (ptr1 && isZero2) || (isZero1 && ptr2)) { //already disambiguated
			++counter;
		} else if ((ptr1 && !ptr2) || (!ptr1 && ptr2) || (isZero1 || isZero2)) {
			//ambiguous ; needs to be disambiguated
			fclose(tmp1);
			fclose(tmp2);
			long fpos = ftello(tmp1);
			bool success = disambiguate(num, l, ctr, tmpfile1, tmpfile2);
			tmp1 = fopen64(tmpfile1, "a");
			tmp2 = fopen64(tmpfile2, "a");
			fseek(tmp1, fpos, SEEK_SET);
			fseek(tmp2, fpos, SEEK_SET);
			if (success) {
				char* bnum = _bin(counter);
				long lb = strlen(bnum);
				if (t == 0) {
					strcpy(factor1, bnum);
					factor1 += lb;
				} else if (t == 1) {
					_copy_(factor2, idx2, bnum, lb);
					idx2 -= lb;
				}
				counter = 0;
				t = t - 1;
			}
		}
		char* _int_factor1 = _int(factor1);
		char* _int_factor2 = _int(factor2);
		char* _prod_ = product(_int_factor1, _int_factor2);
		if (strcmp(_prod_, num) == 0) {
			printf("\n%s = %s X %s\n", num, _int_factor1, _int_factor2);
			break;
		}
	}
	fclose(tmp1);
	fclose(tmp2);
	unlink(tmpfile1);
	unlink(tmpfile2);
	fclose(fp);
	fclose(fe);
	return 0;
}

