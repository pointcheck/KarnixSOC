package mylib

import java.math._
import spinal.core._
import spinal.lib._
import spinal.lib.io.TriState
import spinal.lib.Counter
import spinal.lib.bus.amba3.apb.{Apb3, Apb3Config, Apb3SlaveFactory}
import spinal.lib.misc.HexTools

case class USBSendToken() extends Component {
    val io = new Bundle {
	val usb_dm    = inout(Analog(Bool()))
	val usb_dp    = inout(Analog(Bool()))
	val valid     = in Bool()
	val ready     = out Bool()
	val pid       = in Bits(4 bits)
	val addr      = in Bits(7 bits)
	val endp      = in Bits(4 bits)
	val clock_div = in UInt(8 bits)

        val test = out Bool()
    }

    def calc_crc5_usb(din: Bits) : Bits = {
      val ret = Bits(5 bits)
      ret(0) := din(10) ^ din(9) ^ din(6) ^ din(5) ^ din(3) ^ din(0) ^ True
      ret(1) := din(10) ^ din(7) ^ din(6) ^ din(4) ^ din(1) ^ True
      ret(2) := din(10) ^ din(9) ^ din(8) ^ din(7) ^ din(6) ^ din(3) ^ din(2) ^ din(0) ^ True
      ret(3) := din(10) ^ din(9) ^ din(8) ^ din(7) ^ din(4) ^ din(3) ^ din(1)
      ret(4) := din(10) ^ din(9) ^ din(8) ^ din(5) ^ din(4) ^ din(2) ^ True
      return ret
    }

    val last_kj = Reg(Bool()).addTag(crossClockDomain)

    def toKJ(input_bit: Bool) : Bool = {
      val ret = Bool()

      when(input_bit === False && clock_strobe) {
        last_kj := !last_kj // Transition
        ret := !last_kj
      } otherwise { // No transition
        ret := last_kj
      }

      return ret
    }

    val crc5_out = calc_crc5_usb(io.addr(0) ## io.addr(1) ## io.addr(2) ## io.addr(3) ##
                        io.addr(4) ## io.addr(5) ## io.addr(6) ## io.endp(0) ##
                        io.endp(1) ## io.endp(2) ## io.endp(3)) ^ B"11111"
    val buffer = crc5_out.reversed ## io.endp ## io.addr ## ~io.pid ## io.pid ## B"10000000"
    val bit_count = Reg(UInt(6 bits)).addTag(crossClockDomain)
    val clock_div = Reg(UInt(8 bits)).addTag(crossClockDomain)
    val clock_strobe = False

    io.ready := False

    io.test := clock_strobe

    // J: D- = 1, D+ = 0, K: D- = 0, D+ = 1
    // KJKJKJKK + PID + ADDR + ENDP + CRC5

    when(io.valid) {
      clock_div := clock_div + 1

      when(clock_div === io.clock_div) {
        clock_div := 0
      }

      when(clock_div === 0) {
        clock_strobe := True
      }

      when(clock_strobe) {
        bit_count := bit_count + 1
      }

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
        when(toKJ(buffer(bit_count(4 downto 0)))) { // DATA: 0 - 'J' or 1 - 'K'
          io.usb_dm := False
          io.usb_dp := True
        } otherwise {
          io.usb_dm := True
          io.usb_dp := False
        }
      }

    } otherwise {
      clock_div := 0
      bit_count := 0
      last_kj := False // 'J'
    }
}

case class USBSendShortToken() extends Component {
    val io = new Bundle {
	val usb_dm    = inout(Analog(Bool()))
	val usb_dp    = inout(Analog(Bool()))
	val valid     = in Bool()
	val ready     = out Bool()
	val pid       = in Bits(4 bits)
	val clock_div = in UInt(8 bits)

        val test = out Bool()
    }

    val last_kj = Reg(Bool()).addTag(crossClockDomain)

    def toKJ(input_bit: Bool) : Bool = {
      val ret = Bool()

      when(input_bit === False && clock_strobe) {
        last_kj := !last_kj // Transition
        ret := !last_kj
      } otherwise { // No transition
        ret := last_kj
      }

      return ret
    }

    val buffer = ~io.pid ## io.pid ## B"10000000"
    val bit_count = Reg(UInt(5 bits)).addTag(crossClockDomain)
    val clock_div = Reg(UInt(8 bits)).addTag(crossClockDomain)
    val clock_strobe = False

    io.ready := False

    io.test := clock_strobe

    // J: D- = 1, D+ = 0, K: D- = 0, D+ = 1
    // KJKJKJKK + PID + ADDR + ENDP + CRC5

    when(io.valid) {
      clock_div := clock_div + 1

      when(clock_div === io.clock_div) {
        clock_div := 0
      }


      when(clock_div === 0) {
        clock_strobe := True
      }

      when(clock_strobe) {
        bit_count := bit_count + 1
      }

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
        when(toKJ(buffer(bit_count(3 downto 0)))) { // DATA: 0 - 'J' or 1 - 'K'
          io.usb_dm := False
          io.usb_dp := True
        } otherwise {
          io.usb_dm := True
          io.usb_dp := False
        }
      }

    } otherwise {
      clock_div := 0
      bit_count := 0
      last_kj := False // 'J'
    }
}

