package mylib 

import spinal.core._
import spinal.lib._
import spinal.lib.Counter
import spinal.lib.bus.amba3.apb.{Apb3, Apb3Config, Apb3SlaveFactory}


case class MicroI2CInterface() extends Bundle with IMasterSlave{
  val sda = inout(Analog(Bool()))
  val scl = inout(Analog(Bool()))

  override def asMaster(): Unit = {
    out(scl)
    inout(sda)
  }
}



class Apb3MicroI2CCtrl(busWidth: Int = 32) extends Component {
	val io = new Bundle {
		val apb = slave( Apb3( addressWidth = 16, dataWidth = 32))
		val interrupt = out Bool()
		val i2c = MicroI2CInterface()
	}

	io.interrupt := False;

	// Define control words and connect it to APB3 bus
	val busCtrl = Apb3SlaveFactory(io.apb)

	val tx_data = Reg(Bits(busWidth bits)) init(0)
	busCtrl.readAndWrite(tx_data, address = 0x00)

	val rx_data = Reg(Bits(busWidth bits)) init(0)
	busCtrl.read(rx_data, address = 0x04)

	val clock_div = Reg(UInt(busWidth bits)) init(0)
	busCtrl.readAndWrite(clock_div, address = 0x08)

	val enable = Reg(Bool()) init(False)
	busCtrl.readAndWrite(enable, address = 0x0C, bitOffset = 0)

	val tx_busy = Reg(Bool()) init(False)
	busCtrl.read(tx_busy, address = 0x0C, bitOffset = 1)

	val need_start = Reg(Bool()) init(True)
	busCtrl.readAndWrite(need_start, address = 0x0C, bitOffset = 2)

	val need_stop = Reg(Bool()) init(True)
	busCtrl.readAndWrite(need_stop, address = 0x0C, bitOffset = 3)

	val tx_drive = Reg(Bool()) init(True) 
	busCtrl.readAndWrite(tx_drive, address = 0x0C, bitOffset = 4)

	val tx_ack = Reg(Bool()) init(False) 
	busCtrl.readAndWrite(tx_ack, address = 0x0C, bitOffset = 5)

	val rx_ack = Reg(Bool()) init(False) 
	busCtrl.read(rx_ack, address = 0x0C, bitOffset = 6)

	val clock_counter = Reg(UInt(busWidth bits)) init(0)

	val bit_counter = Reg(UInt(8 bits)) init(0)
	busCtrl.read(bit_counter, address = 0x14)

	val state = Reg(UInt(4 bits)) init(0)
	busCtrl.read(state, address = 0x10)

	val scl = Reg(Bool()) init(False)
	val sda = Reg(Bool()) init(False)
	val tick = Reg(Bool()) init(False)



	when(busCtrl.isReading(0x08) || busCtrl.isWriting(0x08)) { // clock_div
		clock_counter := clock_div
	}

	when(busCtrl.isWriting(0x00)) { // tx_data
		bit_counter := 7
	}

	io.i2c.scl := scl

	when(enable) {

		when(clock_counter === 0) {
			clock_counter := clock_div
			tick := True
		} otherwise {
			clock_counter := clock_counter - 1
			tick := False 
		}

		switch(state) {

			is(0) { // IDLE, do nothing
				when(tick && bit_counter > 0) {
					when(need_start) { // need to transmit start condition ?
						state := 1 // START 
					} otherwise {
						state := 2 // TX_DATA
						scl := False 

						sda := tx_data(7)
						tx_data(16 downto 1) := tx_data(15 downto 0) 
						tx_data(0) := False 

						rx_data(16 downto 1) := rx_data(15 downto 0) 
						rx_data(0) := io.i2c.sda

						when(bit_counter === 0) {
							state := 3 // ACK
						} otherwise {
							bit_counter := bit_counter - 1
						}
					}
					tx_busy := True 
					when(tx_drive) { io.i2c.sda := sda }  
				} 

				io.interrupt := False 

                                /*
				assert(False, "I2C IDLE: tx_data, bit_counter", NOTE);
				assert(False, List(tx_data), NOTE);
				assert(False, List(bit_counter), NOTE);
                                */
			}

			is(1) { // START
				when(tick) {
					sda := False
					scl := True
					state := 2 // TX_DATA 
				}	
				when(tx_drive) { io.i2c.sda := sda } 

                                /*
				assert(False, "I2C START: tx_data, bit_counter", NOTE);
				assert(False, List(tx_data, bit_counter), NOTE);
                                */
			}

			is(2) { // TX_DATA 
				when(tick) {
					when(scl === True) {
					//when(scl === False) {

						sda := tx_data(7)
						tx_data(16 downto 1) := tx_data(15 downto 0) 
						tx_data(0) := False 

						rx_data(16 downto 1) := rx_data(15 downto 0) 
						rx_data(0) := io.i2c.sda

						when(bit_counter === 0) {
							state := 3 // ACK
						} otherwise {
							bit_counter := bit_counter - 1
						}
					}
					scl := ~scl
				}
				when(tx_drive) { io.i2c.sda := sda } 

				assert(False, "I2C TX_DATA: tx_data, bit_counter", NOTE);
				assert(False, List(tx_data), NOTE);
				assert(False, List(bit_counter), NOTE);
			}

			is(3) { // ACK
				when(tick) {
					when(scl === True) {
					//when(scl === False) {
						rx_data(7 downto 1) := rx_data(6 downto 0) 
						rx_data(0) := io.i2c.sda

						sda := True // just in case... 
						state := 4 // STOP1 
						//io.i2c.sda := sda 
					} otherwise {
						//io.i2c.sda := sda 
					}


					scl := ~scl
				}
				when(tx_drive) { io.i2c.sda := sda } 

                                /*
				assert(False, "I2C ACK: tx_data, bit_counter", NOTE);
				assert(False, List(tx_data), NOTE);
				assert(False, List(bit_counter), NOTE);
                                */
			}

			is(4) { // STOP1 
				when(tick) {
					when(scl === False) {
						state := 5 // STOP2
					}
					scl := ~scl
				}

				when(tx_ack) {
					sda := False // transmit ACK 
					io.i2c.sda := sda 
				}

				//io.i2c.sda := sda 
                                
                                /*
				assert(False, "I2C STOP1: tx_data, bit_counter", NOTE);
				assert(False, List(tx_data), NOTE);
				assert(False, List(bit_counter), NOTE);
                                */
			}

			is(5) { // STOP2 
				when(tick) {
					//when(scl === False) {
					when(scl === True) {
	
						when(need_stop) {
							sda := False 
							//sda := True
							scl := False 
							//scl := True 
							io.i2c.sda := sda 
							state := 6 // STOP3 
						} otherwise {
							scl := False
							state := 0 // IDLE
						}

						tx_busy := False
						rx_ack := io.i2c.sda
						io.interrupt := True
					} otherwise {
						scl := ~scl
					}

				}

				when(tx_ack) { io.i2c.sda := sda }

                                /*
				assert(False, "I2C STOP2: tx_data, bit_counter", NOTE);
				assert(False, List(tx_data), NOTE);
				assert(False, List(bit_counter), NOTE);
                                */
			}

			is(6) { // STOP3 
				when(tick) {
					state := 0 // IDLE
					sda := False 
					//sda := True 
					scl := True 
				}
				io.i2c.sda := sda 
			}

		}

	} otherwise {
		state := 0
		scl := True
		sda := True
		tx_busy := False
		io.interrupt := False 
	}

}

