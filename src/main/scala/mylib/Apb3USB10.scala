package mylib

import spinal.core._
import spinal.lib._
import spinal.lib.io.TriState
import spinal.lib.Counter
import spinal.lib.bus.amba3.apb.{Apb3, Apb3Config, Apb3SlaveFactory}
import spinal.lib.misc.HexTools

/*
case class USBInterface() extends Bundle with IMasterSlave{
  val usb_dm = inout(Analog(Bool()))
  val usb_dp = inout(Analog(Bool()))

  override def asMaster(): Unit = {
    inout(usb_dm, usb_dp)
  }
}
*/

case class USBSendToken() extends Component {
    val io = new Bundle {
	val usb_dm    = inout(Analog(Bool()))
	val usb_dp    = inout(Analog(Bool()))
	val valid     = in Bool()
	val ready     = out Bool()
	val pid       = in Bits(4 bits)
	val addr      = in Bits(7 bits)
	val endp      = in Bits(4 bits)
	val clock_div = in UInt(3 bits)

        val test = out Bool()
    }

    def crc5(din: Bits) : Bits = {
      val ret = Bits(5 bits)
      ret(0) := din(10) ^ din(9) ^ din(6) ^ din(5) ^ din(3) ^ din(0) ^ True 
      ret(1) := din(10) ^ din(7) ^ din(6) ^ din(4) ^ din(1) ^ True 
      ret(2) := din(10) ^ din(9) ^ din(8) ^ din(7) ^ din(6) ^ din(3) ^ din(2) ^ din(0) ^ True 
      ret(3) := din(10) ^ din(9) ^ din(8) ^ din(7) ^ din(4) ^ din(3) ^ din(1) 
      ret(4) := din(10) ^ din(9) ^ din(8) ^ din(5) ^ din(4) ^ din(2) ^ True 
      return ret
    }

    val last_kj = Reg(Bool())

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

    val crc5_out = crc5(io.addr(0) ## io.addr(1) ## io.addr(2) ## io.addr(3) ##
                        io.addr(4) ## io.addr(5) ## io.addr(6) ## io.endp(0) ##
                        io.endp(1) ## io.endp(2) ## io.endp(3)) ^ B"11111" 
    //val buffer = crc5_out ## io.endp ## io.addr ## ~io.pid ## io.pid ## B"10000000"
    val buffer = crc5_out ## io.endp ## io.addr ## ~io.pid ## io.pid ## B"1000000"
    val bit_count = Reg(UInt(6 bits))
    val clock_div = Reg(UInt(3 bits))
    val clock_strobe = False

    io.test := io.valid

    io.ready := False

    // J: D- = 1, D+ = 0, K: D- = 0, D+ = 1
    // KJKJKJKK + PID + ADDR + ENDP + CRC5 

    when(io.valid) {
      clock_div := clock_div + 1 

      when(bit_count === 35) { // Ready 
        io.ready := True 
        bit_count := 0
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

      when(clock_div === io.clock_div) {
        clock_div := 0
        clock_strobe := True
      }

      when(clock_strobe) {
        bit_count := bit_count + 1 
      }

    } otherwise {
      clock_div := 0
      bit_count := 0
      //last_kj := False // 'J'
      last_kj := True // 'K' 
    }
}

object USBPhase extends SpinalEnum{
  val StateUconnected, StateWaitCMDorSYNC, StateSendToken
      = newElement()
}

object USBCommand extends SpinalEnum{
  val CMDSendToken, CMDSendData
      = newElement()
}

case class Apb3USB10Ctrl(
      ) extends Component {
  val io = new Bundle {
    val apb       = slave(Apb3(addressWidth = 16, dataWidth = 32))
    val usb       = master(USBInterface())
    val interrupt = out Bool()
    val usbclk_12mhz = in Bool()

    val test = out Bool()
  }

  import USBPhase._
  import USBCommand._

  val busCtrl = Apb3SlaveFactory(io.apb)

  val usbStatusWord = busCtrl.createReadWrite(Bits(32 bits), address = 0) init(0)
  val error = usbStatusWord(31).addTag(crossClockDomain).addTag(crossClockDomain)
  val report = usbStatusWord(30).addTag(crossClockDomain).addTag(crossClockDomain) 
  val cmd_busy = usbStatusWord(28).addTag(crossClockDomain).addTag(crossClockDomain) 
  val pid = usbStatusWord(27 downto 24).addTag(crossClockDomain).addTag(crossClockDomain)
  val soft_reset = usbStatusWord(0).addTag(crossClockDomain).addTag(crossClockDomain)

  val usbCommandWord = busCtrl.createReadWrite(Bits(32 bits), address = 4) init(0)
  val cmd = usbCommandWord(7 downto 0).addTag(crossClockDomain).addTag(crossClockDomain) 
  val cmd_start = usbCommandWord(31).addTag(crossClockDomain).addTag(crossClockDomain) 

  val usbDataReceivedLowWord = busCtrl.createReadOnly(Bits(32 bits), address = 8) init(0)
  val received_data_low = usbDataReceivedLowWord(31 downto 0).addTag(crossClockDomain).addTag(crossClockDomain) 

  val usbDataReceivedHighWord = busCtrl.createReadOnly(Bits(32 bits), address = 12) init(0)
  val received_data_high = usbDataReceivedHighWord(31 downto 0).addTag(crossClockDomain).addTag(crossClockDomain) 

  val usbSendLowWord = busCtrl.createReadWrite(Bits(32 bits), address = 16) init(0)
  val send_data_low = usbSendLowWord(31 downto 0).addTag(crossClockDomain).addTag(crossClockDomain) 

  val usbSendHighWord = busCtrl.createReadWrite(Bits(32 bits), address = 20) init(0)
  val send_data_high = usbSendHighWord(31 downto 0).addTag(crossClockDomain).addTag(crossClockDomain) 

  val usbCRC16Word = busCtrl.createReadOnly(Bits(32 bits), address = 24) init(0)
  val crc16_received = usbCRC16Word(15 downto 0).addTag(crossClockDomain).addTag(crossClockDomain)
  val crc16_calculated = usbCRC16Word(31 downto 16).addTag(crossClockDomain).addTag(crossClockDomain)

  val usbCRC5Word = busCtrl.createReadOnly(Bits(32 bits), address = 28) init(0)
  val crc5_received = usbCRC5Word(4 downto 0).addTag(crossClockDomain).addTag(crossClockDomain)
  val crc5_calculated = usbCRC5Word(11 downto 8).addTag(crossClockDomain).addTag(crossClockDomain)

  io.interrupt := report 

  val usbClockDomain = ClockDomain(
    clock = io.usbclk_12mhz,
    reset = soft_reset,
    config = ClockDomainConfig(resetKind = SYNC, resetActiveLevel = LOW),
    frequency = FixedFrequency(12.0 MHz)
  )

  val usb_area = new ClockingArea(usbClockDomain) {

    val T1 = Reg(UInt(16 bits)) init(0) // Guard Timer 
    val state = Reg(UInt(4 bits)).addTag(crossClockDomain) init(0)
    
    // Check device presence
    when(!io.usb.usb_dm && !io.usb.usb_dp) {
      T1 := T1 + 1
      when(T1 === 1000) {
        T1 := 0
        state := UNCONNECTED
        error := True
        report := True
        cmd := 0 // clear last cmd
      }
    }

    val send_token = new USBSendToken()
    send_token.io.pid := 0
    send_token.io.addr := 0
    send_token.io.endp := 0
    send_token.io.valid := False
    send_token.io.clock_div := 7 // (12.0 MHz / 1.5 MHz - 1) for Slow Speed 

    io.test := send_token.io.test

    switch(state) {

      is(Unconnected) { // Unconnected
        report := False 

        when(io.usb.usb_dm && !io.usb.usb_dp) {
          error := False 
          report := True
          state := StateWaitCMDorSYNC // Low Speed device just connected
        }
      }

      is(StateWaitCMDorSYNC) { // Wait command or SYNC 
        report := False 
        cmd_busy := False

        when(cmd_start) {
          when(cmd === CMDSendToken) {
            sate := StateSendToken
          }
        }
      }

      is(StateSendToken) { // Connected, send SETUP token 
        io.report := True
        send_token.io.pid := B"1101" // (1011 0100) SETUP token
        send_token.io.addr := 0
        send_token.io.endp := 0
        send_token.io.valid := True
        send_token.io.usb_dm <> io.usb_dm
        send_token.io.usb_dp <> io.usb_dp
        when(send_token.io.ready) {
          state := StateWaitCMDorSYNC 
          report := True
        } 
      }

    } // switch(state)

  } // usb_area

}

