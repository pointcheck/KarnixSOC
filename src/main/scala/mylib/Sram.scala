package mylib 

import spinal.core._
import spinal.lib._
import spinal.lib.bus.simple._
import spinal.lib.io.TriState

case class SramLayout(addressWidth: Int, dataWidth : Int){
  def bytePerWord = dataWidth/8
  def capacity = BigInt(1) << addressWidth
}

case class SramInterface(g : SramLayout) extends Bundle with IMasterSlave{
  val addr = inout(Analog(Bits(g.addressWidth bits)))
  val dat = inout(Analog(Bits(g.dataWidth bits)))
  val cs  = Bool
  val we  = Bool
  val oe  = Bool
  val ble  = Bool
  val bhe  = Bool

  override def asMaster(): Unit = {
    out(cs,we,oe,ble,bhe,addr)
    inout(dat,addr)
  }
}

case class PipelinedMemoryBusSram(pipelinedMemoryBusConfig: PipelinedMemoryBusConfig,
                                       sramLayout : SramLayout) extends Component{
  val io = new Bundle{
    val bus = slave(PipelinedMemoryBus(pipelinedMemoryBusConfig))
    val sram = master(SramInterface(sramLayout))
  }

  val state = Reg(UInt(3 bits)) init(0)
  val rsp_data = Reg(Bits(pipelinedMemoryBusConfig.addressWidth bits)) init(0)
  val rsp_valid = Reg(Bool()) init(False)
  
  io.sram.cs := ~io.bus.cmd.valid
  io.sram.we := True 
  io.sram.oe := True 
  io.sram.bhe := True 
  io.sram.ble := True 

  io.bus.rsp.data := rsp_data; 
  io.bus.rsp.valid := rsp_valid
  io.bus.cmd.ready := (state === 1 && io.bus.cmd.write) || (state === 3 && !io.bus.cmd.write)

  when (io.bus.cmd.valid) {
    when (io.bus.cmd.write) { // Write
      io.sram.we := False // active low 
      when (state === 0) { // Write low 16 bits
        io.sram.addr := io.bus.cmd.address(sramLayout.addressWidth downto 2).asBits ## B"0"
        io.sram.dat := io.bus.cmd.data(15 downto 0).asBits
        io.sram.ble := ~io.bus.cmd.mask(0)
        io.sram.bhe := ~io.bus.cmd.mask(1)
        state := 1
      } otherwise { // Write high 16 bits
        io.sram.addr := io.bus.cmd.address(sramLayout.addressWidth downto 2).asBits ## B"1"
        io.sram.dat := io.bus.cmd.data(31 downto 16).asBits
        io.sram.ble := ~io.bus.cmd.mask(2)
        io.sram.bhe := ~io.bus.cmd.mask(3)
        state := 0
      } 
    } otherwise { // Read
      io.sram.ble := False 
      io.sram.bhe := False 
      io.sram.oe := False 
      when (state === 0) {
        io.sram.addr := io.bus.cmd.address(sramLayout.addressWidth downto 2).asBits ## B"0"
        rsp_data(15 downto 0) := io.sram.dat // buffer low 16 bits - first time 
        state := 1
        rsp_valid := False
      } elsewhen (state === 1) {
        io.sram.addr := io.bus.cmd.address(sramLayout.addressWidth downto 2).asBits ## B"0"
        rsp_data(15 downto 0) := io.sram.dat // buffer low 16 bits - second time
        state := 2
      } elsewhen (state === 2) {
        io.sram.addr := io.bus.cmd.address(sramLayout.addressWidth downto 2).asBits ## B"1"
        rsp_data(31 downto 16) := io.sram.dat // buffer high 16 bits - first time
        state := 3
      } otherwise {
        io.sram.addr := io.bus.cmd.address(sramLayout.addressWidth downto 2).asBits ## B"1"
        rsp_data(31 downto 16) := io.sram.dat // buffer high 16 bits - second time
        rsp_valid := True
        state := 0
      }
    }
  } otherwise { // not Valid
    state := 0
    rsp_valid := False
  }
}

