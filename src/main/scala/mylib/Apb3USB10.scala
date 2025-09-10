package mylib

import java.math._
import spinal.core._
import spinal.lib._
import spinal.lib.io.TriState
import spinal.lib.Counter
import spinal.lib.bus.amba3.apb.{Apb3, Apb3Config, Apb3SlaveFactory}
import spinal.lib.misc.HexTools

class USB_IO extends Bundle {
    val usb_dm    = inout(Analog(Bool()))
    val usb_dp    = inout(Analog(Bool()))
    val valid     = in Bool()
    val ready     = out Bool()
    val clock_div = in UInt(8 bits)
    val test      = out Bool()

    ready := False // default value, it will be changed in implementation
}

class USBSendReceive(var hasStrobe  : Boolean = true,
                     var hasSendKJ  : Boolean = true,
                     var szBitcount : Int = 0,
                     var hasCRC16   : Boolean = false) extends Component {

    if(hasSendKJ) { hasStrobe = true; if(szBitcount == 0) szBitcount = 13;  }

    val clock_div    = (hasStrobe   ) generate Reg(UInt(8 bits)).addTag(crossClockDomain)
    val clock_strobe = (hasStrobe   ) generate False
    val bit_count    = (szBitcount>0) generate Reg(UInt(szBitcount bits)).addTag(crossClockDomain)
    val stuffing     = (hasSendKJ   ) generate False
    val ones         = (hasSendKJ   ) generate Reg(UInt(3 bits)).addTag(crossClockDomain)
    val last_kj      = (hasSendKJ   ) generate Reg(Bool()).addTag(crossClockDomain)

    def reset_last_kj(io: USB_IO) = hasSendKJ generate {
      when(!io.valid) {
        last_kj := False // 'J'
      }
    }

    def sendKJ(io: USB_IO, input_bit: Bool) = hasSendKJ generate {
      val new_kj = Bool()

      // convert bit to K/J symbol depending on last symbol sent
      when(clock_strobe && ((input_bit === False) || stuffing)) {
        new_kj := !last_kj // Transition
        last_kj := new_kj
      } otherwise { // No transition
        new_kj := last_kj
      }

      // Transmit symbol, USB 1.0 Low Speed
      when(new_kj) { // 'K' (True)
        io.usb_dm := False
        io.usb_dp := True
      } otherwise { // 'J' (False)
        io.usb_dm := True
        io.usb_dp := False
      }
    }

    def calc_crc16_usb(crc_in: Bits, din: Bits) : Bits = hasCRC16 generate {
      val ret = Bits(16 bits)

	ret(0) :=	din(7) ^ din(6) ^ din(5) ^ din(4) ^ din(3) ^
        		din(2) ^ din(1) ^ din(0) ^ crc_in(8) ^ crc_in(9) ^
        		crc_in(10) ^ crc_in(11) ^ crc_in(12) ^ crc_in(13) ^
        		crc_in(14) ^ crc_in(15)
	ret(1) :=	din(7) ^ din(6) ^ din(5) ^ din(4) ^ din(3) ^ din(2) ^
        		din(1) ^ crc_in(9) ^ crc_in(10) ^ crc_in(11) ^
        		crc_in(12) ^ crc_in(13) ^ crc_in(14) ^ crc_in(15)
	ret(2) :=	din(1) ^ din(0) ^ crc_in(8) ^ crc_in(9)
	ret(3) :=	din(2) ^ din(1) ^ crc_in(9) ^ crc_in(10)
	ret(4) :=	din(3) ^ din(2) ^ crc_in(10) ^ crc_in(11)
	ret(5) :=	din(4) ^ din(3) ^ crc_in(11) ^ crc_in(12)
	ret(6) :=	din(5) ^ din(4) ^ crc_in(12) ^ crc_in(13)
	ret(7) :=	din(6) ^ din(5) ^ crc_in(13) ^ crc_in(14)
	ret(8) :=	din(7) ^ din(6) ^ crc_in(0) ^ crc_in(14) ^ crc_in(15)
	ret(9) :=	din(7) ^ crc_in(1) ^ crc_in(15)
	ret(10) :=	crc_in(2)
	ret(11) :=	crc_in(3)
	ret(12) :=	crc_in(4)
	ret(13) :=	crc_in(5)
	ret(14) :=	crc_in(6)
	ret(15) :=	din(7) ^ din(6) ^ din(5) ^ din(4) ^ din(3) ^ din(2) ^
			din(1) ^ din(0) ^ crc_in(7) ^ crc_in(8) ^ crc_in(9) ^
			crc_in(10) ^ crc_in(11) ^ crc_in(12) ^ crc_in(13) ^
			crc_in(14) ^ crc_in(15)

      return ret
    }


    def make_clock_strobe(io: USB_IO) = hasStrobe generate {
      when(io.valid) {
        clock_div := clock_div + 1
        when(clock_div === io.clock_div) {
          clock_div := 0
        }
        when(clock_div === 0) {
          clock_strobe := True
        }
      } otherwise {
        clock_div := 0
      }
    }

    def inc_bit_count(io: USB_IO) = (szBitcount > 0) generate {
      when(io.valid) {
        when(clock_strobe) {
          bit_count := bit_count + 1
        }
      } otherwise {
        bit_count := 0
      }
    }

    def make_stuffing(io: USB_IO, bit_to_send: Bool) = hasSendKJ generate {
      when(io.valid) {

        when(ones === 6) {
          stuffing := True
        }

        when(clock_strobe) {

          when(bit_to_send) { // Count ones if bit_to_send == '1' 
            ones := ones + 1
          } otherwise {
            ones := 0
          }

          when(stuffing) {
            ones := 0
          }

          when(!stuffing) { // Advance bit_count only if not stuffing zero
            bit_count := bit_count + 1
          }
        }

        when(stuffing) { // Substitute current bit to '0' if stuffing
          bit_to_send := False
        }


      } otherwise {
        ones := 0
        bit_count := 0
      }
    }


}

