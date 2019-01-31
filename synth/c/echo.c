/* echo.c - echo effect
 *
 * (c) 2018 David Haworth
 *
 * $(NAME) is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * $(NAME) is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with $(NAME).  If not, see <http://www.gnu.org/licenses/>.
*/
#include <dv-config.h>
#include <davroska.h>
#include <effect.h>

/* The echo effect works by adding an earlier sample onto the current sample.
 *
 * This version is a hifi version - all the samples are stored in a ring buffer.
 * To reduce the amount of memory needed, it's possible to store fewer samples. You get a
 * slightly distorted echo.
*/

#define DELAY_BUF_LEN	48000	/* Allows a 1 second echo at 48 Ksamples/sec */

int delay_buf[DELAY_BUF_LEN];	/* The delay buffer. */
int store_index;				/* Where to store the new sample. */
int load_index;					/* Where to load the echo sample from. */
float attenuation;				/* A fractional attenuation factor. */

/* A single echo is generated by adding a fraction of an earlier sample to the input.
 * The earlier sample is obtained via the load_index, which is always delay_samples behind the store_index.
 *
 * The new input sample is stored at the store_index.
 *
 * Then both indexes are incremented modulo DELAY_BUF_LEN.
*/
int single_echo(effect_t *link, int sample_in)
{
	int sample_out;

	sample_out = sample_in + (int)((float)delay_buf[load_index] * attenuation);

	delay_buf[store_index] = sample_in;

	load_index++;
	if ( load_index >= DELAY_BUF_LEN )	load_index = 0;

	store_index++;
	if ( store_index >= DELAY_BUF_LEN )	store_index = 0;

	return sample_out;
}

/* A multiple echo is generated in the same way as a single echo, except the sample that is stored
 * at the store_index is the new output sample rather than the input sample. Thus the echo feeds back.
*/
int multiple_echo(effect_t *link, int sample_in)
{
	int sample_out;

	sample_out = sample_in + (int)((float)delay_buf[load_index] * attenuation);

	delay_buf[store_index] = sample_out;

	load_index++;
	if ( load_index >= DELAY_BUF_LEN )	load_index = 0;

	store_index++;
	if ( store_index >= DELAY_BUF_LEN )	store_index = 0;

	return sample_out;
}

/* Initialisation function for changing the setup.
*/
int setup_echo(effect_t *link, int delay_samples, float atten, int single)
{
	store_index = (load_index + delay_samples) % DELAY_BUF_LEN;
	attenuation = atten;
	if ( single )
		link->func = single_echo;
	else
		link->func = multiple_echo;

	return 0;
}

/* Initialisation function for a new effect (clears the buffer to avoid spurious injection of old samples)
*/
int init_echo(effect_t *link, int delay_samples, float atten, int single)
{
	int i;
	int rv;

	load_index = 0;
	rv = setup_echo(link, delay_samples, atten, single);

	for ( i = 0; i <= store_index; i++ )
		delay_buf[i] = 0;

	return rv;
}
