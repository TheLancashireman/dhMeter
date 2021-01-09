/* dhMeter - a frequency meter with capacitance and inductance ranges
 *
 * (c) David Haworth
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
#include <Arduino.h>
#include <dhMeter.h>
#include <timestamp.h>
#include <LiquidCrystal.h>

/* How it works:
 *
 * The external frequency to be measured is connected to the T0 pin. Timer 0 is configured to count
 * the pulses on this external pin.
 *
 * The standard Arduino time functions (millis, micros, delay etc.) are disabled.
 *
 * In their place there is a timestamp feature that uses TCNT1 (16-bit) running at the CPU clock frequency.
 * This clock is used to measure an interval of 1 second as accurately as possible. During this interval,
 * the pulses from TCNT0 are accumulated in a variable. The content of this variable is therefore the
 * frequency on the T0 pin, in Hz.
 *
*/

byte mode;

void Setup(void);
unsigned long Measure(void);
void CalculateAndDisplay(unsigned long f);
void SetMode(byte m);
void ClearToEol(byte np);

/* We cannot use a static constructor because the LiquidCrystal library uses the Arduino delay functions
 * and the local replacement hasn't been initialised yet.
*/
LiquidCrystal *lcd;

int main(void)
{
	init();
	Setup();

	for (;;)
	{
		unsigned long f = Measure();
		CalculateAndDisplay(f);
	}
}

void Setup()
{
	// Initialise the timestamp (timer1)
	init_timestamp();

	// Initialise the lcd driver
	lcd = new LiquidCrystal(lcd_rs, lcd_rw, lcd_e, lcd_d0, lcd_d1, lcd_d2, lcd_d3, lcd_d4, lcd_d5, lcd_d6, lcd_d7);
	lcd->begin(16, 2);

	// Display a friendly greeting
	lcd->setCursor(0,0);
	lcd->print(F("dhMeter"));
	lcd->setCursor(0,1);
	lcd->print(F("(c) dh   GPLv3"));

	// Initialise the pulse counter (timer0)
	pinMode(4, INPUT_PULLUP);
	TCCR0A = 0;		// All waveform generation disabled
	TCCR0B = 0x7;	// All waveform generation disabled. Clock source is T0 (port D4), rising edge.
	TIMSK0 = 0;		// All interrupts disabled.
	TIFR0 = 0x07;	// Clear all interrupts.
	GTCCR = 0;		// Clear synchronisation modes.
	TCNT0 = 0;

	// Give the user chance to read the friendly greeting
	tick_delay(MILLIS_TO_TICKS(2000));

	// Initial mode is frequency measurement.
	SetMode(dhm_freq);
}

unsigned long Measure(void)
{
	unsigned long pulses = 0;
	unsigned long ticks = 0;
	byte last_tc0 = TCNT0;
	unsigned last_tc1 = TCNT1;

	while ( ticks < (unsigned long)(MILLIS_TO_TICKS(1000)) )
	{
		byte tc0 = TCNT0;
		unsigned tc1 = TCNT1;
		pulses += (unsigned long)(tc0 - last_tc0) & 0xff;
		last_tc0 = tc0;
		ticks += (unsigned long)(tc1 - last_tc1) & 0xffff;
		last_tc1 = tc1;
	}

	/* Adjust the timestamp variables to account for the time spent in the measurement loop
	*/
	ts_timestamp += ticks;
	ts_last_t1 = last_tc1;

#if 0	// Testing the display - replace output with a value that increments every measurement.
	static unsigned long testval;
	pulses = testval++;
#endif

	return pulses;
}

void CalculateAndDisplay(unsigned long f)
{
	byte nprinted = 0;

	lcd->setCursor(0,1);

	switch ( mode )
	{
	case dhm_idle:
		break;

	case dhm_freq:
		nprinted += lcd->print(f);
		nprinted += lcd->print(F("Hz"));
		break;

	default:
		nprinted += lcd->print(F("5 gold rings"));
		break;
	}

	ClearToEol(nprinted);
}

void SetMode(byte m)
{
	byte np;
	mode = m;

	lcd->setCursor(0,0);
	switch (mode)
	{
	case dhm_idle:
		np = lcd->print(F("Off"));
		break;

	case dhm_freq:
		np = lcd->print(F("Frequency"));
		break;

	case dhm_cap:
		np = lcd->print(F("Capacitance"));
		break;

	case dhm_ind:
		np = lcd->print(F("Inductance"));
		break;

	default:
		np = lcd->print(F("Wibble"));
		break;
	}

	ClearToEol(np);
	lcd->setCursor(0,1);
	ClearToEol(0);
}

void ClearToEol(byte np)
{
	while ( np < 16 )
	{
		lcd->print(' ');
		np++;
	}
}
