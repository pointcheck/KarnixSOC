package spinal.lib.com.uart 

import spinal.core._
import spinal.lib._


case class Uart(config : UartCtrlGenerics = UartCtrlGenerics(ctsGen = false, rtsGen = false, deGen = false)) extends Bundle with IMasterSlave {
  val txd = Bool
  val rxd = Bool
  val cts = config.ctsGen generate Bool()
  val rts = config.rtsGen generate Bool()
  val de = config.deGen generate Bool()

  override def asMaster(): Unit = {
    out(txd)
    in(rxd)
    outWithNull(rts)
    outWithNull(de)
    inWithNull(cts)
  }
}


object UartParityType extends SpinalEnum(binarySequential) {
  val NONE, EVEN, ODD = newElement()
}


object UartStopType extends SpinalEnum(binarySequential) {
  val ONE, TWO = newElement()
  def toBitCount(that : C) : UInt = (that === ONE) ? U"0" | U"1"
}
