#!/usr/bin/env python 
# $Id: modlib.py 408 2005-03-03 16:42:04Z hynek $
"""Modbus/TCP protocol implementation in Python.   

Modbus/TCP Server/Slave
=======================

  To write a Modbus/TCP Server you have to create ModbusServerContext first.
  ModbusServerContext contains configuration and process image of the
  server. To be continued ...

  Sample Modbus/TCP Server (slave):
  ---------------------------------
   
    import modlib

    c = modlib.ModbusServerContext(
        holding_registers_address=0,
        holding_registers_count=2024)
    s = modlib.ModbusTCPServer(context=c)
    try:
        s.serve_forever()
    except KeyboardInterrupt:
        print "Ctrl+C pressed - exiting..."
    s.server_close()

Modbus/TCP Client/Master
========================

  Sample Modbus/TCP Client (master):
  ----------------------------------

    from modlib import *

    cn = TCPMasterConnection("127.0.0.1")

    # Read Coils
    req = ReadCoilsRequest(address=0, count=2)
    tr = cn.createTransaction(req)
    res = tr.execute()

    # Read Discrete Inputs
    req = ReadDiscreteInputsRequest(address=0, count=1)
    tr.setRequest(req)
    res = tr.execute()

     Read Holding Registers
    req = ReadHoldingRegistersRequest(address=0, count=4)
    tr.setRequest(req)
    res = tr.execute()

Copyright & Licence
===================

Copyright (c) 2001-2005 S.W.A.C. GmbH, Germany.
Copyright (c) 2001-2005 S.W.A.C. Bohemia s.r.o., Czech Republic.

This application is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License as published
by the Free Software Foundation; either version 2 of the License, or (at
your option) any later version.

This application is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public
License along with this application; if not, write to the
Free Software Foundation, Inc., 59 Temple Place - Suite 330,
Boston, MA 02111-1307, USA.
  
"""
__author__  = "Hynek Petrak <hynek@swac.cz>"
__date__    = "20 January 2004"
__version__ = "$Rev: 408 $"

import socket
import SocketServer
import struct
import logging
import sys
import traceback
        
__all__ = ["READ_COILS", "READ_DISCRETE_INPUTS", "READ_HOLDING_REGISTERS",
    "READ_INPUT_REGISTERS", "WRITE_SINGLE_COIL", "WRITE_SINGLE_REGISTER",
    "WRITE_MULTIPLE_COILS", "WRITE_MULTIPLE_REGISTERS", 
    "ILLEGAL_FUNCTION_EXCEPTION",
    "ReadBitsResponseBase", "ReadRegistersResponseBase",
    "ILLEGAL_ADDRESS_EXCEPTION", "ILLEGAL_VALUE_EXCEPTION",
    "ParameterException", "ReadCoilsRequest", "ReadDiscreteInputsRequest",
    "ReadHoldingRegistersRequest", "ReadInputRegistersRequest",
    "WriteSingleCoilRequest", "WriteSingleRegisterRequest",
    "ReadWriteMultipleRegistersRequest",
    "ReadWriteMultipleRegistersResponse",
    "WriteMultipleCoilsRequest", "WriteMultipleRegistersRequest",
    "ExceptionResponse", "TCPMasterConnection",
    "ModbusTCPServer", "ModbusServerContext"]
#    "ModbusRequestHandler"]

# TODO: 
# * Implement Modbus over serial line
# * Check inside encode method whether parameters are OK.


READ_COILS = 1
"""Defines function code for ``read coils``."""
READ_DISCRETE_INPUTS = 2
"""Defines function code for ``read input discretes``."""
READ_HOLDING_REGISTERS = 3
"""Defines function code for ``read holding registers``."""
READ_INPUT_REGISTERS = 4
"""Defines function code for ``read input registers``."""
WRITE_SINGLE_COIL = 5
"""Defines function code for ``write single coil``."""
WRITE_SINGLE_REGISTER = 6
"""Defines function code for ``write single register``."""
WRITE_MULTIPLE_COILS = 15
"""Defines function code for ``write multiple coils``."""       
WRITE_MULTIPLE_REGISTERS = 16
"""Defines function code for ``write multiple registers``."""       
READ_WRITE_MULTIPLE_REGISTERS = 23
"""Defines function code for ``read/write multiple registers``."""       
ANNOUNCE_MASTER = 100
"""Defines a user-defined function code
   for announcing a master in Modbus/UDP.
"""
REGISTER_SLAVE = 105
"""Defines a user-defined function code
   for registering a slave in Modbus/UDP.
"""
COIL_ON = 255
"""Defines the byte representation of the coil state **on**."""
COIL_OFF = 0
"""Defines the byte representation of the coil state **off**."""
COIL_ON_BYTES = struct.pack(">BB", COIL_ON, COIL_OFF)
"""Defines the word representation of the coil state **on**."""
COIL_OFF_BYTES = struct.pack(">BB", COIL_OFF, COIL_OFF)
"""Defines the word representation of the coil state **off**."""
EXCEPTION_OFFSET = 128
"""Defines the Modbus slave exception offset that is added to the
   function code, to flag an exception.
   The last valid function code is 127.
"""
ILLEGAL_FUNCTION_EXCEPTION = 1
"""Defines the Modbus slave exception type ``Illegal Function``.
   This exception code is returned if the slave:
   - does not implement the function code **or**
   - is not in a state that allows it to process the function
"""
ILLEGAL_ADDRESS_EXCEPTION = 2
"""Defines the Modbus slave exception type ``Illegal Data Address``.
   This exception code is returned if the reference:
   - does not exist on the slave **or**
   - the combination of reference and length exceeds the bounds
     of the existing registers.
"""
ILLEGAL_VALUE_EXCEPTION = 3
"""Defines the Modbus slave exception type ``Illegal Data Value``.
   This exception code indicates a fault in the structure of the data values
   of a complex request, such as an incorrect implied length.
   
   **This code does not indicate a problem with application specific validity
   of the value.**
"""
SLAVE_DEVICE_FAILURE_EXCEPTION = 4
"""Defines the Modbus slave exception type ``Slave Device Failure``.

"""
DEFAULT_PORT = 502
"""Defines the default port number of Modbus (=``502``)"""
DEFAULT_TRANSACTION_ID = 0
"""Defines the default transaction identifier (=``0``)."""
DEFAULT_PROTOCOL_ID = 0
"""Defines the default protocol identifier (=``0``)."""
DEFAULT_UNIT_ID = 0
"""Defines the default unit identifier (=``0``)."""
DEFAULT_VALIDITYCHECK = 1
"""Defines the default setting for validity checking
   in transactions (=``1``)."""
