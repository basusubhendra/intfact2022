#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gmp.h>
#include <string>
#include <algorithm>
#include <boost/lexical_cast.hpp>
using namespace std;
using namespace boost;

//returns the length of the binary
//equivalent string of the number
//to be factored.  Note that the 
//binary value per se is not used 
//here.
void* _bin(char* num, long& l_bin) {
	mpz_t nz;
	mpz_init(nz);
	mpz_set_str(nz, num, 10);
	while (mpz_cmp_si(nz, 0) > 0) {
		mpz_div_ui(nz, nz, 2);
		++l_bin; //pass by reference
	}
	mpz_clear(nz);
	return 0;
}

//convert a long integer input
//to binary string.
char* _bin(long x) {
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

//convert a binary string to its 
//arbitrary precision integer equivalent
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

//return the product of two
//arbitrary precision integers
//x and y
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

//copy bnum bit by bit from 0 to lb
//onto factor2 indices idx downto idx-lb 
//the right bound being exclusive.
void* _copy_(char* factor2, long idx, char* bnum, long lb) {
        long cnt = 0;
	for (int i = idx; i > idx-lb; --i) {
		factor2[idx] = bnum[cnt++];
	}
	return 0;
}

//This function ``searches'' for the digits of 
//the number to be factored after reversing the 
//digits of e when an ambiguous (odd-odd ;
//even-even point) is encountered.
//In case, a valid unambigous (even-odd;
//odd-even) point of 
//the number to be factored is found 
//after reversing the digits of e,
//the ambiguous point is said to be 
//dis-ambiguated.
bool disambiguate(char* num, long l, long& ctr, char* tmpfile1, char* tmpfile2, long fpos) {
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
	long _ctr = ctr;
	while (1) {
		int ret1 = fscanf(tmp1, "%c", &pp);
		int ret2 = fscanf(tmp3, "%c", &ee);
		if (ret1 == EOF) {
			break;
		}
		bool ptr1 = (strchr((char*)"13579", pp) != NULL);
		bool _ptr1 = (strchr((char*)"2468", pp) != NULL);
		bool ptr2 = (strchr((char*)"2468", ee) != NULL);
		bool _ptr2 = (strchr((char*)"13579", ee) != NULL);
		bool isZero1 = (pp == num[ctr]);
		bool isZero2 = (ee == '0');
		if (isZero1 && ((ptr1 && ptr2) || (_ptr1 && _ptr2) || ((ptr1 || _ptr1) && isZero2))) { 
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
	//get the length of the binary representation
	//of num while discarding the representation
	//itself.
	_bin(num, l_bin);
	char* factor1 = (char*) calloc(l_bin, sizeof(char));
	char* _factor1 = factor1;
					factor1[l_bin] = '\0';
	char* factor2 = (char*) calloc(l_bin, sizeof(char));
					factor2[l_bin] = '\0';
	int t = 0;
        long counter = 0;
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
		fprintf(tmp1, "%c", pp);
		fprintf(tmp2, "%c", ee);
		bool ptr1 = (strchr((char*)"13579", pp) != NULL);
		bool _ptr1 = (strchr((char*)"2468", pp) != NULL);
		bool ptr2 = (strchr((char*)"2468", ee) != NULL);
		bool _ptr2 = (strchr((char*)"13579", ee) != NULL);
		bool isZero1 = (pp == '0');
		bool isZero2 = (ee == '0');
		if ((ptr1 && ptr2) || (_ptr1 && _ptr2) || ((ptr1 || _ptr1) && isZero2) || (isZero1 && (ptr2 || _ptr2))) { //already disambiguated
			++counter;
		} else if ((ptr1 && _ptr2) || (_ptr1 && ptr2) || (isZero1 && isZero2)) {
			//ambiguous ; needs to be disambiguated
			fclose(tmp1);
			fclose(tmp2);
			long fpos = ftello(tmp1);
			bool success = disambiguate(num, l, ctr, tmpfile1, tmpfile2, fpos);
			tmp1 = fopen64(tmpfile1, "a");
			tmp2 = fopen64(tmpfile2, "a");
			fseek(tmp1, fpos, SEEK_SET);
			fseek(tmp2, fpos, SEEK_SET);
			if (success) {
				//convert to binary
				char* bnum = _bin(counter);
				long lb = strlen(bnum);
				if (t == 0) {
					//direct copy
					strcpy(factor1+idx1, bnum);
					idx1 += lb;
				} else if (t == 1) {
					_copy_(factor2, idx2, bnum, lb);
					idx2 -= lb;
				}
				//reset for the next cycle
				counter = 0;
				t = 1 - t;
			}
		}
		char* _int_factor1 = _int(_factor1);
		char* _int_factor2 = _int(factor2+idx2);
		char* _prod_ = product(_int_factor1, _int_factor2);
		cout << _prod_ << endl;
		cout << _int_factor1 << endl;
		cout << _int_factor2 << endl;
		system("a=1;read a");
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