case class USBSendData() extends Component {
    val io = new Bundle {
	val usb_dm    = inout(Analog(Bool()))
	val usb_dp    = inout(Analog(Bool()))
	val valid     = in Bool()
	val ready     = out Bool()
	val pid       = in Bits(4 bits)
	val data      = in Bits(64 bits)
	val len       = in UInt(12 bits)
	val clock_div = in UInt(8 bits)

        val test = out Bool()
    }

    def calc_crc16_usb(crc_in: Bits, din: Bits) : Bits = {
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



    val last_kj = Reg(Bool()).addTag(crossClockDomain)

    def toKJ(input_bit: Bool) : Bool = {
      val ret = Bool()

      when(input_bit === False && clock_strobe) {
        last_kj := !last_kj // Transition
        ret := !last_kj
      } otherwise { // No transition
        ret := last_kj
      }

      return ret
    }

    val crc16 = Reg(Bits(16 bits)).addTag(crossClockDomain) init(B"16'hffff")
    val crc_byte = Reg(Bits(8 bits))
    val sync_pid_buffer = ~io.pid ## io.pid ## B"10000000"
    val bit_count = Reg(UInt(13 bits)).addTag(crossClockDomain) // 8192 bits max
    val clock_div = Reg(UInt(8 bits)).addTag(crossClockDomain)
    val clock_strobe = False
    val state = Reg(UInt(3 bits)).addTag(crossClockDomain) init(0)
    val ones = Reg(UInt(3 bits)).addTag(crossClockDomain) init(0)
    val one_bit = False
    val stuffing = False

    io.test := clock_strobe //io.valid

    io.ready := False

    // J: D- = 1, D+ = 0, K: D- = 0, D+ = 1
    // KJKJKJKK + PID + DATA + CRC16

    when(io.valid) {
      clock_div := clock_div + 1

      when(clock_div === io.clock_div) {
        clock_div := 0
      }

      when(clock_div === 0) {
        clock_strobe := True
      }

      when(clock_strobe && !stuffing) {
        bit_count := bit_count + 1
      }

      switch(state) {
        is(0) { // sending SYNC + PID
          val one_bit = sync_pid_buffer(bit_count(3 downto 0))
          when(toKJ(one_bit)) { // DATA: 0 - 'J' or 1 - 'K'
            io.usb_dm := False
            io.usb_dp := True
          } otherwise {
            io.usb_dm := True
            io.usb_dp := False
          }
          when(clock_strobe && bit_count === 15) {
            state := 1
            bit_count := 0
          }
        }
        is(1) { // sending DATA block
          one_bit := io.data(bit_count(5 downto 0))
          val out_bit = one_bit
          when(ones === 6) {
            stuffing := True
          }
          when(stuffing) {
            out_bit := False
          }
          when(clock_strobe) {
            when(one_bit) {
              ones := ones + 1
            } otherwise {
              ones := 0
            }
          }
          when(clock_strobe && !stuffing) {
            val crc_byte_next = one_bit ## crc_byte(7 downto 1)
            crc_byte := crc_byte_next
            when(bit_count(2 downto 0) === U"111") {
              crc16 := calc_crc16_usb(crc16, crc_byte_next.reversed)
            }
            when(bit_count === io.len) {
              state := 2
              bit_count := 0
            }
          }
          when(toKJ(out_bit)) { // DATA: 0 - 'J' or 1 - 'K'
            io.usb_dm := False
            io.usb_dp := True
          } otherwise {
            io.usb_dm := True
            io.usb_dp := False
          }
        }
        is(2) { // sending CRC16 block
          // CRC out is reversed and XORed
          val crc_rev = ~crc16.reversed
          one_bit := crc_rev(bit_count(3 downto 0))
          val out_bit = one_bit
          when(ones === 6) {
            stuffing := True
          }
          when(stuffing) {
            out_bit := False
          }
          when(clock_strobe) {
            when(one_bit) {
              ones := ones + 1
            } otherwise {
              ones := 0
            }
          }
          when(clock_strobe && bit_count === 16) {
            io.usb_dm := False
            io.usb_dp := False
            state := 3
          } otherwise {
            when(toKJ(out_bit)) { // DATA: 0 - 'J' or 1 - 'K'
              io.usb_dm := False
              io.usb_dp := True
            } otherwise {
              io.usb_dm := True
              io.usb_dp := False
            }
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
      clock_div := 0
      bit_count := 0
      ones := 0
      crc16 := B"16'hFFFF"
      //last_kj := True // 'K'
      last_kj := False // 'J'
    }
}

case class USBBusReset() extends Component {
    val io = new Bundle {
	val usb_dm    = inout(Analog(Bool()))
	val usb_dp    = inout(Analog(Bool()))
	val valid     = in Bool()
	val ready     = out Bool()
	val delay     = in UInt(16 bits)
	val clock_div = in UInt(8 bits)

        val test = out Bool()
    }

    val delay_count = Reg(UInt(16 bits)).addTag(crossClockDomain)
    val clock_div = Reg(UInt(8 bits)).addTag(crossClockDomain)
    val clock_strobe = False

    io.test := clock_strobe

    io.ready := False

    when(io.valid) {
      clock_div := clock_div + 1

      when(clock_div === io.clock_div) {
        clock_div := 0
        clock_strobe := True
      }

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
      clock_div := 0
      delay_count := 0
    }
}

case class USBKeepAlive() extends Component {
    val io = new Bundle {
	val usb_dm    = inout(Analog(Bool()))
	val usb_dp    = inout(Analog(Bool()))
	val valid     = in Bool()
	val ready     = out Bool()
	val clock_div = in UInt(8 bits)

        val test = out Bool()
    }

    val bit_count = Reg(UInt(2 bits)).addTag(crossClockDomain)
    val clock_div = Reg(UInt(8 bits)).addTag(crossClockDomain)
    val clock_strobe = False

    io.test := clock_strobe

    io.ready := False

    when(io.valid) {
      clock_div := clock_div + 1

      when(clock_div === io.clock_div) {
        clock_div := 0
      }

      when(clock_div === 0) {
        clock_strobe := True
      }

      when(clock_strobe) {
        bit_count := bit_count + 1
      }

      when(clock_strobe && bit_count === 2) { // Ready, 'J'
        io.usb_dm := True
        io.usb_dp := False
        io.ready := True
      } otherwise {
        io.usb_dm := False // 'SE00'
        io.usb_dp := False
      }

    } otherwise {
      clock_div := 0
      bit_count := 0
    }
}

case class USBReceiver() extends Component {
    val io = new Bundle {
	val usb_dm    = inout(Analog(Bool()))
	val usb_dp    = inout(Analog(Bool()))
	val valid     = in Bool()
	val ready     = out Bool()
        val packet    = out Bits(128 bits) // PID(8) + DATA(64) + CRC16(16) + ALIGN
        val bits_recv = out UInt(7 bits)

        val test = out Bool()
    }

    val bit_count = Reg(UInt(7 bits)).addTag(crossClockDomain) init(0)
    val bit_len = Reg(UInt(8 bits)).addTag(crossClockDomain) init(0)
    val state = Reg(UInt(3 bits)).addTag(crossClockDomain) init(0)
    val ones = Reg(UInt(3 bits)).addTag(crossClockDomain) init(0) // Count log 1's
    val T0 = Reg(UInt(8 bits)).addTag(crossClockDomain) init(0) // Bit timer
    val T1 = Reg(UInt(8 bits)) init(0) // EOP timer
    val T2 = Reg(UInt(8 bits)) init(0) // Guard timer

    val ready = Reg(Bool()).addTag(crossClockDomain) init(False)
    val packet = Reg(Bits(128 bits)).addTag(crossClockDomain) init(0)
    val last_dp = Reg(Bool()) init(True)
    val last_symbol = Reg(Bool()) init(True)

    io.packet := packet
    io.ready := ready
    io.bits_recv := bit_count

//    io.test := io.valid
io.test := False

    when(io.valid) {

      switch(state) {

        is(0) { // SYNC: begin calibration
          when(io.usb_dp && !io.usb_dm) { // First 'K' - start calibration
            state := 1
            bit_count := 0
            ones := 0
            bit_len := 7 // default is 8 clocks
            packet := 0
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
              var one_bit = (io.usb_dp === last_symbol) // convert to bit
              last_symbol := io.usb_dp
              when(one_bit) {
                ones := ones + 1
              } otherwise {
                ones := 0
              }
              when(ones =/= 6) { // save current bit
                bit_count := bit_count + 1
                packet(bit_count) := one_bit
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
          //io.test := True
        }
      } otherwise {
        T1 := 0
      }

      // Check for hung state ('J')
      when(!io.usb_dp && io.usb_dm) {
        T2 := T2 + 1
        when(T2 === (bit_len << 3)) { // is 'J' for more than 8 clocks - report error!
          state := 7
          //io.test := True
        }
      } otherwise {
        T2 := 0
      }

    } otherwise {
      state := 0
      ready := False 
    }
}

object USBPhase extends SpinalEnum{
  val StateUnconnected, StateWaitCMDorSYNC, StateKeepAlive, StateSendToken, StateSendShortToken,
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

  import USBPhase._
  import USBCommand._


  val busCtrl = Apb3SlaveFactory(io.apb)

  val usbStatusWord = busCtrl.createReadOnly(Bits(32 bits), address = 0) init(0)
  val error_flag = usbStatusWord(30).addTag(crossClockDomain)
  val report_flag = usbStatusWord(29).addTag(crossClockDomain)
  val busy_flag = usbStatusWord(28).addTag(crossClockDomain)
  val received_flag = usbStatusWord(27).addTag(crossClockDomain)
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
  val enable = usbControlWord(31).addTag(crossClockDomain)
  val keepalive = usbControlWord(30).addTag(crossClockDomain)
  val reset_delay = usbControlWord(15 downto 0).asUInt.addTag(crossClockDomain)

  io.interrupt := report_flag


  val usbClockDomain = ClockDomain(
    clock = io.usb_clk,
    reset = enable,
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

    error_flag := error
    report_flag := report
    received_flag := received
    busy_flag := busy

    fsm_state := state.asBits

    // Check device presence
    when(!io.usb.usb_dm && !io.usb.usb_dp && state =/= StateSendReset) {
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


    val send_token = new USBSendToken()
    send_token.io.pid := 0
    send_token.io.addr := 0
    send_token.io.endp := 0
    send_token.io.valid := False
    send_token.io.clock_div := USBSlowSpeedClockDiv

    val send_short_token = new USBSendShortToken()
    send_short_token.io.pid := 0
    send_short_token.io.valid := False
    send_short_token.io.clock_div := USBSlowSpeedClockDiv

    val send_data = new USBSendData()
    send_data.io.pid := 0
    send_data.io.data := 0
    send_data.io.len := 0
    send_data.io.valid := False
    send_data.io.clock_div := USBSlowSpeedClockDiv

    val bus_reset = new USBBusReset()
    bus_reset.io.valid := False
    bus_reset.io.delay := reset_delay
    bus_reset.io.clock_div := USBSlowSpeedClockDiv

    val send_keepalive = new USBKeepAlive() // this is for Slow Speed bus only
    send_keepalive.io.valid := False
    send_keepalive.io.clock_div := USBSlowSpeedClockDiv

    val receiver = new USBReceiver()
    receiver.io.valid := False
    when(receiver.io.ready) {
      received_pid := receiver.io.packet(7 downto 0)
      received_data_low := receiver.io.packet(39 downto 8)
      received_data_high := receiver.io.packet(71 downto 40)
      received_crc16 := receiver.io.packet(87 downto 72)
      received_bits := receiver.io.bits_recv.asBits.resized
    }


    //io.test := bus_reset.io.test|send_token.io.test|send_data.io.test
io.test := busy //receiver.io.test

    switch(state) {

      is(StateUnconnected) { // Unconnected
        report := False

        when(io.usb.usb_dm && !io.usb.usb_dp) {
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
              state := StateSendToken
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

        when(!io.usb.usb_dm && io.usb.usb_dp) { // Activity on the bus ?
          state := StateReceive
          busy := True
          received := False
        }

        when(keepalive && !(!io.usb.usb_dm && !io.usb.usb_dp)) { // Keepalive enabled and not Error state ?
          T2 := T2 + 1

          when(T2 === USBLowSpeedKeepAliveClocks) {
            state := StateKeepAlive
            busy := True
          }
        }

      }

      is(StateSendToken) { // Connected, send Token
        send_token.io.pid := cmd_pid
        send_token.io.addr := cmd_addr
        send_token.io.endp := cmd_endp
        send_token.io.valid := True
        send_token.io.usb_dm <> io.usb.usb_dm
        send_token.io.usb_dp <> io.usb.usb_dp
        when(send_token.io.ready) {
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
        bus_reset.io.valid := True
        bus_reset.io.usb_dm <> io.usb.usb_dm
        bus_reset.io.usb_dp <> io.usb.usb_dp
        when(bus_reset.io.ready) {
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
        }
      }

    } // switch(state)

  } // usb_area

}