DEFAULT_TIMEOUT = 3.000
"""Defines the default setting for I/O operation timeouts
   in seconds (=``3.000``)."""
DEFAULT_RECONNECTING = 0 
"""Defines the default reconnecting setting for
   transactions (=``0``)."""
DEFAULT_RETRIES = 3
"""Defines the default amount of retires for opening
   a connection (=``3``)."""
SERIAL_ENCODING_ASCII="ascii"
SERIAL_ENCODING_RTU="rtu"
DEFAULT_SERIAL_ENCODING=SERIAL_ENCODING_ASCII

l = logging.getLogger('modbus.library')
logging.basicConfig()
l.setLevel(logging.DEBUG)

def packBitsToString(bits):
    ret = ''
    i = packed = 0
    for bit in bits:
        if bit: packed += 128
        i += 1
        if i == 8:
            ret += chr(packed)
            i = packed = 0
        else: packed >>= 1
    if i > 0 and i < 8:
        packed >>= 7-i
        ret += chr(packed)
    return ret
    
def unpackBitsFromString(string):
    byte_count = ord(string[0])
    bits = []
    for byte in range(1, byte_count+1):
        value = ord(string[byte])
        for bit in range(8):
            bits.append((value & 1) == 1)
            value >>= 1
    return bits, byte_count

def dumpExceptionInfo():
    ei = sys.exc_info()
    l.error('%s: %s\n%s' % (ei[0], ei[1], '\n'.join(
        traceback.format_tb (ei[2]))))

class ModbusException(Exception):
    # Subclasses that define an __init__ must call Exception.__init__
    # or define self.args.  Otherwise, str() will fail.
    pass

class ModbusIOException(ModbusException):
    pass

class ParameterException(ModbusException):
    pass

class NotImplementedException(ModbusException):
    pass

class ModbusPDU:
    """Base class for all Modbus mesages"""
    def __init__(self):
        self.transaction_id = DEFAULT_TRANSACTION_ID
        self.protocol_id = DEFAULT_PROTOCOL_ID
        self.unit_id = DEFAULT_UNIT_ID
    def encodeData(self):
        raise NotImplementedException()
    def decodeData(self, data):
        """Decodes data part of the message.
           ``data`` is a string object
        """
        raise NotImplementedException()
      
class ModbusRequest(ModbusPDU):
    def __init__(self):
        ModbusPDU.__init__(self)
    def createExceptionResponse(self, exception_code):
        return ExceptionResponse(self.function_code, 
            exception_code)

class ModbusResponse(ModbusPDU):
    def __init__(self):
        ModbusPDU.__init__(self)

class ModbusTransaction:
    def setRequest(self, req):
        pass
    def execute(self):
        """Executes Modbus "transaction" 
        1. sends request to the server
        2. reads the response
        3. returns response
        Implemented in derived classes
        """
        raise NotImplementedException()

class ModbusTCPTransaction(ModbusTransaction):
    transaction_id = DEFAULT_TRANSACTION_ID
    def __init__(self, con, req=None):
        self.req = req
        self.con = con
        self.retries = DEFAULT_RETRIES
    def setRequest(self, req):
        self.req = req
    def setRetryCount(self, retries):
        self.retries = retries
    def execute(self):
        retries = self.retries
        self.req.transaction_id = self.getUniqueTransactionId() 
        l.debug("Running transaction %d" % self.req.transaction_id)
        while(1):
            try:
                self.con.connect()
                tr = self.con.modbus_transport
                tr.writeMessage(self.req)
                res = tr.readResponse()
                return res
            except socket.error, msg:
                self.con.close()
                if retries > 0:
                    l.debug("Attemp to execute transaction failed. (%s) " %
                    msg)
                    l.debug("Will try %d more times." % self.retries)
                    retries -= 1
                    continue
                raise
    def getUniqueTransactionId(self):
        id = ModbusTCPTransaction.transaction_id
        ModbusTCPTransaction.transaction_id += 1
        return id
        
class IllegalFunctionRequest(ModbusRequest):
    """Class for all requests with unknown/unsupported function code.
    """
    def __init__(self, function_code):
        ModbusRequest.__init__(self)
        self.function_code = function_code
    def decodeData(self, data):
        pass
    def execute(self, context):
        return ExceptionResponse(self.function_code, 
            ILLEGAL_FUNCTION_EXCEPTION)
        
class ReadBitsRequestBase(ModbusRequest):
    """Base class for Messages Requesting bit values"""
    def __init__(self, address, count):
        ModbusRequest.__init__(self)
        self.address = address
        self.count = count
    def encodeData(self):
        ret = struct.pack('>HH', self.address, self.count) 
        return ret
    def decodeData(self, data):
        self.address, self.count = struct.unpack('>HH', data)

