// BC1 blocks rewritten as ASTC 4x4 blocks, for GPUs without BC support (iPhones).
//
// A BC1 block is two RGB565 endpoints and a 2-bit index per texel; an ASTC 4x4 block can hold the
// same two endpoints and a weight per texel, so each block converts on its own, with no image search:
//  - four-colour blocks (c0 > c1): RGB endpoints at 8 bits, 2-bit weights 0, 1/3, 2/3, 1
//    (ASTC's 21/64 and 43/64 for BC1's thirds: within one step of 255);
//  - three-colour blocks without transparency: 3-level weights 0, 1/2, 1 (exact midpoint);
//  - three-colour blocks with transparent texels: two weight planes, colour and alpha, with
//    7-bit RGBA endpoints (every remaining bit is used).
// ASTC is 8 bits per texel: half of the 16-bit A1BGR5 copy and a quarter of RGBA8.
#include "BC1ToASTC.h"
#include <stdlib.h>
#include <string.h>

typedef struct { uint8_t bytes[16]; } Block;

static void putBits(uint8_t *b, unsigned at, unsigned count, uint32_t value) {
	for (unsigned i = 0; i < count; i++) if (value >> i & 1) b[(at + i) >> 3] |= (uint8_t)(1u << ((at + i) & 7));
}

// The ASTC integer sequence for trits: five values in 8 bits. Built once by running the spec's decoder
// over every 8-bit pattern and keeping the first pattern for each combination.
static uint8_t tritPack[243];
static void buildTrits(void) {
	static int built;
	if (built) return;
	uint8_t seen[243] = {0};
	for (unsigned T = 0; T < 256; T++) {
		unsigned C, t[5];
		if (((T >> 2) & 7) == 7) { C = ((T >> 5) & 7) << 2 | (T & 3); t[4] = 2; t[3] = 2; }
		else { C = T & 31; if (((T >> 5) & 3) == 3) { t[4] = 2; t[3] = T >> 7 & 1; } else { t[4] = T >> 7 & 1; t[3] = T >> 5 & 3; } }
		if ((C & 3) == 3) { t[2] = 2; t[1] = C >> 4 & 1; t[0] = (C >> 3 & 1) << 1 | ((C >> 2 & 1) & ~(C >> 3) & 1); }
		else if (((C >> 2) & 3) == 3) { t[2] = 2; t[1] = 2; t[0] = C & 3; }
		else { t[2] = C >> 4 & 1; t[1] = C >> 2 & 3; t[0] = (C >> 1 & 1) << 1 | ((C & 1) & ~(C >> 1) & 1); }
		if (t[0] > 2 || t[1] > 2 || t[2] > 2 || t[3] > 2 || t[4] > 2) continue;
		unsigned key = t[0] + 3 * (t[1] + 3 * (t[2] + 3 * (t[3] + 3 * t[4])));
		if (!seen[key]) { seen[key] = 1; tritPack[key] = (uint8_t)T; }
	}
	built = 1;
}

// Weights (values 0..2, 3-level range) as an integer sequence, written from bit 127 downwards.
static void putTritWeights(uint8_t *b, const uint8_t *w, unsigned count) {
	unsigned bit = 0;
	uint8_t stream[16] = {0};
	for (unsigned g = 0; g < count; g += 5) {
		unsigned n = count - g < 5 ? count - g : 5, key = 0, mul = 1;
		for (unsigned i = 0; i < 5; i++, mul *= 3) key += (i < n ? w[g + i] : 0) * mul;
		static const unsigned bitsFor[6] = {0, 2, 4, 5, 7, 8};
		putBits(stream, bit, bitsFor[n], tritPack[key]);
		bit += bitsFor[n];
	}
	for (unsigned i = 0; i < bit; i++) if (stream[i >> 3] >> (i & 7) & 1) b[(127 - i) >> 3] |= (uint8_t)(1u << ((127 - i) & 7));
}

static uint8_t quant7(unsigned v8) {		// best 7-bit endpoint: decodes as v << 1 | v >> 6
	int best = 0, bestError = 1000;
	for (int v = (int)(v8 >> 1) - 1; v <= (int)(v8 >> 1) + 1; v++) {
		if (v < 0 || v > 127) continue;
		int e = abs((v << 1 | v >> 6) - (int)v8);
		if (e < bestError) { bestError = e; best = v; }
	}
	return (uint8_t)best;
}

