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


    def work(self, input_items, output_items):
        in0 = input_items[0]

        tags = self.get_tags_in_window(0,0,len(in0))
        for tag in tags:
            print "Syncword found with {} bit errors at pos {}".format(tag.value, tag.offset)
            packet = {}
            packet['length'] = numpy.packbits(in0[tag.offset:tag.offset+8])
            packet['payload'] = numpy.packbits(in0[tag.offset+8:tag.offset+8+int(packet['length'])*8])
            print "Packet length {}".format(int(packet['length']))
            for i in range(len(packet['payload'])):
                print i, hex(packet['payload'][i])


        return len(input_items[0])