class ReadBitsResponseBase(ModbusResponse):
    """Base class for Messages responding to bit-reading values"""
    def __init__(self, values):
        ModbusResponse.__init__(self)
        if values != None:
            self.bits = values
        else: self.bits = []
    def encodeData(self):
        ret = packBitsToString(self.bits)
        return chr(len(ret)) + ret
    def decodeData(self, data):
        self.bits = unpackBitsFromString(data)[0]
    def setBit(self, address, value=1):
        self.bits[address] = value
    def resetBit(self, address):
        self.setBit(address, 0)
    def getBit(self, address):
        return self.bits[address]

class ReadCoilsRequest(ReadBitsRequestBase):
    """**MODBUS Application Protocol Specification V1.1:**
    "This function code is used to read from 1 to 2000 contiguous status
    of coils in a remote device. The Request PDU specifies the starting
    address, ie the address of the first coil specified, and the number of
    coils. In the PDU Coils are addressed starting at zero. Therefore coils
    numbered 1-16 are addressed as 0-15."
    """
    function_code = READ_COILS
    def __init__(self, address=None, count=None):
        ReadBitsRequestBase.__init__(self, address, count)
    def execute(self, context):
        if not (1 <= self.count <= 0x7d0):
            return self.createExceptionResponse(ILLEGAL_VALUE_EXCEPTION)
        if not context.checkCoilAddress(self.address, self.count):
            return self.createExceptionResponse(ILLEGAL_ADDRESS_EXCEPTION)
        values = context.getCoilValues(self.address, self.count)
        return ReadCoilsResponse(values)


class ReadCoilsResponse(ReadBitsResponseBase):
    """**MODBUS Application Protocol Specification V1.1:**
    "The coils in the response message are packed as one coil per bit of
    the data field. Status is indicated as 1= ON and 0= OFF. The LSB of the
    first data byte contains the output addressed in the query. The other
    coils follow toward the high order end of this byte, and from low order
    to high order in subsequent bytes. 
    
    If the returned output quantity is not a multiple of eight, the 
    remaining bits in the final data byte will be padded with zeros 
    (toward the high order end of the byte). The Byte Count field specifies
    the quantity of complete bytes of data."
    """
    function_code = READ_COILS
    def __init__(self, values=None):
        ReadBitsResponseBase.__init__(self, values)
        
class ReadDiscreteInputsRequest(ReadBitsRequestBase):
    """**MODBUS Application Protocol Specification V1.1:**
    "This function code is used to read from 1 to 2000 contiguous status
    of discrete inputs in a remote device. The Request PDU specifies the
    starting address, ie the address of the first input specified, and the
    number of inputs. In the PDU Discrete Inputs are addressed starting at
    zero. Therefore Discrete inputs numbered 1-16 are addressed as 0-15."
    """
    function_code = READ_DISCRETE_INPUTS
    def __init__(self, address=None, count=None):
        ReadBitsRequestBase.__init__(self, address, count)
    def execute(self, context):
        if not (1 <= self.count <= 0x7d0):
            return self.createExceptionResponse(ILLEGAL_VALUE_EXCEPTION)
        if not context.checkDiscreteInputAddress(self.address, self.count):
            return self.createExceptionResponse(ILLEGAL_ADDRESS_EXCEPTION)
        values = context.getDiscreteInputValues(self.address, self.count)
        return ReadDiscreteInputsResponse(values)
          
class ReadDiscreteInputsResponse(ReadBitsResponseBase):
    """**MODBUS Application Protocol Specification V1.1:**
    "The discrete inputs in the response message are packed as one input per
    bit of the data field. Status is indicated as 1= ON; 0= OFF. The LSB of
    the first data byte contains the input addressed in the query. The other
    inputs follow toward the high order end of this byte, and from low order
    to high order in subsequent bytes. 
    
    If the returned input quantity is not a multiple of eight, the 
    remaining bits in the final data byte will be padded with zeros 
    (toward the high order end of the byte). The Byte Count field specifies
    the quantity of complete bytes of data."
    """
    function_code = READ_DISCRETE_INPUTS
    def __init__(self, values=None):
        ReadBitsResponseBase.__init__(self, values)

class ReadRegistersRequestBase(ModbusRequest):
    def __init__(self, address, count):
        # assert(1<=count<=125(0x7d))
        # assert(0<=address<=0xffff)
        ModbusRequest.__init__(self)
        self.address = address
        self.count = count
    def encodeData(self):
        ret = struct.pack('>HH', self.address, self.count)
        return ret
    def decodeData(self, data):
        self.address, self.count = struct.unpack('>HH', data)

class ReadRegistersResponseBase(ModbusResponse):
    def __init__(self, values):
        ModbusResponse.__init__(self)
        if values != None:
            self.registers = values
        else: self.registers = []
    def decodeData(self, data):
        byte_count = ord(data[0])
        self.registers = []
        for i in range(1, byte_count + 1, 2):
            self.registers.append(struct.unpack('>H', data[i:i+2])[0])
    def encodeData(self):
        ret = chr(len(self.registers)*2)
        for reg in self.registers:
            ret += struct.pack('>H', reg)
        return ret
    def getRegValue(self, index):
        return self.registers[index]
    

class ReadHoldingRegistersRequest(ReadRegistersRequestBase):
    """**MODBUS Application Protocol Specification V1.1:**
    "This function code is used to read the contents of a contiguous block
    of holding registers in a remote device. The Request PDU specifies the
    starting register address and the number of registers. In the PDU
    Registers are addressed starting at zero. Therefore registers numbered
    1-16 are addressed as 0-15."
    """
    function_code = READ_HOLDING_REGISTERS
    def __init__(self, address=None, count=None):
        ReadRegistersRequestBase.__init__(self, address, count)
    def execute(self, context):
        if not (1 <= self.count <= 0x7d):
            return self.createExceptionResponse(ILLEGAL_VALUE_EXCEPTION)
        if not context.checkHoldingRegisterAddress(self.address, self.count):
            return self.createExceptionResponse(ILLEGAL_ADDRESS_EXCEPTION)
        values = context.getHoldingRegisterValues(self.address, self.count)
        return ReadHoldingRegistersResponse(values)

