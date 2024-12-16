package mylib

import spinal.core._
import spinal.lib._
import spinal.lib.Counter
import spinal.lib.bus.amba3.apb.{Apb3, Apb3Config, Apb3SlaveFactory}



class Apb3TimerCtrl() extends Component{
	val io = new Bundle {
		val apb = slave(Apb3(
			addressWidth = 8,
			dataWidth = 32
		))
		val interrupt = out Bool()
	}

	val timer_enable = Reg(Bool) init(False)
	val timer_prescaler = Reg(UInt(32 bits)) init(0)
	val timer_prescaler_counter = Reg(UInt(32 bits)) init(0)
	val timer_counter = Reg(UInt(32 bits)) init(0)
	val timer_limit = Reg(UInt(32 bits)) init(0)

	val busCtrl = Apb3SlaveFactory(io.apb)
	busCtrl.readAndWrite(timer_enable, address = 0)
	busCtrl.readAndWrite(timer_prescaler, address = 4)
	busCtrl.readAndWrite(timer_prescaler_counter, address = 8)
	busCtrl.readAndWrite(timer_counter, address = 12)
	busCtrl.readAndWrite(timer_limit, address = 16)

	io.interrupt := False 

	when(timer_enable) {
		when(timer_prescaler_counter === 0) {
			timer_prescaler_counter := timer_prescaler
			
			when(timer_counter === timer_limit) {
				io.interrupt := True
			} otherwise {
				timer_counter := timer_counter + 1
			}
		} otherwise {
			timer_prescaler_counter := timer_prescaler_counter - 1 
		}
	}
}


