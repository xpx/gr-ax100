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
    frame_decoder_cc::make(char *address)
    {
      return gnuradio::get_initial_sptr
        (new frame_decoder_cc_impl(address));
    }

    /*
     * The private constructor
     */
    frame_decoder_cc_impl::frame_decoder_cc_impl(char *address)
      : gr::sync_block("frame_decoder_cc",
              gr::io_signature::make(1, 1, sizeof(unsigned char)),
              gr::io_signature::make(0, 0, 0))
    {
	    set_output_multiple(256*8);

      	    d_context = new zmq::context_t(1);
	    d_socket = new zmq::socket_t(*d_context, ZMQ_PUB);
	    d_socket->connect(address);

    }

    /*
     * Our virtual destructor.
     */
    frame_decoder_cc_impl::~frame_decoder_cc_impl()
    {
	   delete d_pack;
	   d_socket->close();
	   delete d_socket;
	   delete d_context;
    }

    int
    frame_decoder_cc_impl::work(int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
    {
      const unsigned char *in = (const unsigned char *) input_items[0];
      unsigned char *out = (unsigned char *) output_items[0];
      unsigned char tmp;
      int rs_res;
      void *msg_data = NULL;
      unsigned char frame_data[255];
      int i = 0;
      d_tags.clear();
      d_pack = new blocks::kernel::pack_k_bits(8);

      this->get_tags_in_window(d_tags, 0, 0, noutput_items);
      for(d_tags_itr = d_tags.begin(); d_tags_itr != d_tags.end(); d_tags_itr++) {
		// Check that we have enough data for a full frame
		if ((d_tags_itr->offset - this->nitems_read(0)) > (noutput_items - 255 * 8)) {
			//printf("Not enough samples for full packet at tag offset: %lu\r\n", d_tags_itr->offset - this->nitems_read(0));
			return (d_tags_itr->offset - this->nitems_read(0) - 1);
		}

		d_pack->pack(frame_data, &in[d_tags_itr->offset - this->nitems_read(0)], 1);
		d_pack->pack(&frame_data[1], &in[d_tags_itr->offset + 8 - this->nitems_read(0)], frame_data[0] - 1);
		rs_res = decode_rs_8(&frame_data[1], NULL, 0, 255 - frame_data[0] -1);

		// Send via ZMQ if RS ok
		if (rs_res >= 0) {
			// Swap CSP header
			tmp = frame_data[4];
			frame_data[4] = frame_data[1];
			frame_data[1] = tmp;
			tmp = frame_data[3];
			frame_data[3] = frame_data[2];
			frame_data[2] = tmp;

			// Send with ZMQ
			// len_byte + csp_packet - rs = frame_len - 32
			frame_data[0] -= 32;
			zmq::message_t zmsg(frame_data[0]);
			memcpy(zmsg.data(), frame_data, frame_data[0]);
			if (d_socket->send(zmsg) < 0) {
				printf("ZMQ send error\r\n");
			}
		}
		printf("\r\n------------ FRAME INFO -------------\r\n");
		printf("Length: %d\r\nBytes corrected: %d\r\n", frame_data[0], rs_res);

      }

      // Tell runtime system how many output items we produced.
      return noutput_items;
    }


  } /* namespace ax100 */
} /* namespace gr */