class ReadHoldingRegistersResponse(ReadRegistersResponseBase):
    """**MODBUS Application Protocol Specification V1.1:**
    "This function code is used to read the contents of a contiguous block
    of holding registers in a remote device. The Request PDU specifies the
    starting register address and the number of registers. In the PDU
    Registers are addressed starting at zero. Therefore registers numbered
    1-16 are addressed as 0-15."
    """
    function_code = READ_HOLDING_REGISTERS
    def __init__(self, values=None):
        ReadRegistersResponseBase.__init__(self, values)
        
class ReadInputRegistersRequest(ReadRegistersRequestBase):
    """**MODBUS Application Protocol Specification V1.1:**
    "This function code is used to read from 1 to approx. 125 contiguous
    input registers in a remote device. The Request PDU specifies the
    starting register address and the number of registers. In the PDU
    Registers are addressed starting at zero. Therefore input registers
    numbered 1-16 are addressed as 0-15."
    """
    function_code = READ_INPUT_REGISTERS
    def __init__(self, address=None, count=None):
        ReadRegistersRequestBase.__init__(self, address, count)
    def execute(self, context):
        if not (1 <= self.count <= 0x7d):
            return self.createExceptionResponse(ILLEGAL_VALUE_EXCEPTION)
        if not context.checkInputRegisterAddress(self.address, self.count):
            return self.createExceptionResponse(ILLEGAL_ADDRESS_EXCEPTION)
        values = context.getInputRegisterValues(self.address, self.count)
        return ReadInputRegistersResponse(values)

class ReadInputRegistersResponse(ReadRegistersResponseBase):
    """**MODBUS Application Protocol Specification V1.1:**
    "This function code is used to read from 1 to approx. 125 contiguous
    input registers in a remote device. The Request PDU specifies the
    starting register address and the number of registers. In the PDU
    Registers are addressed starting at zero. Therefore input registers
    numbered 1-16 are addressed as 0-15."
    """
    function_code = READ_INPUT_REGISTERS
    def __init__(self, values=None):
        ReadRegistersResponseBase.__init__(self, values)
        
class WriteSingleCoilRequest(ModbusRequest):
    """**MODBUS Application Protocol Specification V1.1:**
    "This function code is used to write a single output to either ON or OFF
    in a remote device. 
    
    The requested ON/OFF state is specified by a constant in the request
    data field. A value of FF 00 hex requests the output to be ON. A value
    of 00 00 requests it to be OFF. All other values are illegal and will 
    not affect the output.
    
    The Request PDU specifies the address of the coil to be forced. Coils
    are addressed starting at zero. Therefore coil numbered 1 is addressed
    as 0. The requested ON/OFF state is specified by a constant in the Coil
    Value field. A value of 0XFF00 requests the coil to be ON. A value of
    0X0000 requests the coil to be off. All other values are illegal and
    will not affect the coil."
    """
    function_code = WRITE_SINGLE_COIL
    def __init__(self, address=None, value=None):
        ModbusRequest.__init__(self)
        self.address = address
        if value:
            self.value = 0xff00
        else:
            self.value = 0
    def encodeData(self):
        ret = struct.pack('>H', self.address)
        if self.value:
            ret += COIL_ON_BYTES
        else:
            ret += COIL_OFF_BYTES
        return ret
    def decodeData(self, data):
        self.address, self.value = struct.unpack('>HH', data)
    def execute(self, context):
        if self.value != 0 and self.value != 0xff00:
            return self.createExceptionResponse(ILLEGAL_VALUE_EXCEPTION)
        if not context.checkCoilAddress(self.address):
            return self.createExceptionResponse(ILLEGAL_ADDRESS_EXCEPTION)
        context.setCoilValues(self.address, [self.value == 0xff00])
        values = context.getCoilValues(self.address)
        return WriteSingleCoilResponse(self.address, values[0])

class WriteSingleCoilResponse(ModbusResponse):
    """**MODBUS Application Protocol Specification V1.1:**
    "The normal response is an echo of the request, returned after the coil
    state has been written."
    """
    function_code = WRITE_SINGLE_COIL 
    def __init__(self, address=None, value=None):
        ModbusResponse.__init__(self)
        self.address = address
        self.value = value
    def encodeData(self):
        ret = struct.pack('>H', self.address)
        if self.value:
            ret += COIL_ON_BYTES
        else:
            ret += COIL_OFF_BYTES
        return ret
    def decodeData(self, data):
        self.address, value = struct.unpack('>HH', data)
        self.value = (value != 0)
        
class WriteSingleRegisterRequest(ModbusRequest):
    """**MODBUS Application Protocol Specification V1.1:**
    "This function code is used to write a single holding register in a
    remote device. 

    The Request PDU specifies the address of the register to
    be written. Registers are addressed starting at zero. Therefore register
    numbered 1 is addressed as 0."
    """
    function_code = WRITE_SINGLE_REGISTER 
    def __init__(self, address=None, value=None):
        ModbusRequest.__init__(self)
        self.address = address
        self.value = value
    def encodeData(self):
        ret = struct.pack('>HH', self.address, self.value)
        return ret
    def decodeData(self, data):
        self.address, self.value = struct.unpack('>HH', data)
    def execute(self, context):
        if not (0 <= self.value <= 0xffff):
            return self.createExceptionResponse(ILLEGAL_VALUE_EXCEPTION)
        if not context.checkHoldingRegisterAddress(self.address):
            return self.createExceptionResponse(ILLEGAL_ADDRESS_EXCEPTION)
        context.setHoldingRegisterValues(self.address, [self.value])
        values = context.getHoldingRegisterValues(self.address)
        return WriteSingleRegisterResponse(self.address, values[0])

