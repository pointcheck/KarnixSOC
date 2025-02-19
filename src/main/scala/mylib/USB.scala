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
      } otherwise {
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

case class USB_hid_host() extends Component {
    val io = new Bundle {
        val usbclk_12mhz = in Bool()
        val nreset = in Bool()
	val usb_dm = inout(Analog(Bool()))
	val usb_dp = inout(Analog(Bool()))

        // Status
	val typ = out Bits(2 bits)
        val report = out Bool()
        val conerr = out Bool()

        // keyboard
        val key_modifiers = out Bits(8 bits)
        val key1 = out Bits(8 bits)
        val key2 = out Bits(8 bits)
        val key3 = out Bits(8 bits)
        val key4 = out Bits(8 bits)

        // mouse
        val mouse_btn = out Bits(8 bits)  // {5'bx, middle, right, left}
        val mouse_dx = out UInt(8 bits)   // signed 8-bit, cleared after `report` pulse
        val mouse_dy = out UInt(8 bits)   // signed 8-bit, cleared after `report` pulse

        // gamepad 
        // left right up down
        val game_l = out Bool()
        val game_r = out Bool()
        val game_u = out Bool()
        val game_d = out Bool()
        // buttons
        val game_a = out Bool()
        val game_b = out Bool()
        val game_x = out Bool()
        val game_y = out Bool()
        val game_sel = out Bool()
        val game_sta = out Bool()

        // debug
        val dbg_hid_report = out Bits(64 bits)  // last HID report

        val crc16_received = out Bits(16 bits)  // last HID report received CRC16
        val crc16_calculated = out Bits(16 bits)  // last HID report calculated CRC16
        val pid = out Bits(8 bits)  // last PID 

        val test = out Bool()
    }

    val usbClockDomain = ClockDomain(
      clock = io.usbclk_12mhz,
      reset = io.nreset,
      config = ClockDomainConfig(resetKind = ASYNC, resetActiveLevel = LOW),
      frequency = FixedFrequency(12.0 MHz)
    )

    val usb_area = new ClockingArea(usbClockDomain) {

      val T1 = Reg(UInt(16 bits)) init(0) // Disconnect timer
      val T2 = Reg(UInt(16 bits)) init(0) // 1ms timer
      val state = Reg(UInt(4 bits)).addTag(crossClockDomain) init(0)
      // 0 - unconnected
      // 1 - connected Low Speed, handshake
      // 15 - error 

      val conerr = Reg(Bool()).addTag(crossClockDomain) init(True)

      io.crc16_received := B"16'xFFFF" 
      io.crc16_calculated := B"16'xFFFF" 

      io.pid := 0
      io.typ := 0
      io.report := False 
      io.conerr := conerr
      io.key_modifiers := 0
      io.key1 := 0
      io.key2 := 0
      io.key3 := 0
      io.key4 := 0
      io.mouse_btn := 0
      io.mouse_dx := 0
      io.mouse_dy := 0
      io.game_l := False
      io.game_r := False
      io.game_u := False
      io.game_d := False
      io.game_a := False
      io.game_b := False
      io.game_x := False
      io.game_y := False
      io.game_sel := False
      io.game_sta := False
      io.dbg_hid_report := 0

      val send_token = new USBSendToken()
      send_token.io.pid := 0
      send_token.io.addr := 0
      send_token.io.endp := 0
      send_token.io.valid := False
      send_token.io.clock_div := 7 // (12.0 MHz / 1.5 MHz - 1) for Slow Speed 

io.test := send_token.io.test

      when(!io.usb_dm && !io.usb_dp) {
        T1 := T1 + 1
        when(T1 === 1000) {
          state := 0 // Unconnected
          T1 := 0
          T2 := 0
          conerr := True
        }
      }

      switch(state) {

        is(0) { // Unconnected
          when(io.usb_dm && !io.usb_dp) {
            state := 1 // Low Speed just connected
          }
        }

        is(1) { // Connected, SETUP 
          conerr := False
          io.report := True
          send_token.io.pid := B"1101" // (1011 0100) SETUP token
          send_token.io.addr := 0
          send_token.io.endp := 0
          send_token.io.valid := True
          send_token.io.usb_dm <> io.usb_dm
          send_token.io.usb_dp <> io.usb_dp
          when(send_token.io.ready) {
            state := 2
          } 
        }

        is(2) { // SETUP sent, wait 1ms
          T2 := T2 + 1
          when(T2 === 12000) {
            state := 1 // send SETUP again
            T2 := 0
          }
        }

        default {
        }
      }

    } // ClockingArea

}

case class USBInterface() extends Bundle with IMasterSlave{
  val usb_dm = inout(Analog(Bool()))
  val usb_dp = inout(Analog(Bool()))

  override def asMaster(): Unit = {
    inout(usb_dm, usb_dp)
  }
}

case class Apb3USBCtrl(
      ) extends Component {
  val io = new Bundle {
    val apb       = slave(Apb3(addressWidth = 16, dataWidth = 32))
    val usb       = master(USBInterface())
    val interrupt = out Bool()
    val usbclk_12mhz = in Bool()

    val test = out Bool()
  }


  val busCtrl = Apb3SlaveFactory(io.apb)

  val usbStatusWord = busCtrl.createReadWrite(Bits(32 bits), address = 0) init(0)
  val conerr = usbStatusWord(31).addTag(crossClockDomain).addTag(crossClockDomain)
  val report = usbStatusWord(30).addTag(crossClockDomain).addTag(crossClockDomain) 
  val typ = usbStatusWord(29 downto 28).addTag(crossClockDomain).addTag(crossClockDomain)
  val pid = usbStatusWord(27 downto 24).addTag(crossClockDomain).addTag(crossClockDomain)
  val soft_reset = usbStatusWord(0).addTag(crossClockDomain).addTag(crossClockDomain)

  val usbKeyboardWord = busCtrl.createReadOnly(Bits(32 bits), address = 4) init(0)
  val key1 = usbKeyboardWord(7 downto 0).addTag(crossClockDomain).addTag(crossClockDomain) 
  val key2 = usbKeyboardWord(15 downto 8).addTag(crossClockDomain).addTag(crossClockDomain) 
  val key3 = usbKeyboardWord(23 downto 16).addTag(crossClockDomain).addTag(crossClockDomain) 
  val key4 = usbKeyboardWord(31 downto 24).addTag(crossClockDomain).addTag(crossClockDomain) 

  val usbKeyModifiersWord = busCtrl.createReadOnly(Bits(32 bits), address = 8) init(0)
  val key_modifiers = usbKeyModifiersWord(7 downto 0).addTag(crossClockDomain).addTag(crossClockDomain) 

  val usbMouseWord = busCtrl.createReadOnly(Bits(32 bits), address = 12) init(0)
  val mouse_btn = usbMouseWord(7 downto 0).addTag(crossClockDomain).addTag(crossClockDomain) 
  val mouse_dx = usbMouseWord(15 downto 8).asUInt.addTag(crossClockDomain).addTag(crossClockDomain) 
  val mouse_dy = usbMouseWord(23 downto 16).asUInt.addTag(crossClockDomain).addTag(crossClockDomain) 

  val usbGamepadWord = busCtrl.createReadOnly(Bits(32 bits), address = 16) init(0)
  val game_l = usbGamepadWord(0).addTag(crossClockDomain).addTag(crossClockDomain) 
  val game_r = usbGamepadWord(1).addTag(crossClockDomain).addTag(crossClockDomain) 
  val game_u = usbGamepadWord(2).addTag(crossClockDomain).addTag(crossClockDomain) 
  val game_d = usbGamepadWord(3).addTag(crossClockDomain).addTag(crossClockDomain) 
  val game_a = usbGamepadWord(4).addTag(crossClockDomain).addTag(crossClockDomain) 
  val game_b = usbGamepadWord(5).addTag(crossClockDomain).addTag(crossClockDomain) 
  val game_x = usbGamepadWord(6).addTag(crossClockDomain).addTag(crossClockDomain) 
  val game_y = usbGamepadWord(7).addTag(crossClockDomain).addTag(crossClockDomain) 
  val game_sel = usbGamepadWord(8).addTag(crossClockDomain).addTag(crossClockDomain) 
  val game_sta = usbGamepadWord(9).addTag(crossClockDomain).addTag(crossClockDomain) 

  val usbDbgWord1 = busCtrl.createReadOnly(Bits(32 bits), address = 20) init(0)
  val dbg_hid_report_low = usbDbgWord1(31 downto 0).addTag(crossClockDomain).addTag(crossClockDomain) 
  val usbDbgWord2 = busCtrl.createReadOnly(Bits(32 bits), address = 24) init(0)
  val dbg_hid_report_high = usbDbgWord2(31 downto 0).addTag(crossClockDomain).addTag(crossClockDomain) 

  val usbCRCWord = busCtrl.createReadOnly(Bits(32 bits), address = 28) init(0)
  val crc16_received = usbCRCWord(15 downto 0).addTag(crossClockDomain).addTag(crossClockDomain)
  val crc16_calculated = usbCRCWord(31 downto 16).addTag(crossClockDomain).addTag(crossClockDomain)

  val usb_hid_host = new USB_hid_host()

io.test := usb_hid_host.io.test

  usb_hid_host.io.usbclk_12mhz := io.usbclk_12mhz 
  usb_hid_host.io.nreset := soft_reset 
  usb_hid_host.io.usb_dp <> io.usb.usb_dp
  usb_hid_host.io.usb_dm <> io.usb.usb_dm

  conerr := usb_hid_host.io.conerr
  report := usb_hid_host.io.report
  typ := usb_hid_host.io.typ

  key1 := usb_hid_host.io.key1
  key2 := usb_hid_host.io.key2
  key3 := usb_hid_host.io.key3
  key4 := usb_hid_host.io.key4

  game_l := usb_hid_host.io.game_l
  game_r := usb_hid_host.io.game_r
  game_u := usb_hid_host.io.game_u
  game_d := usb_hid_host.io.game_d
  game_a := usb_hid_host.io.game_a
  game_b := usb_hid_host.io.game_b
  game_x := usb_hid_host.io.game_x
  game_y := usb_hid_host.io.game_y
  game_sel := usb_hid_host.io.game_sel
  game_sta := usb_hid_host.io.game_sta

  key_modifiers := usb_hid_host.io.key_modifiers

  mouse_btn := usb_hid_host.io.mouse_btn

  dbg_hid_report_low := usb_hid_host.io.dbg_hid_report(31 downto 0) 
  dbg_hid_report_high := usb_hid_host.io.dbg_hid_report(63 downto 32) 
  crc16_received := usb_hid_host.io.crc16_received
  crc16_calculated := usb_hid_host.io.crc16_calculated
  pid := usb_hid_host.io.pid(3 downto 0)

  io.interrupt := usb_hid_host.io.report & (crc16_received === crc16_calculated)
}

