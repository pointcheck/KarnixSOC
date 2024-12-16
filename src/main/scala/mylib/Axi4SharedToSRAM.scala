package mylib 

import spinal.core._
import spinal.lib._
import spinal.lib.bus.amba4.axi.{Axi4, Axi4Shared, Axi4Config}


/**
  * State of the state machine of the wrapper
  */
object Axi4ToSRAMPhase extends SpinalEnum{
  val SETUP, ACCESSLow, ACCESSHigh, RESPONSE = newElement()
}


object Axi4SharedToSram {

  /**
    * Return the axi and sram configuration
    */
  def getConfigs(addressAxiWidth: Int, dataWidth: Int, idWidth: Int, addressSRAMWidth: Int, dataSRAMWidth: Int): (Axi4Config, SramLayout) =
    (
      Axi4Config(
        addressWidth = addressAxiWidth,
        dataWidth    = dataWidth,
        idWidth      = idWidth,
        useLock      = false,
        useRegion    = false,
        useCache     = false,
        useProt      = false,
        useQos       = false
      ),
      SramLayout(
        dataWidth    = dataSRAMWidth,
        addressWidth = addressSRAMWidth
      )
    )
}


/**
  * Axi4 <-> SRAM bus with burst
  * WARNING do not support byte mask !!
  */
case class Axi4SharedToSram(addressAxiWidth: Int, dataWidth: Int, idWidth: Int, addressSRAMWidth: Int, dataSRAMWidth: Int) extends Component {
  import Axi4ToSRAMPhase._

  assert(addressAxiWidth >= addressSRAMWidth, "Address of the SRAM bus can NOT be bigger than the Axi address")

  val (axiConfig, sramLayout) = Axi4SharedToSram.getConfigs(addressAxiWidth, dataWidth, idWidth, addressSRAMWidth, dataSRAMWidth)

  val io = new Bundle{
    val axi  = slave (Axi4Shared(axiConfig))
    val sram = master(SramInterface(sramLayout))
  }

  val phase      = RegInit(SETUP)
  val lenBurst   = Reg(cloneOf(io.axi.arw.len))
  val arw        = Reg(cloneOf(io.axi.arw.payload))
  val readData   = Reg(cloneOf(io.axi.r.data))

  def isEndBurst = lenBurst === 0

  io.axi.arw.ready  := False
  io.axi.w.ready    := False
  io.axi.b.valid    := False
  io.axi.b.resp     := Axi4.resp.OKAY
  io.axi.b.id       := arw.id
  io.axi.r.valid    := False
  io.axi.r.resp     := Axi4.resp.OKAY
  io.axi.r.id       := arw.id
  io.axi.r.data     := readData
  io.axi.r.last     := isEndBurst && !arw.write

/*
  val addr = inout(Analog(Bits(g.addressWidth bits)))
  val dat = inout(Analog(Bits(g.dataWidth bits)))
  val cs  = Bool
  val we  = Bool
  val oe  = Bool
  val ble  = Bool
  val bhe  = Bool
*/

  io.sram.cs        := !False
  io.sram.oe        := !False
  io.sram.ble       := !True
  io.sram.bhe       := !True
  //io.sram.addr      := arw.addr.asBits(sramLayout.addressWidth downto 2).asBits ## B"0"
  //io.sram.dat       := io.axi.w.data(15 downto 0)
  io.sram.we        := !False

  /**
    * Main state machine
    */
  val sm = new Area {
    switch(phase){
      is(SETUP){
        arw       := io.axi.arw
        lenBurst  := io.axi.arw.len

        when(io.axi.arw.valid){
          io.sram.cs       := !True
          io.axi.arw.ready := True
          phase            := ACCESSLow
        }
      }
      is(ACCESSLow){
        io.sram.cs := !True
        io.sram.addr := arw.addr.asBits(sramLayout.addressWidth downto 2).asBits ## B"0"

        when(arw.write){
          io.sram.we  := !True
          io.sram.dat := io.axi.w.data(15 downto 0)
          io.sram.ble := !io.axi.w.strb(0)
          io.sram.bhe := !io.axi.w.strb(1) 
        } otherwise {
          io.sram.oe  := !True
          readData(15 downto 0) := io.sram.dat
        }

        phase := ACCESSHigh 
      }
      is(ACCESSHigh){
        io.sram.cs := !True
        io.sram.addr := arw.addr.asBits(sramLayout.addressWidth downto 2).asBits ## B"1"

        when(arw.write){ // Write High 16 bits
          io.sram.we  := !True
          io.sram.ble := !io.axi.w.strb(2)
          io.sram.bhe := !io.axi.w.strb(3) 
          io.sram.dat := io.axi.w.data(31 downto 16)
        } otherwise { // Read High 16 bits
          io.sram.oe  := !True
          readData(31 downto 16) := io.sram.dat
        }

        // If read or write, increment beat address
        when(io.axi.w.valid || !arw.write){
          arw.addr   := Axi4.incr(arw.addr, arw.burst, arw.len, arw.size, 4)
        }

        when(io.axi.w.last || arw.len === 0){
          phase := RESPONSE
        }

        io.axi.w.ready   := io.axi.w.valid & arw.write
      }
      default{ // RESPONSE
        when(arw.write){
          io.axi.b.valid := True
          when(io.axi.b.ready){
            phase := SETUP
          }
        }.otherwise {
          io.axi.r.valid   := True
          when(io.axi.r.ready){
            when(isEndBurst){
              phase    := SETUP
            }otherwise{ // Repeat next beat in the burst
              lenBurst := lenBurst - 1
              phase    := ACCESSLow
            }
          }
        }
      }
    }
  }
}
