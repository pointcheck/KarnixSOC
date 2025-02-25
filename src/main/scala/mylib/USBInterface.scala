package mylib

import spinal.core._
import spinal.lib._
import spinal.lib.io.TriState

case class USBInterface() extends Bundle with IMasterSlave{
  val usb_dm = inout(Analog(Bool()))
  val usb_dp = inout(Analog(Bool()))

  override def asMaster(): Unit = {
    inout(usb_dm, usb_dp)
  }
}

