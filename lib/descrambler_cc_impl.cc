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
#include "descrambler_cc_impl.h"

namespace gr {
  namespace ax100 {

    descrambler_cc::sptr
    descrambler_cc::make()
    {
      return gnuradio::get_initial_sptr
        (new descrambler_cc_impl());
    }

    /*
     * The private constructor
     */
    descrambler_cc_impl::descrambler_cc_impl()
      : gr::sync_block("descrambler_cc",
              gr::io_signature::make(1, 1, sizeof(unsigned char)),
              gr::io_signature::make(1, 1, sizeof(unsigned char)))
    {
	    lsr = 0;
    }

    /*
     * Our virtual destructor.
     */
    descrambler_cc_impl::~descrambler_cc_impl()
    {
    }

    int
    descrambler_cc_impl::work(int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
    {
      const unsigned char *in = (const unsigned char *) input_items[0];
      unsigned char *out = (unsigned char *) output_items[0];
      int i;
      for (i = 0; i < noutput_items; i++) {
	out[i] = (lsr & 1) ^ ((lsr >> 12) & 1) ^ ((lsr >> 17) & 1);
	lsr = lsr << 1;
	lsr = lsr | (in[i] & 1);
      }

      // Tell runtime system how many output items we produced.
      return i;
    }

  } /* namespace ax100 */
} /* namespace gr */