class WriteSingleRegisterResponse(ModbusResponse):
    """**MODBUS Application Protocol Specification V1.1:**
    "The normal response is an echo of the request, returned after the
    register contents have been written."
    """
    function_code = WRITE_SINGLE_REGISTER 
    def __init__(self, address=None, value=None):
        ModbusResponse.__init__(self)
        self.address = address
        self.value = value
    def encodeData(self):
        ret = struct.pack('>HH', self.address, self.value)
        return ret
    def decodeData(self, data):
        self.address, self.value = struct.unpack('>HH', data)
        
class WriteMultipleCoilsRequest(ModbusRequest):
    """**MODBUS Application Protocol Specification V1.1:**
    "This function code is used to force each coil in a sequence of coils to
    either ON or OFF in a remote device. The Request PDU specifies the coil
    references to be forced. Coils are addressed starting at zero. Therefore
    coil numbered 1 is addressed as 0. 
    
    The requested ON/OFF states are specified by contents of the request
    data field. A logical '1' in a bit position of the field requests the
    corresponding output to be ON. A logical '0' requests it to be OFF."
    """
    function_code = WRITE_MULTIPLE_COILS
    def __init__(self, address=None, count=None):
        ModbusRequest.__init__(self)
        self.address = address
        if count != None and count > 0:
            self.coils = [False] * count
        else: self.coils = []
    def encodeData(self):
        count = len(self.coils)
        ret = struct.pack('>HHB', self.address, count, (count + 7) / 8)
        ret += packBitsToString(self.coils)
        return ret
    def decodeData(self, data):
        self.address, count = struct.pack('>HH', data[0:2])
        coils, self.byte_count = unpackBitsFromString(data[2:])
        self.coils = coils[:count]
    def execute(self, context):
        count = len(self.coils)
        if not (1 <= count <= 0x07b0):
            return self.createExceptionResponse(ILLEGAL_VALUE_EXCEPTION)
        if (self.byte_count != (count + 7) / 8):
            return self.createExceptionResponse(ILLEGAL_VALUE_EXCEPTION)
        if not context.checkCoilAddress(self.address, count):
            return self.createExceptionResponse(ILLEGAL_ADDRESS_EXCEPTION)
        context.setCoilValues(self.address, self.coils)
        return WriteMultipleCoilsResponse(self.address, count)

class WriteMultipleCoilsResponse(ModbusResponse):
    """**MODBUS Application Protocol Specification V1.1:**
    "The normal response returns the function code, starting address, and
    quantity of coils forced."
    """
    function_code = WRITE_MULTIPLE_COILS
    def __init__(self, address=None, count=None):
        ModbusResponse.__init__(self)
        self.address = address
        self.count = count
    def encodeData(self):
        ret = struct.pack('>HH', self.address, self.count)
        return ret
    def decodeData(self, data):
        self.address, self.count = struct.unpack('>HH', data)

class WriteMultipleRegistersRequest(ModbusRequest):
    """**MODBUS Application Protocol Specification V1.1:**
    "This function code is used to write a block of contiguous registers (1
    to approx. 120 registers) in a remote device.
    
    The requested written values are specified in the request data field.
    Data is packed as two bytes per register."
    """
    function_code = WRITE_MULTIPLE_REGISTERS 
    def __init__(self, address=None, count=None):
        ModbusRequest.__init__(self)
        self.address = address
        if count != None and count > 0:
            self.registers = [0] * count
        else: self.registers = []
    def encodeData(self):
        count = len(self.registers)
        ret = struct.pack('>HHB', self.address, count, count*2) 
        for reg in self.registers:
            ret += struct.pack('>H', reg)
        return ret
    def decodeData(self, data):
        self.address, count, self.byte_count = struct.unpack('>HHB', data[:5])
        for i in range(5, count*2+5, 2):
            self.registers.append(struct.unpack('>H', data[i:i+2])[0])
    def execute(self, context):
        count = len(self.registers)
        if not (1 <= count <= 0x07b):
            return self.createExceptionResponse(ILLEGAL_VALUE_EXCEPTION)
        if (self.byte_count != count * 2):
            return self.createExceptionResponse(ILLEGAL_VALUE_EXCEPTION)
        if not context.checkHoldingRegisterAddress(self.address, count):
            return self.createExceptionResponse(ILLEGAL_ADDRESS_EXCEPTION)
        context.setHoldingRegisterValues(self.address, self.registers)
        return WriteMultipleRegistersResponse(self.address, count)

class WriteMultipleRegistersResponse(ModbusResponse):
    """**MODBUS Application Protocol Specification V1.1:**
    "The normal response returns the function code, starting address, and
    quantity of registers written."
    """
    function_code = WRITE_MULTIPLE_REGISTERS 
    def __init__(self, address=None, count=None):
        ModbusResponse.__init__(self)
        self.address = address
        self.count = count
    def encodeData(self):
        ret = struct.pack('>HH', self.address, self.count)
        return ret
    def decodeData(self, data):
        self.address, self.count = struct.unpack('>HH', data)

