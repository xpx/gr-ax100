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
#include "frame_decoder_cc_impl.h"
#include "ax100rs.h"

namespace gr {
  namespace ax100 {

    frame_decoder_cc::sptr
    frame_decoder_cc::make()
    {
      return gnuradio::get_initial_sptr
        (new frame_decoder_cc_impl());
    }

    /*
     * The private constructor
     */
    frame_decoder_cc_impl::frame_decoder_cc_impl()
      : gr::sync_block("frame_decoder_cc",
              gr::io_signature::make(1, 1, sizeof(unsigned char)),
              gr::io_signature::make(0, 0, 0))
    {
	    set_output_multiple(256*8);
    }

    /*
     * Our virtual destructor.
     */
    frame_decoder_cc_impl::~frame_decoder_cc_impl()
    {
	   delete d_pack;
    }

    int
    frame_decoder_cc_impl::work(int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
    {
      const unsigned char *in = (const unsigned char *) input_items[0];
      unsigned char *out = (unsigned char *) output_items[0];
      unsigned char frame_len;
      int rs_res;
      unsigned char frame_data[255];
      d_tags.clear();
      d_pack = new blocks::kernel::pack_k_bits(8);

      this->get_tags_in_window(d_tags, 0, 0, noutput_items);
      for(d_tags_itr = d_tags.begin(); d_tags_itr != d_tags.end(); d_tags_itr++) {
		// Check that we have enough data for a full frame
		if ((d_tags_itr->offset - this->nitems_read(0)) > (noutput_items - 255 * 8)) {
			printf("Not enough samples for full packet at tag offset: %lu\r\n", d_tags_itr->offset - this->nitems_read(0));
			return (d_tags_itr->offset - this->nitems_read(0) - 1);
		}

		d_pack->pack(&frame_len, &in[d_tags_itr->offset - this->nitems_read(0)], 1);
		printf("Packet len: %d, offset: %d, read: %d, outputote,s: %d\r\n", test, d_tags_itr->offset, this->nitems_read(0), noutput_items);
		d_pack->pack(frame_data, &in[d_tags_itr->offset + 8 - this->nitems_read(0)], test - 1);
		rs_res = decode_rs_8(frame_data, NULL, 0, 255 - 237 -1);
		printf("RS bytes corrected: %d\r\n", rs_res);

      }

      // Tell runtime system how many output items we produced.
      return noutput_items;
    }

  } /* namespace ax100 */
} /* namespace gr */

