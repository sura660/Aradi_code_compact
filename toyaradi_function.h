
// Utility Functions

uint8_t rotate4(uint8_t val, int rot) {
    return ((val << rot) | (val >> (4 - rot))) & 0xF;
}

uint8_t rotate8(uint8_t val, int rot) {
    return ((val << rot) | (val >> (8 - rot))) & 0xFF;
}

void print_hex(uint8_t* arr, int len) {
    for (int i = 0; i < len; i++) {
        printf("0x%x ", arr[i]);
    }
    printf("\n");
}


// S-box Functions

void sbox(uint8_t* w, uint8_t* x, uint8_t* y, uint8_t* z) {
    *x ^= (*w & *y);
    *z ^= (*x & *y);
    *y ^= (*w & *z);
    *w ^= (*x & *z);
}

void sbox_inverse(uint8_t* w, uint8_t* x, uint8_t* y, uint8_t* z) {
    *w ^= (*x & *z);
    *y ^= (*w & *z);
    *z ^= (*x & *y);
    *x ^= (*w & *y);
}

// Linear Map Functions

uint8_t linear(int j, uint8_t x) {
    int a[4] = {2, 1, 3, 3};
    int b[4] = {1, 0, 0, 2};
    int c[4] = {3, 2, 2, 0};
    
    uint8_t u = (x >> 4) & 0xF;
    uint8_t l = x & 0xF;
    
    uint8_t s0 = rotate4(u, a[j]);
    uint8_t t0 = rotate4(l, a[j]);
    uint8_t s1 = rotate4(u, b[j]);
    uint8_t t1 = rotate4(l, c[j]);
    
    u ^= s0 ^ t1;
    l ^= t0 ^ s1;
    
    return ((u << 4) | l) & 0xFF;
}

// M0 and M1 Functions

void m0(uint8_t x, uint8_t y, uint8_t* out1, uint8_t* out2) {
    *out1 = rotate8(x, 1) ^ y;
    *out2 = rotate8(y, 3) ^ rotate8(x, 1) ^ y;
}

void m1(uint8_t x, uint8_t y, uint8_t* out1, uint8_t* out2) {
    *out1 = rotate8(x, 3) ^ y;
    *out2 = rotate8(y, 6) ^ rotate8(x, 3) ^ y;
}

// Key Schedule Functions

void keyschedule(uint8_t* key, int i, uint8_t* ki, uint8_t* ki2) {
    uint8_t t0, t1, t2, t3, t4, t5, t6, t7;
    
    m0(key[0], key[1], &t0, &t1);
    m1(key[2], key[3], &t2, &t3);
    m0(key[4], key[5], &t4, &t5);
    m1(key[6], key[7], &t6, &t7);
    
    ki[0] = t0; ki[1] = t2; ki[2] = t1; ki[3] = t3;
    ki[4] = t4; ki[5] = t6; ki[6] = t5; ki[7] = t7 ^ i;
    
    m0(ki[0], ki[1], &t0, &t1);
    m1(ki[2], ki[3], &t2, &t3);
    m0(ki[4], ki[5], &t4, &t5);
    m1(ki[6], ki[7], &t6, &t7);
    
    ki2[0] = t0; ki2[1] = t4; ki2[2] = t2; ki2[3] = t6;
    ki2[4] = t1; ki2[5] = t5; ki2[6] = t3; ki2[7] = t7 ^ (i + 1);
}

void roundkeys(uint8_t* key, uint8_t round_keys[17][4]) {
    uint8_t keys[17][8];
    
    // Initial key
    for (int i = 0; i < 8; i++) {
        keys[0][i] = key[i];
    }
    
    // Generate round keys
    for (int i = 1; i < 16; i += 2) {
        keyschedule(keys[i - 1], i - 1, keys[i], keys[i + 1]);
    }
    
    // Extract 4 blocks for each round key
    for (int i = 0; i < 17; i++) {
        for (int j = 0; j < 4; j++) {
            round_keys[i][j] = (i == 0) ? key[j] : keys[i][j + (i % 2 == 0 ? 0 : 4)];
        }
    }
}

// Encryption and Decryption Functions

void encryption_ARADI(uint8_t* state, uint8_t* key, uint8_t* out) {
    uint8_t round_keys[17][4];
    roundkeys(key, round_keys);
    
    uint8_t w = state[0];
    uint8_t x = state[1];
    uint8_t y = state[2];
    uint8_t z = state[3];
    


//	w ^= round_keys[0][0];
//    x ^= round_keys[0][1];
//    y ^= round_keys[0][2];
//    z ^= round_keys[0][3];

    for (int i = 9; i < 11; i++) {
        
        
        sbox(&w, &x, &y, &z);
        
        int j = i % 4;
        w = linear(j, w);
        x = linear(j, x);
        y = linear(j, y);
        z = linear(j, z);

    
    
    	w ^= round_keys[i+1][0];
    	x ^= round_keys[i+1][1];
    	y ^= round_keys[i+1][2];
    	z ^= round_keys[i+1][3];
    }
    out[0] = w;
    out[1] = x;
    out[2] = y;
    out[3] = z;
}


void decryption_ARADI(uint8_t* state, uint8_t* key, uint8_t* out) {
    uint8_t round_keys[17][4];
    roundkeys(key, round_keys);
    
    uint8_t w = state[0];
    uint8_t x = state[1];
    uint8_t y = state[2];
    uint8_t z = state[3];
    
    w ^= round_keys[16][0];
    x ^= round_keys[16][1];
    y ^= round_keys[16][2];
    z ^= round_keys[16][3];
    
    for (int i = 15; i >= 0; i--) {
        int j = i % 4;
        
        w = linear(j, w);
        x = linear(j, x);
        y = linear(j, y);
        z = linear(j, z);
        
        sbox_inverse(&w, &x, &y, &z);
        
        w ^= round_keys[i][0];
        x ^= round_keys[i][1];
        y ^= round_keys[i][2];
        z ^= round_keys[i][3];
    }
    
    out[0] = w;
    out[1] = x;
    out[2] = y;
    out[3] = z;
}


