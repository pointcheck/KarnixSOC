package mylib

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
    val buffer = crc5_out ## io.endp ## io.addr ## ~io.pid ## io.pid ## B"1000000"
    val bit_count = Reg(UInt(6 bits)).addTag(crossClockDomain)
    val clock_div = Reg(UInt(8 bits)).addTag(crossClockDomain)
    val clock_strobe = False

    io.test := io.valid

    io.ready := False

    // J: D- = 1, D+ = 0, K: D- = 0, D+ = 1
    // KJKJKJKK + PID + ADDR + ENDP + CRC5 

    when(io.valid) {
      clock_div := clock_div + 1 

      when(clock_div === io.clock_div) {
        clock_div := 0
        clock_strobe := True
      }

      when(clock_strobe) {
        bit_count := bit_count + 1 
      }

      when(bit_count === 35) { // Ready, EOP: 'J' 
        io.usb_dm := True 
        io.usb_dp := False 
        io.ready := True 
        bit_count := 35 
      } elsewhen(bit_count === 34) { // EOP: 'J'
        io.usb_dm := True 
        io.usb_dp := False 
      } elsewhen((bit_count === 32) || (bit_count === 33)) { // EOP: 'SE0'
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
      //last_kj := False // 'J'
      last_kj := True // 'K' 
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
    val sync_pid_buffer = ~io.pid ## io.pid ## B"1000000"
    val bit_count = Reg(UInt(13 bits)).addTag(crossClockDomain) // 8192 bits max
    val clock_div = Reg(UInt(8 bits)).addTag(crossClockDomain)
    val clock_strobe = False
    val state = Reg(UInt(3 bits)).addTag(crossClockDomain) init(0)

    io.test := io.valid

    io.ready := False

    // J: D- = 1, D+ = 0, K: D- = 0, D+ = 1
    // KJKJKJKK + PID + DATA + CRC16

    when(io.valid) {
      clock_div := clock_div + 1 

      when(clock_div === io.clock_div) {
        clock_div := 0
        clock_strobe := True
      }

      when(clock_strobe) {
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
	  val crc_byte = Reg(Bits(8 bits))
          val one_bit = io.data(bit_count(5 downto 0))
          when(toKJ(one_bit)) { // DATA: 0 - 'J' or 1 - 'K'
            io.usb_dm := False
            io.usb_dp := True 
          } otherwise {
            io.usb_dm := True 
            io.usb_dp := False 
          }
          when(clock_strobe) {
            crc_byte := one_bit ## crc_byte(7 downto 1)
            when(bit_count(2 downto 0) === U"000") {
              crc16 := calc_crc16_usb(crc16, crc_byte.reversed)
            }
            when(bit_count === io.len) {
              state := 2
              bit_count := 0
            }
          }
        }
        is(2) { // sending CRC16 block
          // CRC out is reversed and XORed
          val crc_rev = ~crc16.reversed
          //val crc_rev = Bits(16 bits)
          //crc_rev(15 downto 8) := ~crc16(15 downto 8).reversed
          //crc_rev(7 downto 0) := ~crc16(7 downto 0).reversed
          val one_bit = crc_rev(bit_count(3 downto 0))
          when(toKJ(one_bit)) { // DATA: 0 - 'J' or 1 - 'K'
            io.usb_dm := False
            io.usb_dp := True 
          } otherwise {
            io.usb_dm := True 
            io.usb_dp := False 
          }
          when(clock_strobe && bit_count === 16) {
            state := 3
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
      crc16 := B"16'hFFFF"
      last_kj := True // 'K' 
    }
}

object USBPhase extends SpinalEnum{
  val StateUnconnected, StateWaitCMDorSYNC, StateSendToken, StateSendData
      = newElement()
}

object USBCommand extends SpinalEnum(defaultEncoding = binarySequential){
  val CMDNone, CMDSendToken, CMDSendData
      = newElement()
}

case class Apb3USB10Ctrl(
      ) extends Component {
  val io = new Bundle {
    val apb       = slave(Apb3(addressWidth = 12, dataWidth = 32))
    val usb       = master(USBInterface())
    val interrupt = out Bool()
    val usbclk_12mhz = in Bool()

    val test = out Bool()
  }

  import USBPhase._
  import USBCommand._

  val busCtrl = Apb3SlaveFactory(io.apb)

  val usbStatusWord = busCtrl.createReadWrite(Bits(32 bits), address = 0) init(0)
  val enable = usbStatusWord(31).addTag(crossClockDomain)
  val error = usbStatusWord(30).addTag(crossClockDomain)
  val report = usbStatusWord(29).addTag(crossClockDomain)
  val busy = usbStatusWord(28).addTag(crossClockDomain)
  val received = usbStatusWord(27).addTag(crossClockDomain)
  // ... more flags here
  val pid = usbStatusWord(23 downto 16).addTag(crossClockDomain)
  // ... reserved for FSM states
  val fsm_state = usbStatusWord(1 downto 0).addTag(crossClockDomain)

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

  val usbCRC16Word = busCtrl.createReadOnly(Bits(32 bits), address = 24) init(0)
  val crc16_received = usbCRC16Word(15 downto 0).addTag(crossClockDomain)
  val crc16_calculated = usbCRC16Word(31 downto 16).addTag(crossClockDomain)

  val usbCRC5Word = busCtrl.createReadOnly(Bits(32 bits), address = 28) init(0)
  val crc5_received = usbCRC5Word(4 downto 0).addTag(crossClockDomain)
  val crc5_calculated = usbCRC5Word(11 downto 8).addTag(crossClockDomain)

  io.interrupt := report


  val usbClockDomain = ClockDomain(
    clock = io.usbclk_12mhz,
    reset = enable,
    config = ClockDomainConfig(resetKind = SYNC, resetActiveLevel = LOW),
    frequency = FixedFrequency(12.0 MHz)
  )

  val usb_area = new ClockingArea(usbClockDomain) {

//  val usb_area = new Area() {

    val baudrate_slow_speed : HertzNumber = 1.5 MHz
    val USBSlowSpeedClockDiv = UInt(8 bits)

    USBSlowSpeedClockDiv := (ClockDomain.current.frequency.getValue / baudrate_slow_speed).toBigInt - 1

    val state = RegInit(StateUnconnected).addTag(crossClockDomain)
    val T1 = Reg(UInt(16 bits)).addTag(crossClockDomain)
    
    fsm_state := state.asBits

    // Check device presence
    when(!io.usb.usb_dm && !io.usb.usb_dp) {
      T1 := T1 + 1
      when(T1 === 1000) { // DM/DP is low for quite some time ?
        T1 := 0
        state := StateUnconnected 
        error := True
        report := True
        received := False
        busy := False
        cmd := CMDNone.asBits.resized // clear last cmd
        cmd_start := False
        received_data_low := 0
        received_data_high := 0
        crc16_received := 0
        crc16_calculated := 0
        crc5_received := 0
        crc5_calculated := 0
      }
    }


    val send_token = new USBSendToken()
    send_token.io.pid := 0
    send_token.io.addr := 0
    send_token.io.endp := 0
    send_token.io.valid := False
    send_token.io.clock_div := USBSlowSpeedClockDiv // for Slow Speed 

    val send_data = new USBSendData()
    send_data.io.pid := 0
    send_data.io.data := 0 
    send_data.io.len := 0 
    send_data.io.valid := False
    send_data.io.clock_div := USBSlowSpeedClockDiv // for Slow Speed 

    io.test := cmd_start //busy //send_token.io.test

    switch(state) {

      is(StateUnconnected) { // Unconnected
        report := False 
        busy := False
        cmd_start := False

        when(io.usb.usb_dm && !io.usb.usb_dp) {
          error := False 
          report := True
          state := StateWaitCMDorSYNC // Low Speed device just connected
        }
      }

      is(StateWaitCMDorSYNC) { // Wait command or SYNC 
        report := False 
        busy := False

        when(cmd_start) {
          switch(cmd) {
            is(CMDSendToken.asBits.resize(4)) {
              state := StateSendToken
              busy := True
            }
            is(CMDSendData.asBits.resize(4)) {
              state := StateSendData
              busy := True
            }
            default {
              cmd_start := False
              report := True
            }
          }
        }
      }

      is(StateSendToken) { // Connected, send SETUP token 
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
        } 
      }

      is(StateSendData) { // send DATA packet
        send_data.io.pid := cmd_pid
        send_data.io.data := send_data_high ## send_data_low // B"64'hAAAAAAAAAAAAAAAA" // 0 //B"01010101010101010101010101010101" ## B"01010101010101010101010101010101" //send_data_high ## send_data_low
        send_data.io.len := cmd_len //64-1 //cmd_len // in bits 
        send_data.io.valid := True
        send_data.io.usb_dm <> io.usb.usb_dm
        send_data.io.usb_dp <> io.usb.usb_dp
        when(send_data.io.ready) {
          state := StateWaitCMDorSYNC 
          report := True
          cmd_start := False
        } 
      }

    } // switch(state)

  } // usb_area

}

