/*
 * FELICS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef ARM_MACROS_H
#define ARM_MACROS_H

#include "stringify.h"


#define SEC_AND_(r_mval, r_mask, x_mval, x_mask, y_mask, y_mval, temp) \
	mov temp, 0                                                   \n\t /* correct leakage */ \
	mov r_mask, 0                                                 \n\t /* correct lekage */ \
    and r_mval, x_mval, y_mval                                    \n\t \
    orn temp, x_mval, y_mask                                      \n\t \
    eor r_mval, r_mval, temp                                      \n\t \
    and r_mask, x_mask, y_mval                                    \n\t \
	mov temp, 0                                                   \n\t /* correct leakage */ \
    orn temp, x_mask, y_mask                                      \n\t \
    eor r_mask, r_mask, temp                                      \n\t

#define SEC_AND_IN_PLACE_(r_mask, x_mval, x_mask, y_mask, y_mval, temp) \
    and temp, x_mval, y_mval                                       \n\t \
    orn x_mval, x_mval, y_mask                                     \n\t \
    eor x_mval, x_mval, temp                                       \n\t \
	mov temp, 0                                                    \n\t /* correct leakage */ \
    and temp, x_mask, y_mval                                       \n\t \
    orn r_mask, x_mask, y_mask                                     \n\t \
    eor r_mask, r_mask, temp                                       \n\t

#define SEC_XOR_(r_mval, x_mval, x_mask, y_mask, y_mval) \
    eor r_mval, x_mval, y_mval                      \n\t \
    eor r_mval, r_mval, y_mask                      \n\t

#define SEC_SHIFT_(r_mval, r_mask, x_mval, x_mask, val) \
    lsl r_mval, x_mval, val                        \n\t \
    lsl r_mask, x_mask, val                        \n\t


#define SEC_AND(r_mval, r_mask, x_mval, x_mask, y_mask, y_mval, temp) \
    STR(SEC_AND_(r_mval, r_mask, x_mval, x_mask, y_mask, y_mval, temp))

#define SEC_AND_IN_PLACE(r_mask, x_mval, x_mask, y_mask, y_mval, temp) \
    STR(SEC_AND_IN_PLACE_(r_mask, x_mval, x_mask, y_mask, y_mval, temp))

#define SEC_XOR(r_mval, x_mval, x_mask, y_mask, y_mval) \
    STR(SEC_XOR_(r_mval, x_mval, x_mask, y_mask, y_mval))

#define SEC_SHIFT(r_mval, r_mask, x_mval, x_mask, val) \
    STR(SEC_SHIFT_(r_mval, r_mask, x_mval, x_mask, val))

#endif /* ARM_MACROS_H */