void PGBC1BlockToASTC(const uint8_t bc1[8], uint8_t astc[16]) {
	buildTrits();
	unsigned c0 = bc1[0] | bc1[1] << 8, c1 = bc1[2] | bc1[3] << 8;
	uint32_t indices = bc1[4] | bc1[5] << 8 | bc1[6] << 16 | (uint32_t)bc1[7] << 24;
	unsigned e[2][3] = {
		{(((c0 >> 11) & 31) * 527 + 23) >> 6, (((c0 >> 5) & 63) * 259 + 33) >> 6, ((c0 & 31) * 527 + 23) >> 6},
		{(((c1 >> 11) & 31) * 527 + 23) >> 6, (((c1 >> 5) & 63) * 259 + 33) >> 6, ((c1 & 31) * 527 + 23) >> 6}};
	memset(astc, 0, 16);
	bool fourColour = c0 > c1, transparent = false;
	if (!fourColour) for (unsigned i = 0; i < 16; i++) if ((indices >> 2 * i & 3) == 3) transparent = true;
	if (fourColour) {
		// Block mode 0x042: 4x4 weights, range 0..3, one plane. CEM 8 (RGB direct), endpoints at 8 bits.
		static const uint8_t weightOf[4] = {0, 3, 1, 2};
		bool swap = e[1][0] + e[1][1] + e[1][2] < e[0][0] + e[0][1] + e[0][2];	// keep ASTC's blue contraction off
		putBits(astc, 0, 11, 0x042); putBits(astc, 13, 4, 8);
		for (unsigned c = 0; c < 3; c++) { putBits(astc, 17 + 16 * c, 8, e[swap][c]); putBits(astc, 25 + 16 * c, 8, e[!swap][c]); }
		for (unsigned i = 0; i < 16; i++) {
			unsigned w = weightOf[indices >> 2 * i & 3]; if (swap) w = 3 - w;
			for (unsigned k = 0; k < 2; k++) if (w >> k & 1) { unsigned at = 127 - (2 * i + k); astc[at >> 3] |= (uint8_t)(1u << (at & 7)); }
		}
		return;
	}
	static const uint8_t weightOf[4] = {0, 2, 1, 0};		// c0, c1, midpoint; the transparent texel's colour doesn't matter
	bool swap = e[1][0] + e[1][1] + e[1][2] < e[0][0] + e[0][1] + e[0][2];
	if (!transparent) {
		// Block mode 0x051: 4x4 weights, range 0..2, one plane. CEM 8, endpoints at 8 bits.
		putBits(astc, 0, 11, 0x051); putBits(astc, 13, 4, 8);
		for (unsigned c = 0; c < 3; c++) { putBits(astc, 17 + 16 * c, 8, e[swap][c]); putBits(astc, 25 + 16 * c, 8, e[!swap][c]); }
		uint8_t w[16];
		for (unsigned i = 0; i < 16; i++) { w[i] = weightOf[indices >> 2 * i & 3]; if (swap) w[i] = 2 - w[i]; }
		putTritWeights(astc, w, 16);
		return;
	}
	// Block mode 0x451: two planes (colour, then alpha: CCS 3), range 0..2. CEM 12 (RGBA direct), 7-bit endpoints.
	uint8_t q[2][4];
	for (unsigned k = 0; k < 2; k++) for (unsigned c = 0; c < 3; c++) q[k][c] = quant7(e[k][c]);
	q[0][3] = 127; q[1][3] = 0;		// alpha: endpoint 0 opaque, endpoint 1 transparent
	unsigned s0 = 0, s1 = 0;
	for (unsigned c = 0; c < 3; c++) { s0 += q[0][c] << 1 | q[0][c] >> 6; s1 += q[1][c] << 1 | q[1][c] >> 6; }
	swap = s1 < s0;
	putBits(astc, 0, 11, 0x451); putBits(astc, 13, 4, 12);
	for (unsigned c = 0; c < 4; c++) { putBits(astc, 17 + 14 * c, 7, q[swap][c]); putBits(astc, 24 + 14 * c, 7, q[!swap][c]); }
	uint8_t w[32];
	for (unsigned i = 0; i < 16; i++) {
		unsigned index = indices >> 2 * i & 3, colour = weightOf[index], alpha = index == 3 ? 2 : 0;
		if (swap) { colour = 2 - colour; alpha = 2 - alpha; }
		w[2 * i] = (uint8_t)colour; w[2 * i + 1] = (uint8_t)alpha;
	}
	putTritWeights(astc, w, 32);
	putBits(astc, 128 - 52 - 2, 2, 3);		// colour component selector, right below the 52 weight bits
}

void *PGTranscodeBC1ToASTC(uint32_t w, uint32_t h, uint32_t d, uint32_t bpr, uint32_t rpi, const void *data, size_t size,
                           size_t *outSize, uint32_t *outBpr, uint32_t *outRpi) {
	uint32_t bw = (w + 3) / 4, bh = (h + 3) / 4, imageRows = rpi ? rpi : bh;
	size_t pitch = (size_t)bw * 16, image = pitch * bh;
	if ((size_t)bw * 8 > bpr || (size_t)bpr * imageRows * (d - 1) + (size_t)bpr * bh > size) return NULL;
	uint8_t *out = malloc(image * d);
	if (!out) return NULL;
	for (uint32_t z = 0; z < d; z++) for (uint32_t y = 0; y < bh; y++) {
		const uint8_t *row = (const uint8_t *)data + (size_t)z * bpr * imageRows + (size_t)y * bpr;
		for (uint32_t x = 0; x < bw; x++) PGBC1BlockToASTC(row + 8 * x, out + z * image + y * pitch + 16 * x);
	}
	*outSize = image * d; *outBpr = (uint32_t)pitch; *outRpi = bh;
	return out;
}
