package mylib 

import spinal.core._
import spinal.lib._
import spinal.lib.Counter
import spinal.lib.bus.amba3.apb.{Apb3, Apb3Config, Apb3SlaveFactory}

class Apb3MicroPLICCtrl(irq_nums: Int = 32) extends Component {
	val io = new Bundle {
		val apb = slave( Apb3( addressWidth = 16, dataWidth = 32))
		val externalInterrupt = out Bool()
		val IRQLines = in Bits(irq_nums bits)
	}

	// Define control words and connect it to APB3 bus
	val busCtrl = Apb3SlaveFactory(io.apb)

	val IRQEnabled = Reg(Bits(irq_nums bits)) init(0)
	busCtrl.readAndWrite(IRQEnabled, address = 0)

	val IRQPending = Reg(Bits(irq_nums bits)) init(0)
	busCtrl.readAndWrite(IRQPending, address = 1 * (irq_nums / 8))

	busCtrl.read(io.IRQLines, address = 2 * (irq_nums / 8))

	val IRQPolarity = Reg(Bits(irq_nums bits)) init(0)
	busCtrl.readAndWrite(IRQPolarity, address = 3 * (irq_nums / 8))

	val IRQLastValue = Reg(Bits(irq_nums bits)) init(0)
	busCtrl.read(IRQLastValue, address = 4 * (irq_nums / 8))

	def setIRQ(irq_line: Bool, irq_num: Int): Unit = {
		if(irq_num >= irq_nums) {
			throw new Exception("MicroPLIC: Cannot add IRQ line with number: " + irq_num + " as it is too big!");
		}

		io.IRQLines(irq_num) := irq_line
	}

	for (i <- 0 to (irq_nums - 1)) {
		IRQPending(i).setWhen( IRQEnabled(i) && 
				!(io.IRQLines(i) === IRQLastValue(i)) && 
				(io.IRQLines(i) === IRQPolarity(i)) 
		)
	}

	io.externalInterrupt := !(IRQPending === 0) // trigger external interrupt if there's any pending bits set

	IRQLastValue := io.IRQLines // recall last state of all IRQ lines 
}

