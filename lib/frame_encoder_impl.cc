/* -*- c++ -*- */
/* 
 * Copyright 2015 <+YOU OR YOUR COMPANY+>.
 * 
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 * 
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include "frame_encoder_impl.h"

namespace gr {
  namespace ax100 {

    frame_encoder::sptr
    frame_encoder::make()
    {
      return gnuradio::get_initial_sptr
        (new frame_encoder_impl());
    }

    /*
     * The private constructor
     */
    frame_encoder_impl::frame_encoder_impl()
      : gr::sync_block("frame_encoder",
              gr::io_signature::make(0, 0, 0),
              gr::io_signature::make(1, 1, sizeof(unsigned char)))
    {
	    set_output_multiple(50+4+255);
	    lsr.reset();
    
    }

    /*
     * Our virtual destructor.
     */
    frame_encoder_impl::~frame_encoder_impl()
    {
    }

    int
    frame_encoder_impl::work(int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
    {
	unsigned int preamble_len = 50;
	const unsigned int sync_word = 0xDE510B93;
	unsigned int tmp;
      
      unsigned char *out = (unsigned char *) output_items[0];

	// Reset shift register
	lsr.reset();
      // Check that noutput_items is large enough for preamble and packet
      // TODO

	// For now limiting items per call to be able to get a chance to schedule frame in
	if (noutput_items > 1000)
		noutput_items = 1000;

	// Insert preamble
	memset(out, 0x55, preamble_len);
	// Insert sync word
	memcpy(&out[preamble_len], &sync_word, 4);
	// Insert packet length
	out[preamble_len + 4] = 200;
	// Packet data
	for (int i = 0; i < 250; i++) {
		out[preamble_len + 4 + 1 + i] = i;
	}
	// Add RS
	// TODO

	memset(&out[preamble_len + 4 + 1 + 250], 0, noutput_items - preamble_len - 4 - 1 - 250);

	// Scramble from sync word to end of packet
	// TODO for now just scramble rest of the bits. We should instead turn off the transmitter when the packet is done
	for (int i = preamble_len; i < noutput_items; i++) {
		for (int j = 7; j >= 0; j--) {
			tmp = lsr[16];
			lsr[4] = lsr[4] ^ tmp;
			lsr <<= 1;
			lsr[0] = ((out[i] >> j) & 1) ^ tmp;
			if (tmp) {
				out[i] |= 1 << j;
			} else {
				out[i] &= ~(1 << j);
			}
		}
	}

      // Tell runtime system how many output items we produced.
      return noutput_items;
    }

  } /* namespace ax100 */
} /* namespace gr */

