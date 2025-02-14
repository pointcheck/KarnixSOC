package mylib

import spinal.core._
import spinal.lib._
import spinal.lib.io.TriState
import spinal.lib.Counter
import spinal.lib.bus.amba3.apb.{Apb3, Apb3Config, Apb3SlaveFactory}
import spinal.lib.misc.HexTools

// Usb_hid_host: A compact USB HID host core.
//
// nand2mario, 8/2023, based on work by hi631 (Hiromichi Kitahara)
// https://qiita.com/hi631/items/4f263ca676e4be14b9f8
// https://github.com/hi631/tang-nano-9K/tree/master
// 
// This should support keyboard, mouse and gamepad input out of the box, over low-speed 
// USB (1.5Mbps). Just connect D+, D-, VBUS (5V) and GND, and two 15K resistors between 
// D+ and GND, D- and GND. Then provide a 12Mhz clock through usbclk.
//
// See https://github.com/nand2mario/usb_hid_host

case class USB_HID_host() extends BlackBox{
	val usbclk = in Bool()
	val usbrst_n = in Bool()
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
  }


  val busCtrl = Apb3SlaveFactory(io.apb)

  val usbStatusWord = busCtrl.createReadWrite(Bits(32 bits), address = 0) init(0)
  val conerr = usbStatusWord(31).addTag(crossClockDomain) 
  val report = usbStatusWord(30).addTag(crossClockDomain) 
  val typ = usbStatusWord(29 downto 28).addTag(crossClockDomain)
  val pid = usbStatusWord(27 downto 24).addTag(crossClockDomain)
  val soft_reset = usbStatusWord(0).addTag(crossClockDomain)

  val usbKeyboardWord = busCtrl.createReadOnly(Bits(32 bits), address = 4) init(0)
  val key1 = usbKeyboardWord(7 downto 0).addTag(crossClockDomain) 
  val key2 = usbKeyboardWord(15 downto 8).addTag(crossClockDomain) 
  val key3 = usbKeyboardWord(23 downto 16).addTag(crossClockDomain) 
  val key4 = usbKeyboardWord(31 downto 24).addTag(crossClockDomain) 

  val usbKeyModifiersWord = busCtrl.createReadOnly(Bits(32 bits), address = 8) init(0)
  val key_modifiers = usbKeyModifiersWord(7 downto 0).addTag(crossClockDomain) 

  val usbMouseWord = busCtrl.createReadOnly(Bits(32 bits), address = 12) init(0)
  val mouse_btn = usbMouseWord(7 downto 0).addTag(crossClockDomain) 
  val mouse_dx = usbMouseWord(15 downto 8).asUInt.addTag(crossClockDomain) 
  val mouse_dy = usbMouseWord(23 downto 16).asUInt.addTag(crossClockDomain) 

  val usbGamepadWord = busCtrl.createReadOnly(Bits(32 bits), address = 16) init(0)
  val game_l = usbGamepadWord(0).addTag(crossClockDomain) 
  val game_r = usbGamepadWord(1).addTag(crossClockDomain) 
  val game_u = usbGamepadWord(2).addTag(crossClockDomain) 
  val game_d = usbGamepadWord(3).addTag(crossClockDomain) 
  val game_a = usbGamepadWord(4).addTag(crossClockDomain) 
  val game_b = usbGamepadWord(5).addTag(crossClockDomain) 
  val game_x = usbGamepadWord(6).addTag(crossClockDomain) 
  val game_y = usbGamepadWord(7).addTag(crossClockDomain) 
  val game_sel = usbGamepadWord(8).addTag(crossClockDomain) 
  val game_sta = usbGamepadWord(9).addTag(crossClockDomain) 

  val usbDbgWord1 = busCtrl.createReadOnly(Bits(32 bits), address = 20) init(0)
  val dbg_hid_report_low = usbDbgWord1(31 downto 0).addTag(crossClockDomain) 
  val usbDbgWord2 = busCtrl.createReadOnly(Bits(32 bits), address = 24) init(0)
  val dbg_hid_report_high = usbDbgWord2(31 downto 0).addTag(crossClockDomain) 

  val usbCRCWord = busCtrl.createReadOnly(Bits(32 bits), address = 28) init(0)
  val crc16_received = usbCRCWord(15 downto 0).addTag(crossClockDomain)
  val crc16_calculated = usbCRCWord(31 downto 16).addTag(crossClockDomain)

  val usbClockDomain = ClockDomain(
    clock = io.usbclk_12mhz,
    config = ClockDomainConfig(resetKind = BOOT, resetActiveLevel = LOW),
    frequency = FixedFrequency(12.0 MHz)
  )

  val usb_area = new ClockingArea(usbClockDomain) {

    val usb_hid_host = new USB_HID_host()

    usb_hid_host.usbclk := io.usbclk_12mhz
    //usb_hid_host.usbrst_n := ClockDomain.current.readResetWire
    usb_hid_host.usbrst_n := soft_reset 
    usb_hid_host.usb_dp <> io.usb.usb_dp
    usb_hid_host.usb_dm <> io.usb.usb_dm

    conerr := usb_hid_host.conerr
    report := usb_hid_host.report
    typ := usb_hid_host.typ

    key1 := usb_hid_host.key1
    key2 := usb_hid_host.key2
    key3 := usb_hid_host.key3
    key4 := usb_hid_host.key4

    game_l := usb_hid_host.game_l
    game_r := usb_hid_host.game_r
    game_u := usb_hid_host.game_u
    game_d := usb_hid_host.game_d
    game_a := usb_hid_host.game_a
    game_b := usb_hid_host.game_b
    game_x := usb_hid_host.game_x
    game_y := usb_hid_host.game_y
    game_sel := usb_hid_host.game_sel
    game_sta := usb_hid_host.game_sta

    key_modifiers := usb_hid_host.key_modifiers

    mouse_btn := usb_hid_host.mouse_btn

    dbg_hid_report_low := usb_hid_host.dbg_hid_report(31 downto 0) 
    dbg_hid_report_high := usb_hid_host.dbg_hid_report(63 downto 32) 
    crc16_received := usb_hid_host.crc16_received
    crc16_calculated := usb_hid_host.crc16_calculated
    pid := usb_hid_host.pid(3 downto 0)

    //io.interrupt := usb_hid_host.report & crc16_received === crc16_calculated
    io.interrupt := usb_hid_host.report & (crc16_received === crc16_calculated)
  }

}

