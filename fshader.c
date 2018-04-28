#version 300 es

precision highp float;
precision highp int;

out vec4 o_color;
in float v_percent;

//[
float U_THREAD;
uint U_INPUT_0;
uint U_INPUT_1;
uint U_INPUT_2;
uint U_ITER;
//]

uniform uint u_start;


vec4 int2rgba(const int i) {
    ivec4 i4;
    i4.r = i >> 24 & 0xFF;
    i4.g = i >> 16 & 0xFF;
    i4.b = i >>  8 & 0xFF;
    i4.a = i >>  0 & 0xFF;

    vec4 f4;
    f4.r = float(i4.r) / 255.;
    f4.g = float(i4.g) / 255.;
    f4.b = float(i4.b) / 255.;
    f4.a = float(i4.a) / 255.;
    return f4;
}


/* Elementary functions used by SHA256 */
#define Ch(x, y, z) ((x & (y ^ z)) ^ z)
#define Maj(x, y, z)((x & (y | z)) | (y & z))
#define SHR(x, n)   (x >> n)
#define ROTR(x, n)  ((x >> n) | (x << (32 - n)))
#define S0(x)       (ROTR(x, 2) ^ ROTR(x, 13) ^ ROTR(x, 22))
#define S1(x)       (ROTR(x, 6) ^ ROTR(x, 11) ^ ROTR(x, 25))
#define s0(x)       (ROTR(x, 7) ^ ROTR(x, 18) ^ SHR(x, 3))
#define s1(x)       (ROTR(x, 17) ^ ROTR(x, 19) ^ SHR(x, 10))

/* SHA256 round function */
#define RND(a, b, c, d, e, f, g, h, k)     \
    t0 = h + S1(e) + Ch(e, f, g) + k;      \
    t1 = S0(a) + Maj(a, b, c);             \
    d += t0;                               \
    h  = t0 + t1;



