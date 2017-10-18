#include <stdint.h>

#define CR "\n\t"

void sec_rectangle_v02(uint16_t *buffer, uint32_t rk_masked)
{
	/* buffer defined as follow:
		[0]: row0v, [1]:row1v, [2]: row2v, [3]: row3v
		[4]: row0m, [5]:row1m, [6]: row2m, [7]: row3m
		[8]: random, [9]: random
	*/
	
	asm volatile (
	/* Registers allocation:
		r2 : row0v
		r3 : row1v
		r4 : row2v
		r5 : row3v
		r6 : row0m
		r7 : row1m
		r8 : row2m
		r9 : row3m
		r10: rkv/temp
		r11: rkm/temp
		r12: random a
		r14: loop counter
	 */
	/* load block */
	"ldm %[buffer], {r2, r4, r6, r8, r12}" CR
	"mov r3, r2, lsr #16" CR
	"mov r5, r4, lsr #16" CR
	"mov r7, r6, lsr #16" CR
	"mov r9, r8, lsr #16" CR
	"movt r2, #0" CR
	"movt r4, #0" CR
	"movt r6, #0" CR
	"movt r8, #0" CR
	"push {%[buffer]}" CR
	/* initialize loop */
	"mov r14, 25" CR
	"enc_step:" CR
	/* load key */
	"ldm %[rk_masked]!, {r10, r11}" CR
	"eor r2, r2, r10" CR
	"eor r3, r3, r10, lsr #16" CR
	"eor r4, r4, r11" CR
	"eor r5, r5, r11, lsr #16" CR
	"ldm %[rk_masked]!, {r10, r11}" CR
	"eor r6, r6, r10" CR
	"eor r7, r7, r10, lsr #16" CR
	"eor r8, r8, r11" CR
	"eor r9, r9, r11, lsr #16" CR

	/* S-layer */
	"push {%[rk_masked], r14}" CR
	/* "orn r6, r5, r3" mapped to [r10, r12] <= orn([r5, r9], [r3, r7])*/
	"mvn r11, r3" CR
	"mvn r10, r12" CR
	"and %[buffer], r5, r11" CR
	"eor r10, r10, %[buffer]" CR
	"mov %[buffer], 0" CR /* prevent leakage for next instruction */
	"bic %[buffer], r5, r7" CR
	"eor r10, r10, %[buffer]" CR
	"mov %[buffer], 0" CR /* prevent leakage for next instruction */
	"bic %[buffer], r11, r9" CR
	"eor r10, r10, %[buffer]" CR
	"mvn r11, r9" CR
	"mov %[buffer], 0" CR /* prevent leakage for next instruction */
	"bic %[buffer], r11, r7" CR
	"eor r10, r10, %[buffer]" CR
	/* "eor r6, r6, r2" mapped to [r10, r0] <= eor([r10, r12], [r2, r6]), r0 goes to stack */
	"eor r10, r10, r2" CR
	"eor %[buffer], r12, r6" CR
	"push {%[buffer]}" CR
	/* "bic r2, r2, r3" mapped to [r2, r12] <= bic([r2, r6], [r3, r7]) */
	"bic %[buffer], r2, r3" CR
	"and r11, r2, r7" CR
	"eor r2, r12, %[buffer]" CR
	"eor r2, r2, r11" CR
	"mov %[buffer], 0" CR /* prevent leakage for next instruction */
	"bic %[buffer], r6, r3" CR
	"eor r2, r2, %[buffer]" CR
	"mov %[buffer], 0" CR /* prevent leakage for next instruction */
	"and %[buffer], r6, r7" CR
	"eor r2, r2, %[buffer]" CR
	/* "eor r7, r4, r5" mapped to [r11, r14] <= eor([r4, r8], [r5, r9]) */
	"eor r11, r4, r5" CR
	"eor r14, r8, r9" CR
	/* "eor r2, r2, r7" mapped to [r2, r6] <= eor([r2, r12], [r11, r14]) */
	"eor r2, r2, r11" CR
	// "eor r6, r6, r14" CR /* <- YLC, bug: r2 is (r2, r12) once masked, not (r2, r6) anymore */
	"eor r6, r12, r14" CR
	/*"eor r5, r3, r4" mapped to [r5, r9] <= eor([r3, r7], [r4, r8]) */
	"eor r5, r3, r4" CR
	"eor r9, r7, r8" CR
	/*"eor r3, r4, r6" mapped to [r3, r7] <= eor([r4, r8], [r10, stack]) */
	"eor r3, r4, r10" CR
	"pop {%[buffer]}" CR
	"eor r7, r8, %[buffer]" CR
	/*"and r7, r6, r7" mapped to [r11, r12] <= and([r10, r0/stack], r[11, r14]) */
	/* YLC, bug r12 is not used correctly. Should use r0 instead most of the time
	"and %[buffer], r10, r11" CR
	"eor r11, r12, %[buffer]" CR
	"and %[buffer], r10, r14" CR
	"eor r11, r11, %[buffer]" CR
	"eor r11, r11, %[rk_masked]" CR
	"and %[buffer], r12, r14" CR
	"eor r11, r11, %[buffer]" CR
	*/
	"and %[rk_masked], r10, r11" CR
	"and r11, r11, %[buffer]" CR
	"eor r11, r11, r12" CR
	"eor r11, r11, %[rk_masked]" CR
	"mov %[rk_masked], 0" CR /* prevent leakage for next instruction */
	"and %[rk_masked], r10, r14" CR
	"eor r11, r11, %[rk_masked]" CR
	"mov %[rk_masked], 0" CR /* prevent leakage for next instruction */
	"and %[rk_masked], %[buffer], r14" CR
	"eor r11, r11, %[rk_masked]" CR
	/*"eor r5, r5, r7" mapped to [r5, r9] <= eor([r5, r9], [r11, r12] */
	"eor r5, r5, r11" CR
	"eor r9, r9, r12" CR
	/*"orr r4, r2, r5" mapped to [r4, r8] <= orr([r2, r6], [r5, r9]) */
	/* YLC, bug: ? but corrected code works and takes 1 less instruction
	"mvn r12, r12" CR
	"and %[rk_masked], r2, r5" CR
	"eor r4, r12, %[rk_masked]" CR
	"mvn r12, r12" CR
	"bic %[rk_masked], r2, r9" CR
	"eor r4, r4, %[rk_masked]" CR
	"bic %[rk_masked], r5, r6" CR
	"eor r4, r4, %[rk_masked]" CR
	"mvn %[rk_masked], r6" CR
	"bic %[rk_masked], %[rk_masked], r9" CR
	"eor r4, r4, %[rk_masked]" CR
	"mov r8, r12" CR
	*/
	"mvn r14, r2" CR
	"bic r11, r14, r5" CR
	"eor r4, r12, r11" CR
	"mov r11, 0" CR /* prevent leakage for next instruction */
	"and r11, r14, r9" CR
	"eor r4, r4, r11" CR
	"mov r11, 0" CR /* prevent leakage for next instruction */
	"bic r11, r6, r5" CR
	"eor r4, r4, r11" CR
	"mov r11, 0" CR /* prevent leakage for next instruction */
	"and r11, r6, r9" CR
	"eor r4, r4, r11" CR
	"mvn r4, r4" CR
	"mov r8, r12" CR
	/*"eor r4, r4, r6" mapped to [r4, r8] <= eor([r4, r8], [r10, r0/stack]) */
	"eor r4, r4, r10" CR
	"eor r8, r8, %[buffer]" CR
	"pop {%[rk_masked], r14}" CR

	/* P layer */
	"bfi r3, r3, #16, #15" CR
	"ror r3, r3, #15" CR
	"bfi r7, r7, #16, #15" CR
	"ror r7, r7, #15" CR
	"bfi r4, r4, #16, #4" CR
	"ror r4, r4, #4" CR
	"bfi r8, r8, #16, #4" CR
	"ror r8, r8, #4" CR
	"bfi r5, r5, #16, #3" CR
	"ror r5, r5, #3" CR
	"bfi r9, r9, #16, #3" CR
	"ror r9, r9, #3" CR
	/* loop end */
	"subs r14, r14, #1" CR
	"bne enc_step" CR
	"pop {%[buffer]}" CR
	/* load key */
	"ldm %[rk_masked]!, {r10, r11}" CR
	"eor r2, r2, r10" CR
	"eor r3, r3, r10, lsr #16" CR
	"eor r4, r4, r11" CR
	"eor r5, r5, r11, lsr #16" CR
	"ldm %[rk_masked]!, {r10, r11}" CR
	"eor r6, r6, r10" CR
	"eor r7, r7, r10, lsr #16" CR
	"eor r8, r8, r11" CR
	"eor r9, r9, r11, lsr #16" CR
	/* store results */
	"bfi r2, r3, #16, #16" CR
	"bfi r4, r5, #16, #16" CR
	"bfi r6, r7, #16, #16" CR
	"bfi r8, r9, #16, #16" CR
	"stm %[buffer], {r2, r4, r6, r8}" CR
	:
	: [rk_masked] "r" (rk_masked), [buffer] "r" (buffer)
	: "r2", "r3", "r4", "r5", "r6", "r7", "r8", "r9", "r10", "r11", "r12", "r14"
	);
}
