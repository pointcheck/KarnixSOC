package mylib

import spinal.core._
import spinal.lib._
import spinal.lib.Counter
import spinal.lib.bus.amba3.apb.{Apb3, Apb3Config, Apb3SlaveFactory}



class Apb3WatchDogCtrl() extends Component{
	val io = new Bundle {
		val apb = slave(Apb3(
			addressWidth = 8,
			dataWidth = 32
		))
		val hard_reset = out Bool()
	}

	val wd_enable = Reg(Bool) init(False)
	val wd_counter = Reg(UInt(32 bits)) init(0)
	val wd_limit = Reg(UInt(32 bits)) init(0)
	val wd_reboot = Reg(Bool) init(False)

	val busCtrl = Apb3SlaveFactory(io.apb)
	busCtrl.readAndWrite(wd_enable, address = 0)
	busCtrl.readAndWrite(wd_limit, address = 4)
	busCtrl.readAndWrite(wd_reboot, address = 8)
	busCtrl.readAndWrite(wd_counter, address = 12)

	io.hard_reset := !wd_reboot // no reset 

	when(wd_enable) {
		when(wd_counter === wd_limit) {
			wd_reboot := True
			wd_counter := 0
		} otherwise {
			wd_counter := wd_counter + 1
		}
	}
}


