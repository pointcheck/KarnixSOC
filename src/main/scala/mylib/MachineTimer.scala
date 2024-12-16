package mylib 

import spinal.core._
import spinal.lib._
import spinal.lib.bus.amba3.apb._
import spinal.lib.bus.misc._

case class MachineTimer(clockMHz: Int = 25) extends Component {
  val io = new Bundle {
    val micros = out UInt(32 bits)
  }

  val counter = Reg(UInt(8 bits))
  val microCounter = Reg(UInt(32 bits))
  io.micros := microCounter

  counter := counter + 1

  when (counter === clockMHz - 1) {
    counter := 0;
    microCounter := microCounter + 1
  }

  def driveFrom(busCtrl : BusSlaveFactory, baseAddress : Int = 0) () = new Area {
    busCtrl.read(io.micros, baseAddress)
  }
}

/*
 * Micros -> 0x00 Read register for micros since start-up
 **/
case class Apb3MachineTimer(clockMHz : Int = 25) extends Component {
  val io = new Bundle {
    val apb = slave(Apb3(Apb3Config(addressWidth = 8, dataWidth = 32)))
  }

  val busCtrl = Apb3SlaveFactory(io.apb)
  val mtCtrl = MachineTimer(clockMHz)

  mtCtrl.driveFrom(busCtrl)()
}
