#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include "functions.h"

// Main Function
int main() {
	srand(time(NULL));
	uint32_t Delta[4] = {0x10800010, 0x00000000, 0x00000000, 0x00000000};
	uint32_t Delta1[4] = {0xef7fffef,0xef7fffef, 0xef7fffef, 0xef7fffef};
    uint32_t key[8] = {0x03020100, 0x07060504, 0x0b0a0908, 0x0f0e0d0c, 
                       0x13121110, 0x17161514, 0x1b1a1918, 0x1f1e1d1c};

	int non_zero[3] = {4,23,28};
	int cntt;
	uint32_t key_d[4] = {0x12345678,0x98765430,0x17359257,0x19263540};
	uint8_t diff_8[6] = {0x5,0x7,0x8,0xa,0xd,0xf};
	uint64_t tcount=0,count;
    uint32_t p1[4], deltap[4] ;
	uint32_t p2[4] ;
    uint32_t c1[4],c2[4],c11[4],c22[4], deciphered[4], delta_c[4], delta_cc[4];
	uint32_t out[4] = {0x00000000, 0x00000000, 0x00000000, 0x00000000};
	uint8_t temp[3];
	int col9[9] = {0,3,8,9,14,17,26,27,31};
	int col3[3] = {3,8,27};
	int loop;

	int flag;

	for(loop = 0; loop < (1); loop++){
		count=0;
		flag = 0;
		while(flag == 0){
			for(int i=0; i < 4;i++){	
				p1[i] = rand() & 0xffffffff;
				p2[i] = rand() & 0xffffffff;
				if(p1[i] ^ p2[i] != Delta[i])
					flag = 1;
				}
			}

//		for(int i = 0; i < 4; i++)
//			deltap[i] = p1[i] ^ p2[i];

//		print_hex(deltap,4);
//		print_hex(p2,4);
		printf("\n");

		encryption_ARADI(p1, key, c1);
		encryption_ARADI(p2, key, c2);
			
//		print_hex(c1,4);
//		print_hex(c2,4);



//		sbox(&c1[0], &c1[1], &c1[2], &c1[3]);
//		sbox(&c2[0], &c2[1], &c2[2], &c2[3]);



//		for(int i = 0; i <4 ;i++){
//			c1[i] = c1[i] ^ key_d[i]; 
//			c2[i] = c2[i] ^ key_d[i];			
//			}


//		print_hex(c1,4);
//		print_hex(c2,4);



		for(int i = 0; i < 4; i++){
			c11[i] = c1[i];
			c22[i] = c2[i];
			}
//		for(int i=0; i < 4;i++) printf("0x%.8x ",c1[i]^c2[i]);


		for(uint64_t gkey = 0 ; gkey < ((uint64_t)(1)<<36) ; gkey++){
			if(gkey%(1<<30) == 0){
				printf("%ld\n",gkey);
				printf("\n\t\tcount = %ld \n\n",count);
				}

			for(int i=0; i < 4;i++) {
				c1[i] = c11[i];
				c2[i] = c22[i];
				}



			c1[0] = linear(2, c1[0]);
		    c1[1] = linear(2, c1[1]);
		    c1[2] = linear(2, c1[2]);
		    c1[3] = linear(2, c1[3]);

			c2[0] = linear(2, c2[0]);
		    c2[1] = linear(2, c2[1]);
		    c2[2] = linear(2, c2[2]);
		    c2[3] = linear(2, c2[3]);


			for(int i=0; i < 4;i++) delta_c[i] = c1[i]^c2[i];

//			print_hex(delta_c,4);


//			printf("%ld\n",gkey);
			for(int i=0; i < 4;i++) {
				for(int j = 0; j < 9; j++){
					c1[i] = c1[i] ^ (((gkey >> 9*i+j)&1) << (31-col9[j])); 
					c2[i] = c2[i] ^ (((gkey >> 9*i+j)&1) << (31-col9[j]));
//					printf("%ld",(((gkey >> 9*i+j)&1) ));
					}
				
				}
//				printf("\n");



	

//			print_hex(c1,4);
//			print_hex(c2,4);


			sbox_inverse(&c1[0], &c1[1], &c1[2], &c1[3]);
			sbox_inverse(&c2[0], &c2[1], &c2[2], &c2[3]);

//			print_hex(c1,4);
//			print_hex(c2,4);



			for(int i=0; i < 4;i++) delta_c[i] = c1[i]^c2[i];

			delta_c[0] = linear(1, delta_c[0]);
		    delta_c[1] = linear(1, delta_c[1]);
		    delta_c[2] = linear(1, delta_c[2]);
		    delta_c[3] = linear(1, delta_c[3]);



//			print_hex(delta_c,4);
//			print_hex(c2,4);

			int cnt = 0;
			for(int i=0; i < 4;i++){ 
				delta_cc[i] = delta_c[i] & Delta1[i];
				if(delta_cc[i] == 0)
					cnt++;
				}
			if(cnt == 4){
				for(int i = 0; i < 3; i++)
					temp[i] = (((delta_c[0] >> non_zero[i])&1)<<3) ^ (((delta_c[1] >> non_zero[i])&1)<<2) ^ (((delta_c[2] >> non_zero[i])&1)<<1) ^ ((delta_c[3] >> non_zero[i])&1) ;
				cntt = 0;
				for(int i = 0; i < 6; i++){ 
					if(temp[0] == diff_8[i])
						for(int ii = 0; ii < 6; ii++)
							if(temp[1] == diff_8[ii])
								for(int iii = 0; iii < 6; iii++)
									if(temp[2] == diff_8[iii]){
										cntt++;
										break;
										}
 					}
				if(cntt == 1){
					count++;
					}				
				}
//			print_hex(delta_c,4);
			}
		printf("\n\t\tTotal count = %ld \n\n",count);
		tcount = tcount + count;
		}

	printf("\n\t\tAverage count = %ld \n\n",tcount/loop);

//		encryption_ARADI(p1, key, c1);
//		encryption_ARADI(p2, key, c2);

/*
		for(int i = 0; i < 4; i++) out[i] = out[i] | (c1[i] ^ c2[i]);
		}
    printf("\n\n\n\nOutput:\t");
    print_hex(out, 4);
	int x,cnt=0;
	printf("\n");
	for(int i = 0; i < 4; i++){
		for(int j = 0; j < 32; j++){
			x = (out[i] >> (31-j)) & 1;
			if(x == 1)	cnt++;
			printf("%d",x);
			}
		}
	printf("\n");

	printf("Total number of unknown position %d\n",cnt);

    
    encryption_ARADI(plaintext, key, ciphertext);
    printf("Output:\t");
    print_hex(ciphertext, 4);
    
    decryption_ARADI(ciphertext, key, deciphered);
    printf("Dec.:\t");
    print_hex(deciphered, 4);
*/    
    return 0;
}

