package mylib

import spinal.core._
import spinal.lib._
import spinal.lib.io.TriState

case class USBInterface() extends Bundle with IMasterSlave{
  val dm = inout(Analog(Bool()))
  val dp = inout(Analog(Bool()))

  override def asMaster(): Unit = {
    inout(dm, dp)
  }
}