class ReadWriteMultipleRegistersRequest(ModbusRequest):
    """**MODBUS Application Protocol Specification V1.1:**
    "This function code performs a combination of one read operation and one
    write operation in a single MODBUS transaction. The write
    operation is performed before the read.
    
    Holding registers are addressed starting at zero. Therefore holding
    registers 1-16 are addressed in the PDU as 0-15.
    
    The request specifies the starting address and number of holding
    registers to be read as well as the starting address, number of holding
    registers, and the data to be written. The byte count specifies the
    number of bytes to follow in the write data field."
    """ 
    function_code = READ_WRITE_MULTIPLE_REGISTERS 
    def __init__(self, raddress=None, rcount=None, waddress=None,
    wcount=None):
        ModbusRequest.__init__(self)
        self.raddress = raddress
        self.rcount = rcount
        self.waddress = waddress
        if wcount != None and wcount > 0:
            self.wregisters = [0] * wcount
        else: self.wregisters = []
    def encodeData(self):
        wcount = len(self.wregisters)
        ret = struct.pack('>HHHHB',  
            self.raddress, self.rcount, \
            self.waddress, wcount, \
            wcount*2)
        for reg in self.wregisters:
            ret += struct.pack('>H', reg)
        return ret
    def decodeData(self, data):
        self.raddress, self.rcount, \
        self.waddress, wcount, \
        self.wbyte_count = \
        struct.unpack('>HHHHB', data[:9])
        self.wregisters = []
        for i in range(9, self.wbyte_count+9, 2):
            self.wregisters.append(struct.unpack('>H', data[i:i+2])[0])
    def execute(self, context):
        wcount = len(self.wregisters)
        if not (1 <= self.rcount <= 0x07d):
            return self.createExceptionResponse(ILLEGAL_VALUE_EXCEPTION)
        if not (1 <= wcount <= 0x079):
            return self.createExceptionResponse(ILLEGAL_VALUE_EXCEPTION)
        if (self.wbyte_count != wcount * 2):
            return self.createExceptionResponse(ILLEGAL_VALUE_EXCEPTION)
        if not context.checkHoldingRegisterAddress(self.waddress, wcount):
            return self.createExceptionResponse(ILLEGAL_ADDRESS_EXCEPTION)
        if not context.checkHoldingRegisterAddress(self.raddress, 
            self.rcount):
            return self.createExceptionResponse(ILLEGAL_ADDRESS_EXCEPTION)
        context.setHoldingRegisterValues(self.waddress, self.wregisters)
        rvalues = context.getHoldingRegisterValues(self.raddress,
            self.rcount)  
        return ReadWriteMultipleRegistersResponse(rvalues)

class ReadWriteMultipleRegistersResponse(ModbusResponse):
    """**MODBUS Application Protocol Specification V1.1:**
    "The normal response contains the data from the group of registers that
    were read. The byte count field specifies the quantity of bytes to
    follow in the read data field."
    """
    function_code = READ_WRITE_MULTIPLE_REGISTERS 
    def __init__(self, rvalues=None):
        ModbusResponse.__init__(self)
        if rvalues != None:
            self.rregisters = rvalues
        else: self.rregisters = []
    def encodeData(self):
        ret = chr(len(self.rregisters)*2)
        for reg in self.rregisters:
            ret += struct.pack('>H', reg)
        return ret
    def decodeData(self, data):
        bytes = ord(data[0])
        for i in range(1, bytes, 2):
            self.rregisters.append(struct.unpack('>H', data[i:i+2])[0])
        
class ExceptionResponse(ModbusResponse):
    def __init__(self, function_code, exception_code=None):
        ModbusResponse.__init__(self)
        self.function_code = function_code + EXCEPTION_OFFSET
        self.exception_code = exception_code
    def encodeData(self):
        ret = chr(self.exception_code)
        return ret
    def decodeData(self, data):
        self.exception_code = ord(data[0])
        
def decodeModbusRequestPDU(data):
    function_code = ord(data[0])
    if function_code == READ_HOLDING_REGISTERS:
        request = ReadHoldingRegistersRequest()
    elif function_code == READ_DISCRETE_INPUTS:
        request = ReadDiscreteInputsRequest()
    elif function_code == READ_INPUT_REGISTERS:
        request = ReadInputRegistersRequest()
    elif function_code == READ_COILS:
        request = ReadCoilsRequest()
    elif function_code == WRITE_MULTIPLE_COILS:
        request = WriteMultipleCoilsRequest()
    elif function_code == WRITE_MULTIPLE_REGISTERS:
        request = WriteMultipleRegistersRequest()
    elif function_code == WRITE_SINGLE_REGISTER:
        request = WriteSingleRegisterRequest()
    elif function_code == WRITE_SINGLE_COIL:
        request = WriteSingleCoilRequest()
    elif function_code == READ_WRITE_MULTIPLE_REGISTERS:
        request = ReadWriteMultipleRegistersRequest()
    else:
        l.debug("Unknown request received: %d" % function_code)
        request = IllegalFunctionRequest(function_code)
    request.decodeData(data[1:])
    return request           

def decodeModbusResponsePDU(data):
    function_code = ord(data[0])
    if function_code == READ_HOLDING_REGISTERS:
        response = ReadHoldingRegistersResponse()
    elif function_code == READ_DISCRETE_INPUTS:
        response = ReadDiscreteInputsResponse()
    elif function_code == READ_INPUT_REGISTERS:
        response = ReadInputRegistersResponse()
    elif function_code == READ_COILS:
        response = ReadCoilsResponse()
    elif function_code == WRITE_MULTIPLE_COILS:
        response = WriteMultipleCoilsResponse()
    elif function_code == WRITE_MULTIPLE_REGISTERS:
        response = WriteMultipleRegistersResponse()
    elif function_code == WRITE_SINGLE_REGISTER:
        response = WriteSingleRegisterResponse()
    elif function_code == WRITE_SINGLE_COIL:
        response = WriteSingleCoilResponse()
    elif function_code == READ_WRITE_MULTIPLE_REGISTERS:
        response = ReadWriteMultipleRegistersResponse()
    elif function_code > 0x80:
        response = ExceptionResponse(function_code - 0x80)
        l.debug("Exception response received %d." % ord(data[1]))
    else:
        raise ModbusException("Unknown responsee %d" % function_code)
    response.decodeData(data[1:])
    return response
    

