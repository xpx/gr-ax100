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

#ifndef INCLUDED_AX100_FRAME_DECODER_CC_IMPL_H
#define INCLUDED_AX100_FRAME_DECODER_CC_IMPL_H

#include <ax100/frame_decoder_cc.h>
#include <gnuradio/blocks/pack_k_bits.h>
#include <zmq.hpp>

namespace gr {
  namespace ax100 {

    class frame_decoder_cc_impl : public frame_decoder_cc
    {
     private:
      // Tags
      std::vector<tag_t> d_tags;
      std::vector<tag_t>::iterator d_tags_itr;

      // Pack bits
      blocks::kernel::pack_k_bits *d_pack;

      // ZMQ
      zmq::context_t 	*d_context;
      zmq::socket_t 	*d_socket;
	
     public:
      frame_decoder_cc_impl();
      ~frame_decoder_cc_impl();

      // Where all the action really happens
      int work(int noutput_items,
         gr_vector_const_void_star &input_items,
         gr_vector_void_star &output_items);
    };

  } // namespace ax100
} // namespace gr

#endif /* INCLUDED_AX100_FRAME_DECODER_CC_IMPL_H */