case class USBSendLongToken() extends USBSendReceive(szBitcount = 6) {

    val io = new USB_IO {
	val pid       = in Bits(4 bits)
	val addr      = in Bits(7 bits)
	val endp      = in Bits(4 bits)
    }

    def calc_crc5_usb(din: Bits) : Bits = {
      val ret = Bits(5 bits)
      val din_rev = din.reversed;
      ret(0) := din_rev(10) ^ din_rev(9) ^ din_rev(6) ^ din_rev(5) ^ din_rev(3) ^ din_rev(0) ^ True
      ret(1) := din_rev(10) ^ din_rev(7) ^ din_rev(6) ^ din_rev(4) ^ din_rev(1) ^ True
      ret(2) := din_rev(10) ^ din_rev(9) ^ din_rev(8) ^ din_rev(7) ^ din_rev(6) ^ din_rev(3) ^ din_rev(2) ^ din_rev(0) ^ True
      ret(3) := din_rev(10) ^ din_rev(9) ^ din_rev(8) ^ din_rev(7) ^ din_rev(4) ^ din_rev(3) ^ din_rev(1)
      ret(4) := din_rev(10) ^ din_rev(9) ^ din_rev(8) ^ din_rev(5) ^ din_rev(4) ^ din_rev(2) ^ True
      return ret.reversed ^ B"11111"
    }


    val crc5_out = calc_crc5_usb(io.endp ## io.addr)
    val buffer = crc5_out ## io.endp ## io.addr ## ~io.pid ## io.pid ## B"10000000"
    val bit_to_send = buffer(bit_count(4 downto 0))

    make_clock_strobe(io)
    inc_bit_count(io)
    reset_last_kj(io)

    io.test := clock_strobe

    when(io.valid) {
      when(bit_count === 36) { // Ready, EOP: 'J'
        io.usb_dm := True
        io.usb_dp := False
        io.ready := True
        bit_count := 36
      } elsewhen(bit_count === 35) { // EOP: 'J'
        io.usb_dm := True
        io.usb_dp := False
      } elsewhen((bit_count === 33) || (bit_count === 34)) { // EOP: 'SE0'
        io.usb_dm := False
        io.usb_dp := False
      } elsewhen(bit_count === 32 && clock_strobe) { // EOP: 'SE0' - coner case
        io.usb_dm := False
        io.usb_dp := False
      } otherwise {
        sendKJ(io, bit_to_send)
      }

    }
}

case class USBSendShortToken() extends USBSendReceive(szBitcount = 5) {
    val io = new USB_IO {
	val pid       = in Bits(4 bits)
    }

    val buffer = ~io.pid ## io.pid ## B"10000000"
    val bit_to_send = buffer(bit_count(3 downto 0))

    make_clock_strobe(io)
    inc_bit_count(io)
    reset_last_kj(io)

    io.test := clock_strobe

    when(io.valid) {
      when(bit_count === 20) { // Ready, EOP: 'J'
        io.usb_dm := True
        io.usb_dp := False
        io.ready := True
        bit_count := 20
      } elsewhen(bit_count === 19) { // EOP: 'J'
        io.usb_dm := True
        io.usb_dp := False
      } elsewhen(bit_count === 18 && clock_strobe) { // EOP: 'SE0' - coner case
        io.usb_dm := True
        io.usb_dp := False
      } elsewhen((bit_count === 17) || (bit_count === 18)) { // EOP: 'SE0'
        io.usb_dm := False
        io.usb_dp := False
      } elsewhen(bit_count === 16 && clock_strobe) { // EOP: 'SE0' - coner case
        io.usb_dm := False
        io.usb_dp := False
      } otherwise {
        sendKJ(io, bit_to_send)
      }

    }
}

case class USBSendData() extends USBSendReceive(hasCRC16 = true, szBitcount = 13) {
    val io = new USB_IO {
	val pid       = in Bits(4 bits)
	val data      = in Bits(64 bits)
	val len       = in UInt(12 bits)
    }

    val crc16 = Reg(Bits(16 bits)).addTag(crossClockDomain) init(B"16'hffff")
    val crc_byte = Reg(Bits(8 bits))
    val sync_pid_buffer = ~io.pid ## io.pid ## B"10000000"
    val state = Reg(UInt(3 bits)).addTag(crossClockDomain) init(0)
    val bit_to_send = False

    make_clock_strobe(io)
    make_stuffing(io, bit_to_send)
    reset_last_kj(io)

    io.test := clock_strobe //io.valid

    when(io.valid) {

      switch(state) {
        is(0) { // sending SYNC + PID
          bit_to_send := sync_pid_buffer(bit_count(3 downto 0))
          sendKJ(io, bit_to_send)
          when(clock_strobe && bit_count === 15) {
            bit_count := 0
            state := 1 // send data
            when(io.len === 0xfff) { // zero len ?
              state := 2 // send CRC16
            }
          }
        }
        is(1) { // sending DATA block
          bit_to_send := io.data(bit_count(5 downto 0))
          when(clock_strobe && !stuffing) {
            val crc_byte_next = bit_to_send ## crc_byte(7 downto 1)
            when(bit_count(2 downto 0) === U"111") {
              crc16 := calc_crc16_usb(crc16, crc_byte_next.reversed)
            }
            when(bit_count === io.len) {
              state := 2
              bit_count := 0
            }
            crc_byte := crc_byte_next
          }
          // J: D- = 1, D+ = 0, K: D- = 0, D+ = 1
          // KJKJKJKK + PID + DATA + CRC16
          sendKJ(io, bit_to_send)
        }
        is(2) { // sending CRC16 block
          // CRC out is reversed and XORed
          val crc_rev = ~crc16.reversed
          bit_to_send := crc_rev(bit_count(3 downto 0))
          when(clock_strobe && bit_count === 16) {
            io.usb_dm := False
            io.usb_dp := False
            state := 3
          } otherwise {
            sendKJ(io, bit_to_send)
          }
        }
        is(3) { // sending EOP: 'SE0'
          io.usb_dm := False
          io.usb_dp := False
          when(clock_strobe) {
            state := 4
          }
        }
        is(4) { // sending EOP: 'SE0'
          io.usb_dm := False
          io.usb_dp := False
          when(clock_strobe) {
            io.usb_dm := True // sending EOP: 'J'
            state := 5
          }
        }
        is(5) { // sending EOP: 'J'
          io.usb_dm := True
          io.usb_dp := False
          when(clock_strobe) {
            state := 6
          }
        }
        is(6) { // Ready, EOP: 'J'
          io.usb_dm := True
          io.usb_dp := False
          io.ready := True
        }
      }

    } otherwise {
      state := 0
      crc16 := B"16'hFFFF"
    }

}

case class USBBusReset() extends USBSendReceive(hasSendKJ = false) {
    val io = new USB_IO {
	val delay     = in UInt(16 bits)
    }

    val delay_count = Reg(UInt(16 bits)).addTag(crossClockDomain)

    make_clock_strobe(io)

    io.test := clock_strobe

    when(io.valid) {

      when(clock_strobe) {
        delay_count := delay_count + 1
      }

      when(clock_strobe && delay_count === io.delay) { // Ready, EOP: 'J'
        io.usb_dm := True
        io.usb_dp := False
        io.ready := True
      } otherwise {
        io.usb_dm := False // '00'
        io.usb_dp := False
      }

    } otherwise {
      delay_count := 0
    }
}

case class USBKeepAlive() extends USBSendReceive(hasSendKJ = false, hasStrobe = true, szBitcount = 2) {

    val io = new USB_IO;

    make_clock_strobe(io)
    inc_bit_count(io)

    io.test := clock_strobe

    when(io.valid) {

      when(clock_strobe && bit_count === 2) { // Ready, 'J'
        io.usb_dm := True
        io.usb_dp := False
        io.ready := True
      } otherwise {
        io.usb_dm := False // 'SE00'
        io.usb_dp := False
      }

    }
}

case class USBReceiver() extends USBSendReceive(hasStrobe = false, hasSendKJ = false, hasCRC16 = true, szBitcount = 7) {
    val io = new USB_IO {
        val packet    = out Bits(128 bits) // PID(8) + DATA(64) + CRC16(16) + ALIGN
        val bits_recv = out UInt(7 bits)
        val calculated_crc16 = out Bits(16 bits)
        val received_crc16 = out Bits(16 bits)
    }

    override val ones = Reg(UInt(3 bits)).addTag(crossClockDomain)
    val calculated_crc16 = Reg(Bits(16 bits)).addTag(crossClockDomain) init(0)
    val received_crc16 = Reg(Bits(16 bits)).addTag(crossClockDomain) init(0)
    val bit_len = Reg(UInt(8 bits)).addTag(crossClockDomain) init(0)
    val state = Reg(UInt(3 bits)).addTag(crossClockDomain) init(0)
    val T0 = Reg(UInt(8 bits)).addTag(crossClockDomain) init(0) // Bit timer
    val T1 = Reg(UInt(8 bits)) init(0) // EOP timer
    val T2 = Reg(UInt(12 bits)) init(0) // Guard timer

    val ready = Reg(Bool()).addTag(crossClockDomain) init(False)
    val packet = Reg(Bits(128 bits)).addTag(crossClockDomain) init(0)
    val last_dp = Reg(Bool()) init(True)
    val last_symbol = Reg(Bool()) init(True)

    io.packet := packet
    io.bits_recv := bit_count
    io.calculated_crc16 := ~calculated_crc16.reversed
    io.received_crc16 := received_crc16

    //io.test := io.valid
    io.test := False

    when(io.valid) {

      io.ready := ready

      switch(state) {

        is(0) { // SYNC: begin calibration
          when(io.usb_dp && !io.usb_dm) { // First 'K' - start calibration
            state := 1
            bit_count := 0
            ones := 0
            bit_len := 7 // default is 8 clocks
            packet := 0
            calculated_crc16 := B"16'hFFFF"
            ready := False
            last_dp := True
            last_symbol := True
            T0 := 0
            T1 := 0
            T2 := 0
          }
        }

        is(1) { // SYNC: 'K' is going, waiting for 'J'
          T0 := T0 + 1
          when(!io.usb_dp && io.usb_dm) { // 'J' received
            bit_count := bit_count + 1
            state := 2
          }
          when(T0 === 255) { // Too much, error
            state := 7
          }
        }

        is(2) { // SYNC: 'J' is going, waiting for 'K'
          T0 := T0 + 1
          when(io.usb_dp && !io.usb_dm) { // 'K' received
            bit_count := bit_count + 1
            state := 1
            when(bit_count === 3) { // Two 'KJ' received
              bit_len := ((T0 + 1) >> 2).resized // calculate bit duration: div by 4
              state := 3
              T0 := 0
            }
          }
          when(T0 === 255) { // Too much, error
            state := 7
          }
        }

        is(3) { // Wait for end of SYNC: two 'K's
          when(io.usb_dp && !io.usb_dm) { // 'K' received
            T0 := T0 + 1
            when(T0.asBits === bit_len(6 downto 0) ## B"0") { // T0 = bit_len * 2
              T0 := 0
              state := 4
              bit_count := 0
            }
          } otherwise {
            T0 := 0
          }
          when(T0 === 255) { // Too much, error
            state := 7
          }
        }

        is(4) { // Receiving data
          when(io.usb_dp =/= io.usb_dm) { // Valid data are only when DP != DM
            T0 := T0 + 1
            last_dp := io.usb_dp
            when(last_dp =/= io.usb_dp) { // sync on each edge
              T0 := 0
            }
            when(T0 === bit_len) { // end of symbol ?
              T0 := 0
            }
            when(T0 === bit_len(7 downto 1).resized) { // sample one symbol in the middle of tick
              var bit_received = (io.usb_dp === last_symbol) // convert symbol to bit
              last_symbol := io.usb_dp
              when(bit_received) {
                ones := ones + 1
              } otherwise {
                ones := 0
              }
              when(ones =/= 6) { // save current bit
                bit_count := bit_count + 1
                packet(bit_count) := bit_received
		received_crc16 := bit_received ## received_crc16(15 downto 1)
                when(bit_count > 23 && bit_count(2 downto 0) === U("000")) {
			calculated_crc16 := calc_crc16_usb(calculated_crc16, received_crc16(7 downto 0).reversed)
		}
              } otherwise { // skip current bit because it's stuffing bit
                ones := 0
              }
              when(bit_count === 127) { // max data size achieved
                state := 7
              }
              io.test := True
            }
          }
        }

        is(6) { // EOP received 
          T0 := T0 + 1
          when(T0 === bit_len) { // wait for 'J' after SE0
            state := 7
          }
        }

        is(7) { // Ready 
          ready := True
          io.bits_recv := bit_count
          io.test := True
        }

        default {
          state := 7
          packet(87 downto 80) := state.asBits.resized
          io.test := True
        }

      }

      // Check for EOP (SE0)
      when(!io.usb_dp && !io.usb_dm) {
        T1 := T1 + 1
        when(T1 === ((bit_len << 1) - U(2))) { // is SE0 for two bit intervals - report EOP
          state := 6
          T0 := 0
        }
      } otherwise {
        T1 := 0
      }

      // Check for hung state ('J')
      when(!io.usb_dp && io.usb_dm) {
        T2 := T2 + 1
        when(T2 === (bit_len << 3)) { // is 'J' for more than 8 clocks - report error!
          state := 7
        }
      } otherwise {
        T2 := 0
      }

    } otherwise {
      state := 0
      ready := False 
    }
}

object USBMain extends SpinalEnum{
  val StateUnconnected, StateWaitCMDorSYNC, StateKeepAlive, StateSendLongToken, StateSendShortToken,
      StateSendData, StateSendReset, StateReceive
      = newElement()
}

object USBCommand extends SpinalEnum(defaultEncoding = binarySequential){
  val CMDNone, CMDSendToken, CMDSendShortToken, CMDSendData, CMDBusReset
      = newElement()
}

case class Apb3USB10Ctrl(
	usbFrequency : HertzNumber = 12.0 MHz
      ) extends Component {
  val io = new Bundle {
    val apb       = slave(Apb3(addressWidth = 12, dataWidth = 32))
    val usb       = master(USBInterface())
    val interrupt = out Bool()
    val usb_clk = in Bool()

    val test = out Bool()
  }

  import USBMain._
  import USBCommand._


  val busCtrl = Apb3SlaveFactory(io.apb)

  val usbStatusWord = busCtrl.createReadOnly(Bits(32 bits), address = 0) init(0)
  val error_flag = usbStatusWord(30).addTag(crossClockDomain)
  val report_flag = usbStatusWord(29).addTag(crossClockDomain)
  val busy_flag = usbStatusWord(28).addTag(crossClockDomain)
  val received_flag = usbStatusWord(27).addTag(crossClockDomain)
  val crc16_ok_flag = usbStatusWord(26).addTag(crossClockDomain)
  // ... more flags here
  //val received_pid = usbStatusWord(23 downto 16).addTag(crossClockDomain)
  // ... reserved for FSM states
  val fsm_state = usbStatusWord(2 downto 0).addTag(crossClockDomain)

  val usbCommandWord = busCtrl.createReadWrite(Bits(32 bits), address = 4) init(0)
  val cmd_start = usbCommandWord(31).addTag(crossClockDomain)
  val cmd_addr = usbCommandWord(30 downto 24).addTag(crossClockDomain)
  val cmd_endp = usbCommandWord(23 downto 20).addTag(crossClockDomain)
  val cmd_len = usbCommandWord(19 downto 8).asUInt.addTag(crossClockDomain) // len in bits - 1
  val cmd_pid = usbCommandWord(7 downto 4).addTag(crossClockDomain)
  val cmd = usbCommandWord(3 downto 0).addTag(crossClockDomain)

  val usbDataReceivedLowWord = busCtrl.createReadOnly(Bits(32 bits), address = 8) init(0)
  val received_data_low = usbDataReceivedLowWord(31 downto 0).addTag(crossClockDomain)

  val usbDataReceivedHighWord = busCtrl.createReadOnly(Bits(32 bits), address = 12) init(0)
  val received_data_high = usbDataReceivedHighWord(31 downto 0).addTag(crossClockDomain)

  val usbSendLowWord = busCtrl.createReadWrite(Bits(32 bits), address = 16) init(0)
  val send_data_low = usbSendLowWord(31 downto 0).addTag(crossClockDomain)

  val usbSendHighWord = busCtrl.createReadWrite(Bits(32 bits), address = 20) init(0)
  val send_data_high = usbSendHighWord(31 downto 0).addTag(crossClockDomain)

  val usbReceiverStatusWord = busCtrl.createReadOnly(Bits(32 bits), address = 24) init(0)
  val received_bits = usbReceiverStatusWord(7 downto 0).addTag(crossClockDomain)
  val received_pid = usbReceiverStatusWord(15 downto 8).addTag(crossClockDomain)
  val received_crc16 = usbReceiverStatusWord(31 downto 16).addTag(crossClockDomain)

  val usbControlWord = busCtrl.createReadWrite(Bits(32 bits), address = 28) init(22500) // keepalive: 15 ms at 1.5 MHz
  val bus_enable = usbControlWord(31).addTag(crossClockDomain)
  val keepalive_enable = usbControlWord(30).addTag(crossClockDomain)
  val reset_delay = usbControlWord(15 downto 0).asUInt.addTag(crossClockDomain)

  val usbReceiverStatusWord2 = busCtrl.createReadOnly(Bits(32 bits), address = 32) init(0)
  val calculated_crc16 = usbReceiverStatusWord2(15 downto 0).addTag(crossClockDomain)

  io.interrupt := report_flag


  val usbClockDomain = ClockDomain(
    clock = io.usb_clk,
    reset = bus_enable,
    config = ClockDomainConfig(resetKind = SYNC, resetActiveLevel = LOW),
    frequency = FixedFrequency(usbFrequency)
  )

//io.test := busy_flag

  val usb_area = new ClockingArea(usbClockDomain) {

//  val usb_area = new Area() {

    println("Apb3USB10Ctrl::usbFrequency = %d Hz".format(usbFrequency.toInt));

    val USBSlowSpeedClockDiv = UInt(8 bits)
    val low_speed_baudrate : HertzNumber = 1.5 MHz;
    val usbslowspeedclockdiv = (ClockDomain.current.frequency.getValue / low_speed_baudrate + 0.5).toBigInt - 1
    USBSlowSpeedClockDiv := usbslowspeedclockdiv 
    println("Apb3USB10Ctrl::USBSlowSpeedClockDiv = %d".format(usbslowspeedclockdiv));

    val USBLowSpeedKeepAliveClocks = UInt(16 bits)
    val low_speed_keepalive : TimeNumber = 0.9 ms; // Send KeepAlive interval
    val usblowspeedkeepaliveclocks = (ClockDomain.current.frequency.getValue * low_speed_keepalive + 0.5).toBigInt - 1
    USBLowSpeedKeepAliveClocks := usblowspeedkeepaliveclocks
    println("Apb3USB10Ctrl::USBLowSpeedKeepAliveClocks = %d".format(usblowspeedkeepaliveclocks));

    val USBLowSpeedErrorClocks = UInt(16 bits)
    val low_speed_error : TimeNumber = 0.8 ms; // Tiee to detect disconnect or error
    val usblowspeederrorclocks = (ClockDomain.current.frequency.getValue * low_speed_error + 0.5).toBigInt - 1
    USBLowSpeedErrorClocks := usblowspeederrorclocks
    println("Apb3USB10Ctrl::USBLowSpeedErrorClocks = %d".format(usblowspeederrorclocks));

    val state = RegInit(StateUnconnected).addTag(crossClockDomain)
    val T1 = Reg(UInt(16 bits)).addTag(crossClockDomain) init(0) // Guard timer
    val T2 = Reg(UInt(16 bits)).addTag(crossClockDomain) init(0) // Low-Speed Keep-Alive timer
   
    val error = Reg(Bool()).addTag(crossClockDomain) init(False)
    val report = Reg(Bool()).addTag(crossClockDomain) init(False)
    val busy = Reg(Bool()).addTag(crossClockDomain) init(False)
    val received = Reg(Bool()).addTag(crossClockDomain) init(False)
    val crc16_ok = Reg(Bool()).addTag(crossClockDomain) init(False)

    error_flag := error
    report_flag := report
    received_flag := received
    busy_flag := busy
    crc16_ok_flag := crc16_ok
    fsm_state := state.asBits


    val send_long_token = new USBSendLongToken()
    send_long_token.io.pid := 0
    send_long_token.io.addr := 0
    send_long_token.io.endp := 0
    send_long_token.io.valid := False
    send_long_token.io.clock_div := USBSlowSpeedClockDiv

    val send_short_token = new USBSendShortToken()
    send_short_token.io.pid := 0
    send_short_token.io.valid := False
    send_short_token.io.clock_div := USBSlowSpeedClockDiv

    val send_data = new USBSendData()
    send_data.io.pid := 0
    send_data.io.data := 0
    send_data.io.len := 0xfff // zero data bits
    send_data.io.valid := False
    send_data.io.clock_div := USBSlowSpeedClockDiv

    val send_bus_reset = new USBBusReset()
    send_bus_reset.io.valid := False
    send_bus_reset.io.delay := reset_delay
    send_bus_reset.io.clock_div := USBSlowSpeedClockDiv

    val send_keepalive = new USBKeepAlive() // this is for Slow Speed bus only
    send_keepalive.io.valid := False
    send_keepalive.io.clock_div := USBSlowSpeedClockDiv

    val receiver = new USBReceiver()
    receiver.io.valid := False

    val bus_error = !io.usb.usb_dm && !io.usb.usb_dp; // Both DP and DM low means nothing is connected 
    val bus_present = io.usb.usb_dm && !io.usb.usb_dp; // Low Speed device connected
    val bus_activity = !io.usb.usb_dm && io.usb.usb_dp; // Polarity change indicates some activity

    //io.test := send_bus_reset.io.test|send_long_token.io.test|send_data.io.test
    //io.test := busy
    //io.test := receiver.io.test
    //io.test := received
    io.test := crc16_ok 

    // Guard timer T1 checks for error state on the bus

    when(bus_error && state =/= StateSendReset) {
      T1 := T1 + 1
      when(T1 === USBLowSpeedErrorClocks) { // DM/DP is low for quite some time ?
        T1 := 0
        state := StateUnconnected
        error := True
        report := True
        received := False
        busy := False
        cmd := CMDNone.asBits.resized // clear last cmd
        cmd_start := False
      }
    } otherwise {
      T1 := 0
    }


    // Main FSM

    switch(state) {

      is(StateUnconnected) { // Unconnected
        report := False

        when(bus_present) {
          busy := False
          cmd_start := False
          error := False
          report := True
          state := StateWaitCMDorSYNC // Low Speed device just connected
        }
      }

      is(StateWaitCMDorSYNC) { // Wait command or SYNC
        report := False

        when(cmd_start) {
          switch(cmd) {
            is(CMDSendToken.asBits.resize(4)) {
              state := StateSendLongToken
              busy := True
              received := False
            }
            is(CMDSendShortToken.asBits.resize(4)) {
              state := StateSendShortToken
              busy := True
              received := False
            }
            is(CMDSendData.asBits.resize(4)) {
              state := StateSendData
              busy := True
              received := False
            }
            is(CMDBusReset.asBits.resize(4)) {
              state := StateSendReset
              busy := True
              received := False
            }
            default {
              cmd_start := False
              report := True
              received := False
            }
          }
        }

        when(bus_activity) { // Activity on the bus ?
          state := StateReceive
          busy := True
          received := False
        }

        when(keepalive_enable && !(!io.usb.usb_dm && !io.usb.usb_dp)) { // Keepalive enabled and not Error state ?
          T2 := T2 + 1

          when(T2 === USBLowSpeedKeepAliveClocks) {
            state := StateKeepAlive
            busy := True
          }
        }

      }

      is(StateSendLongToken) { // Connected, send Token
        send_long_token.io.pid := cmd_pid
        send_long_token.io.addr := cmd_addr
        send_long_token.io.endp := cmd_endp
        send_long_token.io.valid := True
        send_long_token.io.usb_dm <> io.usb.usb_dm
        send_long_token.io.usb_dp <> io.usb.usb_dp
        when(send_long_token.io.ready) {
          state := StateWaitCMDorSYNC
          report := True
          cmd_start := False
          busy := False
          T2 := 0
        }
      }

      is(StateSendShortToken) { // Connected, send Short Token
        send_short_token.io.pid := cmd_pid
        send_short_token.io.valid := True
        send_short_token.io.usb_dm <> io.usb.usb_dm
        send_short_token.io.usb_dp <> io.usb.usb_dp
        when(send_short_token.io.ready) {
          state := StateWaitCMDorSYNC
          report := True
          cmd_start := False
          busy := False
          T2 := 0
        }
      }

      is(StateSendData) { // send DATA packet
        send_data.io.pid := cmd_pid
        send_data.io.data := send_data_high ## send_data_low // B"64'hAAAAAAAAAAAAAAAA" // 0 //B"01010101010101010101010101010101" ## B"01010101010101010101010101010101" //send_data_high ## send_data_low
        send_data.io.len := cmd_len // in bits - 1
        send_data.io.valid := True
        send_data.io.usb_dm <> io.usb.usb_dm
        send_data.io.usb_dp <> io.usb.usb_dp
        when(send_data.io.ready) {
          state := StateWaitCMDorSYNC
          report := True
          cmd_start := False
          busy := False
          T2 := 0
        }
      }

      is(StateSendReset) { // Bus Reset condition (D+ and D- are low for 11ms)
        send_bus_reset.io.valid := True
        send_bus_reset.io.usb_dm <> io.usb.usb_dm
        send_bus_reset.io.usb_dp <> io.usb.usb_dp
        when(send_bus_reset.io.ready) {
          state := StateWaitCMDorSYNC
          report := True
          cmd_start := False
          busy := False
          T2 := 0
        }
      }

      is(StateKeepAlive) { // Send KeepAlive (Low-Speed only)
        send_keepalive.io.valid := True
        send_keepalive.io.usb_dm <> io.usb.usb_dm
        send_keepalive.io.usb_dp <> io.usb.usb_dp
        when(send_keepalive.io.ready) {
          state := StateWaitCMDorSYNC
          cmd_start := False
          busy := False
          T2 := 0
        }
      }

      is(StateReceive) { // Receive data piece
        receiver.io.valid := True
        receiver.io.usb_dm <> io.usb.usb_dm
        receiver.io.usb_dp <> io.usb.usb_dp
        when(receiver.io.ready) {
          state := StateWaitCMDorSYNC
          received := True
          busy := False
          report := True
          received_pid := receiver.io.packet(7 downto 0)
          received_data_low := receiver.io.packet(39 downto 8)
          received_data_high := receiver.io.packet(71 downto 40)
          received_bits := receiver.io.bits_recv.asBits.resized
          received_crc16 := receiver.io.received_crc16 //receiver.io.packet(87 downto 72)
          calculated_crc16 := receiver.io.calculated_crc16
          crc16_ok := receiver.io.received_crc16 === receiver.io.calculated_crc16
        }
      }

    } // switch(state)

  } // usb_area

}