class ModbusTransport:
    """Interface defining the I/O mechanisms for
       ``ModbusPDU`` instances.
    """
    def close(self):
        """Closes the raw input and output streams of
           this ``ModbusTransport``.
        """
        pass
    def writeMessage(self, msg):
        """Writes a ``ModbusPDU`` to the
           output stream of this ``ModbusTransport``.
        """
        pass
    def readRequest(self):
        """Reads a ``ModbusRequest`` from the
           input stream of this ``ModbusTransport``.
        """
        pass
    def readResponse(self):
        """Reads a ``ModbusResponse`` from the
           input stream of this ``ModbusTransport``.
        """
        pass

class ModbusTCPTransport(ModbusTransport):
    def __init__(self, socket):
        self.socket = socket
    def setSocket(self, socket):
        self.socket = socket
    def writeMessage(self, message):
        data = message.encodeData()
        self.socket.send(
            struct.pack('>HHHBB',
            message.transaction_id,
            message.protocol_id,
            len(data)+2, 
            message.unit_id,
            message.function_code) + 
            data)
    def readResponse(self):
        data = self.socket.recv(7)
        if len(data) == 0:
            raise ModbusIOException("Remote party has closed connection.")
        if len(data) != 7:
            raise ModbusIOException(
                "Received less bytes (%d) than required." % len(data))
        transaction_id, protocol_id, data_length, unit_id \
            = struct.unpack('>HHHB', data)
        if data_length > 1:
            data = self.socket.recv(data_length - 1)
            if len(data) == 0:
                raise ModbusIOException("Remote party has closed connection.")
            if len(data) != (data_length - 1):
                raise ModbusIOException(
                    "Received less bytes (%d) than required." % len(data))
        else:
            raise ModbusIOException(
                "Wrong response packet received.")
        response = decodeModbusResponsePDU(data)
        response.transaction_id = transaction_id
        response.protocol_id = protocol_id
        response.unit_id = unit_id
        return response 
    def readRequest(self):
        data = self.socket.recv(7)
        if len(data) == 0:
            raise ModbusIOException("Remote party has closed connection.")
        if len(data) != 7:
            raise ModbusIOException(
                "Received less bytes (%d) than required." % len(data))
        transaction_id, protocol_id, data_length, unit_id \
            = struct.unpack('>HHHB', data)
        if protocol_id != DEFAULT_PROTOCOL_ID or data_length > 256:
            raise ModbusIOException("Wrong request packed received");
        if data_length > 1:
            data = self.socket.recv(data_length - 1)
            if len(data) == 0:
                raise ModbusIOException("Remote party has closed connection.")
            if len(data) != (data_length - 1):
                raise ModbusIOException(
                    "Received less bytes (%d) than required." % len(data))
        else:
            raise ModbusIOException(
                "Wrong request packet received.")
        request = decodeModbusRequestPDU(data)
        request.transaction_id = transaction_id
        request.protocol_id = protocol_id
        request.unit_id = unit_id
        return request

class ModbusDataBlock:
    def __init__(self, address, count, default_value):
        self.address = address
        self.default_value = default_value
        self.values = [default_value] * count
    def reset(self):
        for i in self.values:
            i = self.default_value

class ModbusServerContext:
    def __init__(self, 
        discrete_inputs_address=0, discrete_inputs_count=0,
        coils_address=0, coils_count=0,
        input_registers_address=0, input_registers_count=0,
        holding_registers_address=0, holding_registers_count=0):
        self.di = ModbusDataBlock(discrete_inputs_address,
            discrete_inputs_count, False)
        self.co = ModbusDataBlock(coils_address, coils_count, False)
        self.ir = ModbusDataBlock(input_registers_address, 
            input_registers_count, 0)
        self.hr = ModbusDataBlock(holding_registers_address,
            holding_registers_count, 0)
    def reset(self):
        self.di.reset()
        self.co.reset()
        self.ir.reset()
        self.hr.reset()
    def checkAddress(self, data_block, address, count=1):
        if data_block.address > address: return False
        if ((data_block.address + len(data_block.values)) < 
           (address + count)): return False
        return True
    def checkCoilAddress(self, address, count=1):
        return self.checkAddress(self.co, address, count)
    def checkDiscreteInputAddress(self, address, count=1):
        return self.checkAddress(self.di, address, count)
    def checkInputRegisterAddress(self, address, count=1):
        return self.checkAddress(self.ir, address, count)
    def checkHoldingRegisterAddress(self, address, count=1):
        return self.checkAddress(self.hr, address, count)
    def getValues(self, data_block, address, count=1):
        b = address - data_block.address
        ret = data_block.values[b:b+count]
        return ret
    def getCoilValues(self, address, count=1):
        return self.getValues(self.co, address, count)
    def getDiscreteInputValues(self, address, count=1):
        return self.getValues(self.di, address, count)
    def getInputRegisterValues(self, address, count=1):
        return self.getValues(self.ir, address, count)
    def getHoldingRegisterValues(self, address, count=1):
        return self.getValues(self.hr, address, count)
    def setValues(self, data_block, address, values):
        b = address - data_block.address
        data_block.values[b:b+len(values)] = values
    def setCoilValues(self, address, values):
        self.setValues(self.co, address, values)
    def setHoldingRegisterValues(self, address, values):
        self.setValues(self.hr, address, values)

class ModbusTCPServer(SocketServer.ThreadingTCPServer):
    def __init__(self, port=DEFAULT_PORT, context=None):
        self.port = port
        self.threads = []
        if isinstance(context, ModbusServerContext):
            self.ctx = context
        else: self.ctx = ModbusServerContext()
        SocketServer.ThreadingTCPServer.__init__(self, ("", port), ModbusTCPRequestHandler)
    def process_request(self, request, client_address):
        l.info("Started new thread to serve client at " + str(client_address))
        SocketServer.ThreadingTCPServer.process_request(self, request,
            client_address)
    def server_close(self):
        l.debug("server_close() called")
        self.socket.close()
        for t in self.threads:
            t.run = False

