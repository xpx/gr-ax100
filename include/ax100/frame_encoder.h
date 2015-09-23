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


#ifndef INCLUDED_AX100_FRAME_ENCODER_H
#define INCLUDED_AX100_FRAME_ENCODER_H

#include <ax100/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace ax100 {

    /*!
     * \brief <+description of block+>
     * \ingroup ax100
     *
     */
    class AX100_API frame_encoder : virtual public gr::sync_block
    {
     public:
      typedef boost::shared_ptr<frame_encoder> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of ax100::frame_encoder.
       *
       * To avoid accidental use of raw pointers, ax100::frame_encoder's
       * constructor is in a private implementation
       * class. ax100::frame_encoder::make is the public interface for
       * creating new instances.
       */
      static sptr make();
    };

  } // namespace ax100
} // namespace gr

#endif /* INCLUDED_AX100_FRAME_ENCODER_H */

