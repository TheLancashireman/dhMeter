/* dhMeter.h
 *
 * (c) David Haworth
 *
 * dhMeter.h is part of dhMeter
 *
 * dhMeter is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * dhMeter is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with dhMeter.  If not, see <http://www.gnu.org/licenses/>.
 *
 * dhMeter is an Arduino sketch, written for an Arduino Nano
*/
#ifndef DHMETER_H
#define DHMETER_H	1

// Operating modes
#define dhm_idle	0
#define dhm_freq	1
#define dhm_cap		2
#define dhm_ind		3

// Pin selection for the LCD display
#define lcd_rs		2
#define lcd_rw		3
#define lcd_e		5
#define lcd_d0		6
#define lcd_d1		7
#define lcd_d2		8
#define lcd_d3 		9
#define lcd_d4		10
#define lcd_d5		11
#define lcd_d6		12
#define lcd_d7		13

extern void init(void);

#endif
