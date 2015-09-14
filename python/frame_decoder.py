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
import ctypes

class frame_decoder(gr.sync_block):
    """
    docstring for block frame_decoder
    """
    def __init__(self, rs_enabled):
        gr.sync_block.__init__(self,
            name="frame_decoder",
            in_sig=[numpy.uint8],
            out_sig=None)

        self.libax100 = ctypes.CDLL('libgnuradio-ax100.so')
        self.libax100.decode_rs_8.restype = ctypes.c_int
        self.libax100.decode_rs_8.argtypes = [ctypes.c_char_p, ctypes.POINTER(ctypes.c_int), ctypes.c_int, ctypes.c_int]
        self.message_port_register_out(pmt.intern('out'))


    def work(self, input_items, output_items):
        in0 = input_items[0]

        tags = self.get_tags_in_window(0,0,len(in0))

        for tag in tags:
            try:
                packet = {}
                packet['length'] = numpy.packbits(in0[tag.offset - self.nitems_read(0):tag.offset + 8 - self.nitems_read(0)])
                packet['payload'] = numpy.packbits(in0[tag.offset + 8 - self.nitems_read(0):tag.offset + int(packet['length']) * 8 - self.nitems_read(0)])

                # Calculate RS
                bytes_corr = self.libax100.decode_rs_8(packet['payload'].ctypes.data_as(ctypes.c_char_p), ctypes.POINTER(ctypes.c_int)(), 0, 255 - int(packet['length'] - 1))

                # Endian swap CSP header
                tmp = numpy.copy(packet['payload'][:4])
                packet['payload'][0] = tmp[3]
                packet['payload'][1] = tmp[2]
                packet['payload'][2] = tmp[1]
                packet['payload'][3] = tmp[0]
            
                if bytes_corr >= 0:
                    msg_data = pmt.to_pmt(numpy.concatenate((numpy.array([int(packet['length']) - 33], dtype=numpy.uint8), packet['payload'][:int(packet['length']) - 33])))
                    self.message_port_pub(pmt.intern('out'),  msg_data)

                print "------ PACKET DECODING -------"
                print "Packet length:  ", int(packet['length'])
                if bytes_corr < 0:
                    print "RS ERROR - COULD NOT DECODE PACKET"
                else:
                    print "Bytes corrected: ", bytes_corr

                print ""

            except:
                print "except in frame decode"

        return len(input_items[0])

