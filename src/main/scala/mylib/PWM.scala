package mylib

import spinal.core._
import spinal.lib._
import spinal.lib.Counter
import spinal.lib.bus.amba3.apb.{Apb3, Apb3Config, Apb3SlaveFactory}

class Pwm(size: Int) extends Component {
  val io = new Bundle {
    val pulse_width = in UInt (size bits)
    val pulse_top = in UInt (size bits)
    val output = out Bool()
    val enable = in Bool()
  }

  val ct = Counter(size bits)

  when(io.enable) {
	ct.increment()

	when(ct.value > io.pulse_top) { 
		ct.clear() 
	}
	
	io.output := ct.value < io.pulse_width

  } otherwise {

	io.output := False;
  }
  
}

class Apb3PwmCtrl(size: Int) extends Component {
  val io = new Bundle {
    val apb = slave(
      Apb3(
        addressWidth = 4,
        dataWidth = 32
      )
    )
    val output = out Bool()
  }

  val pwm = new Pwm(size)
  io.output := pwm.io.output

  val busCtrl = Apb3SlaveFactory(io.apb)
  busCtrl.driveAndRead(pwm.io.pulse_width, address = 0)
  busCtrl.driveAndRead(pwm.io.pulse_top, address = 4)
  busCtrl.driveAndRead(pwm.io.enable, address = 8)

}