class ModbusTCPRequestHandler(SocketServer.BaseRequestHandler):
    def __init__(self, request, client_address, server):
        self.run = True
        SocketServer.BaseRequestHandler.__init__(self, request, client_address, server)
    def handle(self):
        self.server.threads.append(self)
        try:
            self.request.settimeout(DEFAULT_TIMEOUT)
            transport = ModbusTCPTransport(self.request)
            while self.run:
                try:    
                    request = transport.readRequest()
                    try:
                        response = request.execute(self.server.ctx)
                    except:
                        dumpExceptionInfo()
                        response = request.createExceptionResponse(
                            SLAVE_DEVICE_FAILURE_EXCEPTION)
                    response.transaction_id = request.transaction_id
                    response.unit_id = request.unit_id
                    transport.writeMessage(response)
                except socket.timeout:
                    pass
                except socket.error, msg:
                    l.info("Socket error ocured:" + str(msg))
                    self.run = False
                except ModbusIOException, msg:
                    l.info(msg)
                    self.run = False
                except KeyboardInterrupt:
                    import thread
                    thread.interrupt_main()
        finally:
            self.server.threads.remove(self)
            l.info("Exiting thread that served client at " + 
                str(self.client_address))
            
class TCPMasterConnection:
    """Class that implements a TCPMasterConnection."""
    def __init__(self, host, port=DEFAULT_PORT):
        self.sock = None
        self.host = host
        self.port = port
        self.timeout = DEFAULT_TIMEOUT
        self.modbus_transport = None
    
    def connect(self):
        """Connect to the host and port specified in __init__."""
        if self.sock:
            return
        msg = "getaddrinfo returns an empty list"
        for res in socket.getaddrinfo(self.host, self.port, 0,
                                      socket.SOCK_STREAM):
            af, socktype, proto, canonname, sa = res
            try:
                self.sock = socket.socket(af, socktype, proto)
                self.sock.settimeout(self.timeout)
                l.info("Connecting to: (%s, %s)" % (self.host, self.port))
                self.sock.connect(sa)
            except socket.error, msg:
                l.warn('Connect to (%s, %s) failed: %s' % \
                    (self.host, self.port, msg))
                if self.sock:
                    self.sock.close()
                self.sock = None
                continue
            break
        if not self.sock:
            raise socket.error, msg
        else:
            self.prepareTransport()
    
    def prepareTransport(self):
        if not self.modbus_transport:
            self.modbus_transport = ModbusTCPTransport(self.sock)
        else:
            self.modbus_transport.setSocket(self.sock)

    def getModbusTransport(self):
        return self.modbus_transport
    
    def close(self):
        if self.sock:
            self.sock.close()
            self.sock = None
    
#    def setTimeout(self, timeout):
#        self.timeout = timeout
#        if self.sock:
#            self.sock.settimeout(self.timeout)

    def createTransaction(self, request=None):
        return ModbusTCPTransaction(self, request)

def test():
    print "Testing modlib:"
    print '.',
    r = ReadBitsResponseBase([0]*13)
    r.bits[0] = True
    r.bits[7] = True
    r.bits[8] = True
    assert r.encodeData() == struct.pack('BBB', 2, 129, 1)
    print '.',
    r = ReadBitsResponseBase([0]*8)
    r.bits[0] = True
    r.bits[7] = True
    assert r.encodeData() == struct.pack('BB', 1, 129)
    print '.',
    r = ReadBitsResponseBase([0]*16)
    r.bits[0] = True
    r.bits[7] = True
    r.bits[15] = True
    assert r.encodeData() == struct.pack('BBB', 2, 129, 128)
    print '.',
    m = WriteMultipleRegistersRequest(10, 10)
    m1 = WriteMultipleRegistersRequest()
    m.registers[1] = 10212
    m.registers[9] = 1024
    m1.decodeData(m.encodeData())
    assert m1.registers[1] == 10212
    assert m1.registers[9] == 1024
    assert len(m1.registers) == 10
    print '.',
    m = ReadWriteMultipleRegistersRequest(1,2,3,10)
    m1 = ReadWriteMultipleRegistersRequest()
    m.wregisters[1] = 10212
    m.wregisters[9] = 1024
    m1.decodeData(m.encodeData())
    assert m1.wregisters[1] == 10212
    assert m1.wregisters[9] == 1024
    assert len(m1.wregisters) == 10
    print '.',
    ctx = ModbusServerContext(
        discrete_inputs_address=10, 
        discrete_inputs_count=2,
        input_registers_address=2,
        input_registers_count=1
    )
    assert not ctx.checkCoilAddress(0)
    assert not ctx.checkCoilAddress(1)
    assert not ctx.checkCoilAddress(-1)
    assert not ctx.checkDiscreteInputAddress(9)
    assert ctx.checkDiscreteInputAddress(10)
    assert ctx.checkDiscreteInputAddress(11)
    assert ctx.checkDiscreteInputAddress(10, 2)
    assert not ctx.checkDiscreteInputAddress(10, 3)
    assert not ctx.checkDiscreteInputAddress(9, 2)
    assert not ctx.checkInputRegisterAddress(2, 2)
    assert not ctx.checkInputRegisterAddress(1, 2)
    assert not ctx.checkInputRegisterAddress(1)
    assert ctx.checkInputRegisterAddress(2)
    assert not ctx.checkInputRegisterAddress(3)
    
    print " done"

if __name__ == "__main__":
    test()
    
# vim: set et ts=4 sw=4 tw=76 si:
# $Id: modlib.py 408 2005-03-03 16:42:04Z hynek $
