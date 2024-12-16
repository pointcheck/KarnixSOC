package mylib

import spinal.core._
import spinal.lib._
import spinal.lib.bus.amba3.apb.{Apb3, Apb3Config, Apb3SlaveFactory}
import spinal.lib.eda.altera.QSysify
import spinal.lib.com.eth._

object Apb3MacEthCtrl{
  def getApb3Config = Apb3Config(
    addressWidth = 16,
    dataWidth = 32,
    selWidth = 1,
    useSlaveError = false
  )
}


case class Apb3MacEthCtrl(p : MacEthParameter) extends Component{
	val io = new Bundle{
		val apb =  slave(Apb3(Apb3MacEthCtrl.getApb3Config))
		val interrupt = out Bool()
		val mii = master(Mii( MiiParameter( MiiTxParameter( dataWidth = p.phy.txDataWidth, withEr = false), MiiRxParameter( dataWidth = p.phy.rxDataWidth))))
	}

	val txCd = ClockDomain(io.mii.TX.CLK) 
	val rxCd = ClockDomain(io.mii.RX.CLK) 
	val mac = new MacEth(p, txCd, rxCd)
	val phy = PhyIo(p.phy)
	phy <> mac.io.phy

	txCd.copy(reset = mac.txReset) on {
		val tailer = MacTxInterFrame(dataWidth = p.phy.txDataWidth)
		tailer.io.input << phy.tx

		io.mii.TX.EN := RegNext(tailer.io.output.valid)
		io.mii.TX.D := RegNext(tailer.io.output.data)
	}

	rxCd on {
		phy.rx << io.mii.RX.toRxFlow().toStream
	}

	val busCtrl = Apb3SlaveFactory(io.apb)
	val bridge = mac.io.ctrl.driveFrom(busCtrl)
	io.interrupt := bridge.interruptCtrl.pending
}



