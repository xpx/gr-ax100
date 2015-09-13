#!/usr/bin/env python
# -*- coding: utf-8 -*-
# 
# Copyright 2015 <+YOU OR YOUR COMPANY+>.
# 
# This is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
# 
# This software is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this software; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.
# 

import numpy
import pmt
from gnuradio import gr

class frame_decoder(gr.sync_block):
    """
    docstring for block frame_decoder
    """
    def __init__(self, rs_enabled):
        gr.sync_block.__init__(self,
            name="frame_decoder",
            in_sig=[numpy.uint8],
            out_sig=None)

        self.message_port_register_out(pmt.intern('out'))


    def work(self, input_items, output_items):
        in0 = input_items[0]

        tags = self.get_tags_in_window(0,0,len(in0))
        for tag in tags:
            try:
                print "Syncword found with {} bit errors at pos {}".format(tag.value, tag.offset)
                packet = {}
                packet['length'] = numpy.packbits(in0[tag.offset - self.nitems_read(0):tag.offset+8 - self.nitems_read(0)])
                packet['payload'] = numpy.packbits(in0[tag.offset+8 - self.nitems_read(0):tag.offset+8+int(packet['length'])*8 - self.nitems_read(0)])
                print "packet len: ", int(packet['length'])
                tmp = numpy.copy(packet['payload'][:4])
                packet['payload'][0] = tmp[3]
                packet['payload'][1] = tmp[2]
                packet['payload'][2] = tmp[1]
                packet['payload'][3] = tmp[0]
            
                msg_data = pmt.to_pmt(numpy.concatenate((numpy.array([int(packet['length']) - 33], dtype=numpy.uint8), packet['payload'][:int(packet['length']) - 33])))
                self.message_port_pub(pmt.intern('out'),  pmt.cons(pmt.PMT_NIL, msg_data))
            except:
                print "except in frame decode"
        return len(input_items[0])