void main() {
    float p = (U_ITER * U_THREAD) * v_percent - U_ITER / 2.0;
    uint base = u_start + uint(p);


    uint W64;
    //
    // uint8_t [64]
    // |0  ~ 16| = DATA
    // |16     | = 0x80
    // |17 ~ 56| = 0
    // |56 ~ 64| = len64bit << 3
    // 
    // uint32_t[16]
    // |0  ~  4| = DATA
    // |4      | = 0x80000000
    // |5  ~ 15| = 0
    // |15 ~ 16| = len64bit << 3
    //

    uint W00 = U_INPUT_0;
    uint W01 = U_INPUT_1;
    uint W02 = U_INPUT_2;
    uint W03;
    uint W04 = 0x80000000u;
    uint W05 = 0u;
    uint W06 = 0u;
    uint W07 = 0u;
    uint W08 = 0u;
    uint W09 = 0u;
    uint W10 = 0u;
    uint W11 = 0u;
    uint W12 = 0u;
    uint W13 = 0u;
    uint W14 = 0u;
    uint W15 = 128u;

    // SHA256_Transform
    uint t0, t1;

    for (uint i = uint(U_ITER) - 1u; i != -1u; i--) {
        uint nonce = base + i;
        W03 = nonce;

        /* 1. Prepare message schedule W. */
        uint W16 = s1(W14) + W09 + s0(W01) + W00;
        uint W17 = s1(W15) + W10 + s0(W02) + W01;
        uint W18 = s1(W16) + W11 + s0(W03) + W02;
        uint W19 = s1(W17) + W12 + s0(W04) + W03;
        uint W20 = s1(W18) + W13 + s0(W05) + W04;
        uint W21 = s1(W19) + W14 + s0(W06) + W05;
        uint W22 = s1(W20) + W15 + s0(W07) + W06;
        uint W23 = s1(W21) + W16 + s0(W08) + W07;
        uint W24 = s1(W22) + W17 + s0(W09) + W08;
        uint W25 = s1(W23) + W18 + s0(W10) + W09;
        uint W26 = s1(W24) + W19 + s0(W11) + W10;
        uint W27 = s1(W25) + W20 + s0(W12) + W11;
        uint W28 = s1(W26) + W21 + s0(W13) + W12;
        uint W29 = s1(W27) + W22 + s0(W14) + W13;
        uint W30 = s1(W28) + W23 + s0(W15) + W14;
        uint W31 = s1(W29) + W24 + s0(W16) + W15;
        uint W32 = s1(W30) + W25 + s0(W17) + W16;
        uint W33 = s1(W31) + W26 + s0(W18) + W17;
        uint W34 = s1(W32) + W27 + s0(W19) + W18;
        uint W35 = s1(W33) + W28 + s0(W20) + W19;
        uint W36 = s1(W34) + W29 + s0(W21) + W20;
        uint W37 = s1(W35) + W30 + s0(W22) + W21;
        uint W38 = s1(W36) + W31 + s0(W23) + W22;
        uint W39 = s1(W37) + W32 + s0(W24) + W23;
        uint W40 = s1(W38) + W33 + s0(W25) + W24;
        uint W41 = s1(W39) + W34 + s0(W26) + W25;
        uint W42 = s1(W40) + W35 + s0(W27) + W26;
        uint W43 = s1(W41) + W36 + s0(W28) + W27;
        uint W44 = s1(W42) + W37 + s0(W29) + W28;
        uint W45 = s1(W43) + W38 + s0(W30) + W29;
        uint W46 = s1(W44) + W39 + s0(W31) + W30;
        uint W47 = s1(W45) + W40 + s0(W32) + W31;
        uint W48 = s1(W46) + W41 + s0(W33) + W32;
        uint W49 = s1(W47) + W42 + s0(W34) + W33;
        uint W50 = s1(W48) + W43 + s0(W35) + W34;
        uint W51 = s1(W49) + W44 + s0(W36) + W35;
        uint W52 = s1(W50) + W45 + s0(W37) + W36;
        uint W53 = s1(W51) + W46 + s0(W38) + W37;
        uint W54 = s1(W52) + W47 + s0(W39) + W38;
        uint W55 = s1(W53) + W48 + s0(W40) + W39;
        uint W56 = s1(W54) + W49 + s0(W41) + W40;
        uint W57 = s1(W55) + W50 + s0(W42) + W41;
        uint W58 = s1(W56) + W51 + s0(W43) + W42;
        uint W59 = s1(W57) + W52 + s0(W44) + W43;
        uint W60 = s1(W58) + W53 + s0(W45) + W44;
        uint W61 = s1(W59) + W54 + s0(W46) + W45;
        uint W62 = s1(W60) + W55 + s0(W47) + W46;
        uint W63 = s1(W61) + W56 + s0(W48) + W47;

        /* 2. Initialize working variables. */
        uint S0 = 0x6A09E667u;
        uint S1 = 0xBB67AE85u;
        uint S2 = 0x3C6EF372u;
        uint S3 = 0xA54FF53Au;
        uint S4 = 0x510E527Fu;
        uint S5 = 0x9B05688Cu;
        uint S6 = 0x1F83D9ABu;
        uint S7 = 0x5BE0CD19u;

        /* 3. Mix. */
        RND(S0, S1, S2, S3, S4, S5, S6, S7, W00 + 0x428A2F98u);
        RND(S7, S0, S1, S2, S3, S4, S5, S6, W01 + 0x71374491u);
        RND(S6, S7, S0, S1, S2, S3, S4, S5, W02 + 0xB5C0FBCFu);
        RND(S5, S6, S7, S0, S1, S2, S3, S4, W03 + 0xE9B5DBA5u);
        RND(S4, S5, S6, S7, S0, S1, S2, S3, W04 + 0x3956C25Bu);
        RND(S3, S4, S5, S6, S7, S0, S1, S2, W05 + 0x59F111F1u);
        RND(S2, S3, S4, S5, S6, S7, S0, S1, W06 + 0x923F82A4u);
        RND(S1, S2, S3, S4, S5, S6, S7, S0, W07 + 0xAB1C5ED5u);
        RND(S0, S1, S2, S3, S4, S5, S6, S7, W08 + 0xD807AA98u);
        RND(S7, S0, S1, S2, S3, S4, S5, S6, W09 + 0x12835B01u);
        RND(S6, S7, S0, S1, S2, S3, S4, S5, W10 + 0x243185BEu);
        RND(S5, S6, S7, S0, S1, S2, S3, S4, W11 + 0x550C7DC3u);
        RND(S4, S5, S6, S7, S0, S1, S2, S3, W12 + 0x72BE5D74u);
        RND(S3, S4, S5, S6, S7, S0, S1, S2, W13 + 0x80DEB1FEu);
        RND(S2, S3, S4, S5, S6, S7, S0, S1, W14 + 0x9BDC06A7u);
        RND(S1, S2, S3, S4, S5, S6, S7, S0, W15 + 0xC19BF174u);
        RND(S0, S1, S2, S3, S4, S5, S6, S7, W16 + 0xE49B69C1u);
        RND(S7, S0, S1, S2, S3, S4, S5, S6, W17 + 0xEFBE4786u);
        RND(S6, S7, S0, S1, S2, S3, S4, S5, W18 + 0x0FC19DC6u);
        RND(S5, S6, S7, S0, S1, S2, S3, S4, W19 + 0x240CA1CCu);
        RND(S4, S5, S6, S7, S0, S1, S2, S3, W20 + 0x2DE92C6Fu);
        RND(S3, S4, S5, S6, S7, S0, S1, S2, W21 + 0x4A7484AAu);
        RND(S2, S3, S4, S5, S6, S7, S0, S1, W22 + 0x5CB0A9DCu);
        RND(S1, S2, S3, S4, S5, S6, S7, S0, W23 + 0x76F988DAu);
        RND(S0, S1, S2, S3, S4, S5, S6, S7, W24 + 0x983E5152u);
        RND(S7, S0, S1, S2, S3, S4, S5, S6, W25 + 0xA831C66Du);
        RND(S6, S7, S0, S1, S2, S3, S4, S5, W26 + 0xB00327C8u);
        RND(S5, S6, S7, S0, S1, S2, S3, S4, W27 + 0xBF597FC7u);
        RND(S4, S5, S6, S7, S0, S1, S2, S3, W28 + 0xC6E00BF3u);
        RND(S3, S4, S5, S6, S7, S0, S1, S2, W29 + 0xD5A79147u);
        RND(S2, S3, S4, S5, S6, S7, S0, S1, W30 + 0x06CA6351u);
        RND(S1, S2, S3, S4, S5, S6, S7, S0, W31 + 0x14292967u);
        RND(S0, S1, S2, S3, S4, S5, S6, S7, W32 + 0x27B70A85u);
        RND(S7, S0, S1, S2, S3, S4, S5, S6, W33 + 0x2E1B2138u);
        RND(S6, S7, S0, S1, S2, S3, S4, S5, W34 + 0x4D2C6DFCu);
        RND(S5, S6, S7, S0, S1, S2, S3, S4, W35 + 0x53380D13u);
        RND(S4, S5, S6, S7, S0, S1, S2, S3, W36 + 0x650A7354u);
        RND(S3, S4, S5, S6, S7, S0, S1, S2, W37 + 0x766A0ABBu);
        RND(S2, S3, S4, S5, S6, S7, S0, S1, W38 + 0x81C2C92Eu);
        RND(S1, S2, S3, S4, S5, S6, S7, S0, W39 + 0x92722C85u);
        RND(S0, S1, S2, S3, S4, S5, S6, S7, W40 + 0xA2BFE8A1u);
        RND(S7, S0, S1, S2, S3, S4, S5, S6, W41 + 0xA81A664Bu);
        RND(S6, S7, S0, S1, S2, S3, S4, S5, W42 + 0xC24B8B70u);
        RND(S5, S6, S7, S0, S1, S2, S3, S4, W43 + 0xC76C51A3u);
        RND(S4, S5, S6, S7, S0, S1, S2, S3, W44 + 0xD192E819u);
        RND(S3, S4, S5, S6, S7, S0, S1, S2, W45 + 0xD6990624u);
        RND(S2, S3, S4, S5, S6, S7, S0, S1, W46 + 0xF40E3585u);
        RND(S1, S2, S3, S4, S5, S6, S7, S0, W47 + 0x106AA070u);
        RND(S0, S1, S2, S3, S4, S5, S6, S7, W48 + 0x19A4C116u);
        RND(S7, S0, S1, S2, S3, S4, S5, S6, W49 + 0x1E376C08u);
        RND(S6, S7, S0, S1, S2, S3, S4, S5, W50 + 0x2748774Cu);
        RND(S5, S6, S7, S0, S1, S2, S3, S4, W51 + 0x34B0BCB5u);
        RND(S4, S5, S6, S7, S0, S1, S2, S3, W52 + 0x391C0CB3u);
        RND(S3, S4, S5, S6, S7, S0, S1, S2, W53 + 0x4ED8AA4Au);
        RND(S2, S3, S4, S5, S6, S7, S0, S1, W54 + 0x5B9CCA4Fu);
        RND(S1, S2, S3, S4, S5, S6, S7, S0, W55 + 0x682E6FF3u);
        RND(S0, S1, S2, S3, S4, S5, S6, S7, W56 + 0x748F82EEu);
        RND(S7, S0, S1, S2, S3, S4, S5, S6, W57 + 0x78A5636Fu);
        RND(S6, S7, S0, S1, S2, S3, S4, S5, W58 + 0x84C87814u);
        RND(S5, S6, S7, S0, S1, S2, S3, S4, W59 + 0x8CC70208u);
        RND(S4, S5, S6, S7, S0, S1, S2, S3, W60 + 0x90BEFFFAu);
        RND(S3, S4, S5, S6, S7, S0, S1, S2, W61 + 0xA4506CEBu);
        RND(S2, S3, S4, S5, S6, S7, S0, S1, W62 + 0xBEF9A3F7u);
        RND(S1, S2, S3, S4, S5, S6, S7, S0, W63 + 0xC67178F2u);

        /* 4. Mix local working variables into global state. */
        uint state0;
        state0 = 0x6A09E667u + S0;
        // state1 = 0xBB67AE85u + S1;
        // state2 = 0x3C6EF372u + S2;
        // state3 = 0xA54FF53Au + S3;
        // state4 = 0x510E527Fu + S4;
        // state5 = 0x9B05688Cu + S5;
        // state6 = 0x1F83D9ABu + S6;
        // state7 = 0x5BE0CD19u + S7;

        if ((state0 & 0xFFFFFFFFu) == 0u) {
            o_color = int2rgba( int(nonce) );
            return;
        }
    }

    o_color = vec4(1., 1., 1., 1.);
}
