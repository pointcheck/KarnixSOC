package mylib 

import spinal.core._
import spinal.lib._
import spinal.lib.Counter
import spinal.lib.bus.amba3.apb.{Apb3, Apb3Config, Apb3SlaveFactory}

case class HubSPICmd(data_size: Int = 8) extends Bundle{
	val data = Bits(data_size bits)
}

class Hub12SPI(data_size: Int = 8) extends Component {
	val io = new Bundle {
		val clk = out Bool()
		val data = out Bool()
		val cmd = slave Stream(HubSPICmd(data_size = 8))
	}

	val state = Reg(UInt(3 bits)) init(0)
	val mask = Reg(Bits(data_size bits)) init(0)
	val clk_div = Reg(Bool) init(False)


	io.clk := False
	io.data := False
	io.cmd.ready := False

        clk_div := RegNext(~clk_div)
        
        /*
	assert(False, "HUB12SPI mask:", NOTE)
	assert(False, List(io.cmd.valid), NOTE)
	assert(False, List(mask), NOTE)
	assert(False, List(io.cmd.payload.data), NOTE)
        */

	when(io.cmd.valid && clk_div) {
	//when(io.cmd.valid) {

		switch(state) {

			is(0) {
				mask := (1 << (data_size-1))
				state := 1
			}

			is(1) {
				io.data.setWhen((io.cmd.payload.data & mask).asUInt > 0)
				io.clk := True
				state := 2
			}

			is(2) {
				io.data.setWhen((io.cmd.payload.data & mask).asUInt > 0)
				io.clk := True
				state := 3
			}
	
			is(3) {
				io.data.setWhen((io.cmd.payload.data & mask).asUInt > 0)
				io.clk := False 
				state := 4
			}
	
			is(4) {
				io.data.setWhen((io.cmd.payload.data & mask).asUInt > 0)
				io.clk := False
				
				when(mask === 1) {
					state := 5 // END
				} otherwise {
					state := 1
					mask := mask >> U(1)
				}
			}

			is(5) {
				state := 0
				io.cmd.ready := True
			}

			default {
				state := 0
			}
		}
	}

} 


class Hub75SPI() extends Component {
	val io = new Bundle {
		val clk = out Bool()
		val r1 = out Bool()
		val r2 = out Bool()
		val g1 = out Bool()
		val g2 = out Bool()
		val b1 = out Bool()
		val b2 = out Bool()
		val phase = in UInt(2 bits) 
		val cmd = slave Stream(HubSPICmd(data_size = 64))
	}

	val state = Reg(UInt(2 bits)) init(0)
	val counter = Reg(UInt(2 bits)) init(0)
	val data = Reg(Bits(64 bits)) init(0)
	val clk_div = Reg(Bool) init(False)

	io.clk := False
	io.r1 := False
	io.r2 := False
	io.g1 := False
	io.g2 := False
	io.b1 := False
	io.b2 := False
	io.cmd.ready := False

        clk_div := RegNext(~clk_div)

	//assert(False, "HUB75SPI mask:", NOTE)
	//assert(False, List(io.cmd.valid), NOTE)
	//assert(False, List(mask), NOTE)
	//assert(False, List(io.cmd.payload.data), NOTE)

	val r1 = UInt(2 bits)
	val r2 = UInt(2 bits)
	val g1 = UInt(2 bits)
	val g2 = UInt(2 bits)
	val b1 = UInt(2 bits)
	val b2 = UInt(2 bits)

	// Assign aliases to upper plane bits
	r1 := data(1+32 downto 0+32).asUInt
	g1 := data(3+32 downto 2+32).asUInt
	b1 := data(5+32 downto 4+32).asUInt

	// Assign aliases to lower plane bits
	r2 := data(1 downto 0).asUInt
	g2 := data(3 downto 2).asUInt
	b2 := data(5 downto 4).asUInt

	when(io.cmd.valid && clk_div) {
	//when(io.cmd.valid) {

		switch(state) {

			is(0) {
				counter := 0
				data := io.cmd.payload.data
				state := 1
			}

			is(1) {
				// Upper plane
				io.r1 := (r1 > io.phase) || (r1 === 3)
				io.g1 := (g1 > io.phase) || (g1 === 3)
				io.b1 := (b1 > io.phase) || (b1 === 3)

				// Lower plane
				io.r2 := (r2 > io.phase) || (r2 === 3)
				io.g2 := (g2 > io.phase) || (g2 === 3)
				io.b2 := (b2 > io.phase) || (b2 === 3)

				io.clk := True
				state := 2
			}

			is(2) {
				// Upper plane
				io.r1 := (r1 > io.phase) || (r1 === 3)
				io.g1 := (g1 > io.phase) || (g1 === 3)
				io.b1 := (b1 > io.phase) || (b1 === 3)

				// Lower plane
				io.r2 := (r2 > io.phase) || (r2 === 3)
				io.g2 := (g2 > io.phase) || (g2 === 3)
				io.b2 := (b2 > io.phase) || (b2 === 3)

				io.clk := False

				when(counter === 3) {
					state := 3
				} otherwise {
					data := (data >> U(8)).resized 
					counter := counter + 1
					state := 1
				}
			}
	
			is(3) {
				state := 0
				io.cmd.ready := True
			}

		}
	}
} 


class Hub12Transfer(max_planes: Int = 8) extends Component {
	val io = new Bundle {
		val planeOffsets = in Vec(UInt(32 bits), max_planes)
		val num_of_planes = in UInt(8 bits) 
		val col = in UInt(8 bits) 
		val row = in UInt(8 bits) 
		val row_offset = in UInt(16 bits) 
		val fb_data = in UInt(32 bits)
		val fb_addr = out(Reg(UInt(32 bits))) 
		val fb_valid = out Bool()
		val valid = in Bool()
		val ready = out(Reg(Bool()))
		val clk = out Bool()
		val dr = out Bool()
		val phase = in UInt(8 bits) 
	}

	val spi = new Hub12SPI()
	spi.io.cmd.payload.data := 0 
	spi.io.cmd.valid := False
	io.clk := spi.io.clk
	io.dr := spi.io.data

	val state = Reg(UInt(3 bits)) init(0)
	val plane = Reg(UInt(8 bits))
	

	io.ready := False

	io.fb_valid := False

	when(io.valid) {
		switch(state) {

			is(0) { // Begin
				plane := io.num_of_planes - 1
				state := 1
			}

			is(1) { // Load plane offset address into fb_addr
				io.fb_addr := io.planeOffsets(plane.resized) + io.row_offset
				state := 2
			}	

			is(2) {
				// Once fb_addr is settled, initiate mem read
				io.fb_valid := True
				state := 3
			}

			is(3) {
				// Wait for fb_data to settle
				io.fb_valid := True
				state := 6
			}

			is(6) {
				// Wait more
				io.fb_valid := True
				state := 4
			}

			is(4) {
				// fb_data now has 32 bits of image data

				// Transfer one byte over Hub12SPI
				spi.io.cmd.valid := True
				io.fb_valid := False


				switch(io.col & 0x03) {

					is(0) {
						spi.io.cmd.payload.data := ~io.fb_data.asBits(7 downto 0)
					}

					is(1) {
						spi.io.cmd.payload.data := ~io.fb_data.asBits(15 downto 8)
					}

					is(2) {
						spi.io.cmd.payload.data := ~io.fb_data.asBits(23 downto 16)
					}

					is(3) {
						spi.io.cmd.payload.data := ~io.fb_data.asBits(31 downto 24)
					}

				}

				//spi.io.cmd.payload.data := ~io.row_offset.asBits(7 downto 0)
				//spi.io.cmd.payload.data := ~io.row.asBits(7 downto 0)

				when(spi.io.cmd.ready) {
					state := 5 
				}
			}

			is(5) { // Iterate through planes in reverse order

				when(plane === 0) {
					state := 0
					io.ready := True
				} otherwise {
					plane := plane - 1
					state := 1 
				}
                                /*
				assert(False, "HUB12 plane/col/row_offset/fb_addr/fb_data:", NOTE)
				assert(False, List(plane), NOTE)
				assert(False, List(io.col), NOTE)
				assert(False, List(io.row_offset), NOTE)
				assert(False, List(io.fb_addr), NOTE)
				assert(False, List(io.fb_data), NOTE)
                                */
			}
		}

	} otherwise {
		state := 0
	}


}

class Hub75Transfer(max_planes: Int = 8) extends Component {
	val io = new Bundle {
		val planeOffsets = in Vec(UInt(32 bits), max_planes)
		val num_of_planes = in UInt(8 bits) 
		val col = in UInt(8 bits) 
		val row = in UInt(8 bits) 
		val row_offset = in UInt(16 bits) 
		val fb_data = in UInt(32 bits)
		val fb_addr = out(Reg(UInt(32 bits))) 
		val fb_valid = out Bool()
		val valid = in Bool()
		val ready = out(Reg(Bool()))
		val clk = out Bool()
		val r1 = out Bool()
		val r2 = out Bool()
		val g1 = out Bool()
		val g2 = out Bool()
		val b1 = out Bool()
		val b2 = out Bool()
		val phase = in UInt(2 bits)
	}

	val spi = new Hub75SPI()
	spi.io.cmd.payload.data := 0 
	spi.io.cmd.valid := False
	spi.io.phase := io.phase
	io.clk := spi.io.clk
	io.r1 := spi.io.r1
	io.r2 := spi.io.r2
	io.g1 := spi.io.g1
	io.g2 := spi.io.g2
	io.b1 := spi.io.b1
	io.b2 := spi.io.b2

	val state = Reg(UInt(4 bits)) init(0)
	val plane = Reg(UInt(8 bits))
	val data1 = Reg(UInt(32 bits))
	val data2 = Reg(UInt(32 bits))
	

	io.ready := False

	io.fb_valid := False

	when(io.valid) {
		switch(state) {

			is(0) { 
				// Load upper plane offset address into fb_addr
				io.fb_addr := io.planeOffsets(0) + io.row_offset
				state := 1
			}	

			is(1) {
				// Once fb_addr is settled, initiate mem read
				io.fb_valid := True
				state := 2
			}

			is(2) {
				// Wait for fb_data to settle
				io.fb_valid := True
				state := 5 
			}

			is(5) {
				// Save upper plane data into data1 register
				io.fb_valid := True
				data1 := io.fb_data

				// Load lower plane offset address into fb_addr
				io.fb_addr := io.planeOffsets(1) + io.row_offset
				state := 6
			}

			is(6) {
				// Once fb_addr is settled, initiate mem read
				io.fb_valid := True
				state := 7
			}

			is(7) {
				// Wait for fb_data to settle
				io.fb_valid := True
				state := 10 
			}

			is(10) {
				// Save upper plane data into data1 register
				io.fb_valid := True
				data2 := io.fb_data

				state := 11 
			}


			is(11) {
				// Both data1 and data2 now loaded with image data

				// Transfer four byte over Hub75SPI
				spi.io.cmd.valid := True
				io.fb_valid := False

				spi.io.cmd.payload.data := (data1.asBits << 32) | data2.resize(64).asBits

				//spi.io.cmd.payload.data :=  (io.row.resize(32).asBits << 32) | io.row.resize(64).asBits

				when(spi.io.cmd.ready) {
					state := 0 
					io.ready := True

                                        /*
					assert(False, "HUB75 col/row_offset/data64/planeOffsets(0)/planeOffsets(1):", NOTE)
					assert(False, List(io.col), NOTE)
					assert(False, List(io.row_offset), NOTE)
					assert(False, List(spi.io.cmd.payload.data), NOTE)
					assert(False, List(io.planeOffsets(0)), NOTE)
					assert(False, List(io.planeOffsets(1)), NOTE)
                                        */
				}

			}

		}

	} otherwise {
		state := 0
	}


}



class Apb3HubCtrl(max_width: Int = 80, max_height: Int = 40, color_bits: Int = 8, max_planes: Int = 8) extends Component {
	val io = new Bundle {
		val apb = slave( Apb3( addressWidth = 16, dataWidth = 32))
		val output = out Bits (16 bits)
	}

	// Define Control word and connect it to APB3 bus
	val busCtrl = Apb3SlaveFactory(io.apb)
	val hubCtrlWord = Bits(32 bits)
	busCtrl.driveAndRead(hubCtrlWord, address = 16*1024)


	// Define plane offset vector and connect to APB3 bus
	val planeOffsets = Reg(Vec(UInt(32 bits), max_planes))
	busCtrl.writeMultiWord(planeOffsets, address = 16*1024+4)
	busCtrl.readMultiWord(planeOffsets, address = 16*1024+4)

	// Define memory block for LED framebuffer

	val fb_mem = Mem(Bits(32 bits), wordCount = max_width * max_height * color_bits / 32)
	//val fb_mem = Mem(Bits(32 bits), 12*1024*8/32 )

/*

	when( io.apb.PENABLE && io.apb.PSEL(0) && io.apb.PADDR < 8*1024) {
		when(io.apb.PWRITE) {
			fb_mem(io.apb.PADDR.resized) := io.apb.PWDATA.resized
		} otherwise {
			io.apb.PRDATA := fb_mem(io.apb.PADDR.resized).resized 
		}
		io.apb.PREADY := True
  	}
*/
	// Connect framebuffer to APB3 bus

	when( io.apb.PENABLE && io.apb.PSEL(0) && io.apb.PADDR < 8*1024) {
		when(io.apb.PWRITE) {
			fb_mem((io.apb.PADDR >> 2).resized) := io.apb.PWDATA.resized
		} otherwise {
			io.apb.PRDATA := fb_mem((io.apb.PADDR >> 2).resized).resized 
		}
		io.apb.PREADY := True
  	}


	// Split Control word into a number of fields for being able to access each separately
	val hub_type =  UInt (7 bits)
	val enabled = Bool
	val brightness = UInt (8 bits)
	val matrix_width = UInt (8 bits)
	val num_of_planes = UInt (8 bits)

	hub_type := hubCtrlWord(6 downto 0).asUInt
	enabled := hubCtrlWord(7)
	brightness := hubCtrlWord(15 downto 8).asUInt
	matrix_width := hubCtrlWord(23 downto 16).asUInt
	num_of_planes := hubCtrlWord(31 downto 24).asUInt
  


	// HUB local state registers 
	val plane = Reg(UInt(log2Up(max_planes) bits)) init(0)
	val col = Reg(UInt(8 bits)) init(0)
	val row = Reg(UInt(8 bits)) init(0)
	val row_offset = Reg(UInt(16 bits)) init(0)
	val state = Reg(Bits(3 bits)) init(0)
	val fb_addr = UInt(32 bits)
	val fb_data = Reg(UInt(32 bits)) init(0)
	val fb_valid = False
	val phase = Reg(UInt(8 bits)) init(0) // luminosity phase


	fb_addr := 0

	when(fb_valid) {
		fb_data := fb_mem.readSync((fb_addr >> 2).resized).asUInt
	}
	
	// Give names to output signals and their default values
	val hub_a = False
	val hub_b = False
	val hub_c = False
	val hub_d = False
	val hub_e = False
	val hub_dr = False
	val hub_r1 = False
	val hub_r2 = False
	val hub_g1 = False
	val hub_g2 = False
	val hub_b1 = False
	val hub_b2 = False
	val hub_lat = False
	val hub_clk = False
	val hub_oe = False
	val rows_per_plane = UInt(8 bits)


	// Map signals to output lines depending on interface type

	io.output := 0

	when(hub_type === 75) { // HUB75 matrix is connected
		hub_a := (row - 2)(0) // bit 0 of (row - 1) 
		hub_b := (row - 2)(1) // bit 1 of (row - 1)
		hub_c := (row - 2)(2) // ...
		hub_d := (row - 2)(3) 
		hub_e := (row - 2)(4) 
		rows_per_plane := 20 
		io.output(7) := hub_e
		io.output(8) := hub_a
		io.output(9) := hub_b
		io.output(10) := hub_c
		io.output(11) := hub_d
		io.output(14) := ~hub_oe // OE signal is Active Low
		io.output(13) := hub_lat
		io.output(12) := hub_clk
		io.output(0) := hub_r1
		io.output(1) := hub_g1
		io.output(2) := hub_b1
		io.output(4) := hub_r2
		io.output(5) := hub_g2
		io.output(6) := hub_b2
		
	} otherwise { // Default interface type is HUB12 matrix
		hub_a := (row - 1)(0) // bit 0 of (row - 1) 
		hub_b := (row - 1)(1) // bit 1 of (row - 1)
		hub_c := (row - 1)(2) // ...
		hub_d := (row - 1)(3) 
		hub_e := (row - 1)(4) 
		rows_per_plane := 4
		io.output(0) := hub_oe
		io.output(1) := hub_a
		io.output(3) := hub_b
		io.output(7) := hub_clk
		io.output(9) := hub_lat
		io.output(11) := hub_dr
	}



	val delay = Counter(32 bits)


	// Instantiate HUB12 processing unit, define its inputs
	val hub12_transfer = new Hub12Transfer()
	hub12_transfer.io.valid := False
	hub12_transfer.io.planeOffsets := planeOffsets
	hub12_transfer.io.num_of_planes := num_of_planes
	hub12_transfer.io.row_offset := row_offset 
	hub12_transfer.io.col := col 
	hub12_transfer.io.row := row 
	hub12_transfer.io.fb_data := fb_data 
	hub12_transfer.io.phase := phase.resize(8)

	// Instantiate HUB75 processing unit, define its inputs
	val hub75_transfer = new Hub75Transfer()
	hub75_transfer.io.valid := False
	hub75_transfer.io.planeOffsets := planeOffsets
	hub75_transfer.io.num_of_planes := num_of_planes
	hub75_transfer.io.row_offset := row_offset 
	hub75_transfer.io.col := col 
	hub75_transfer.io.row := row 
	hub75_transfer.io.fb_data := fb_data 
	hub75_transfer.io.phase := phase.resize(2)


	// When HUB12 unit is valid, connect fb_addr to HUB12 unit, connect its output to HUB interface
	when(hub12_transfer.io.valid) { 
		fb_addr := hub12_transfer.io.fb_addr 
		fb_valid := hub12_transfer.io.fb_valid 
		hub_clk := hub12_transfer.io.clk
		hub_dr := hub12_transfer.io.dr
	} 

	// When HUB75 unit is valid, connect fb_addr to HUB75 unit, connect its output to HUB interface
	when(hub75_transfer.io.valid) {
		fb_addr := hub75_transfer.io.fb_addr 
		fb_valid := hub75_transfer.io.fb_valid 
		hub_clk := hub75_transfer.io.clk
		hub_r1 := hub75_transfer.io.r1
		hub_r2 := hub75_transfer.io.r2
		hub_g1 := hub75_transfer.io.g1
		hub_g2 := hub75_transfer.io.g2
		hub_b1 := hub75_transfer.io.b1
		hub_b2 := hub75_transfer.io.b2
	}


	when(enabled) {

		switch(hub_type) {

			is(75) { // HUB75 interface

				switch(state) {

					is(0) {
						// Turn OFF LAT
						//hub_lat := False 

						// Turn ON the lights
						hub_oe := True

						// Delay for some time
						when(delay.value >= (brightness << 3)) {
							delay.clear() 

							// Start matrix update 
							col := 0
							state := 1 
							hub_oe := False 

						} otherwise {
							delay.increment()
						}

					}

					is(1) { // Send four bytes of a row
						hub75_transfer.io.valid := True

						// Sending complete, choose next byte in a row 
						when(hub75_transfer.io.ready) {
							hub75_transfer.io.valid := False 
							col := col + 4
							state := 2 // Check for end of row
						}

					}

					is(2) {
						// increment offset every 4 bytes
						when((col & 0x03) === 0) {
							row_offset := row_offset + 4
						}

						// Check for end of row 
						when(col === matrix_width) { // col is one byte = 1 pix
							// End of row, calc new row address

                                                        /*
							assert(False, "HUB75: EoR, rows_per_plane/row", NOTE)
							assert(False, List(rows_per_plane), NOTE)
							assert(False, List(row), NOTE)
                                                        */

							when(row === rows_per_plane + 2) {
								row := 0
								row_offset := 0
							} otherwise {
								row := row + 1
							}

							state := 3

						} otherwise {
							state := 1 // Send four bytes of a row
						}
					}

					is(3) {
						// Flip LAT ON for a while 
						hub_lat := True 

						// Delay to let new row address to settle 
						when(delay.value === ((255 - brightness) << 3) + 1) {
							delay.clear() 
							state := 0
						} otherwise {
							delay.increment()
						}
					}

				} // switch(state) 

			} // HUB75 

			default { // HUB12 interface

				switch(state) {

					is(0) {
						// Turn ON the lights
						hub_oe := True

						// Delay for some time
						when(delay.value >= (brightness << 4)) {
							delay.clear() 

							// Start matrix update 
							col := 0
							state := 1 
							hub_oe := False 

						} otherwise {
							delay.increment()
						}

					}

					is(1) { // Send one byte of a row
						hub12_transfer.io.valid := True


						// Sending complete, choose next byte in a row 
						when(hub12_transfer.io.ready) {
							hub12_transfer.io.valid := False 
							col := col + 1
							state := 2 // Check for end of row
						}

					}

					is(2) {
						// increment offset every 4 bytes
						when((col & 0x03) === 0) {
							row_offset := row_offset + 4
						}

						// Check for end of row 
						when(col === (matrix_width >> U(3)) ) { // col is one byte = 8 pix
							// End of row, calc new row address

                                                        /*
							assert(False, "HUB12: EoR, rows_per_plane/row", NOTE)
							assert(False, List(rows_per_plane), NOTE)
							assert(False, List(row), NOTE)
                                                        */

							when(row === (rows_per_plane - 1)) {
								row := 0
								row_offset := 0
							} otherwise {
								row := row + 1
							}

							state := 3

						} otherwise {
							state := 1 // Send one byte of a row
						}


					}

					is(3) {
						// Flip LAT pin ON to update row address in driver
						hub_lat := True 

						// Delay to let new row address to settle 
						when(delay.value === ((255 - brightness) << 4) + 31) {
							delay.clear() 
							state := 0
						} otherwise {
							delay.increment()
						}
					}

				} // switch(state) 

			} // HUB12

		} // switch(hub_type) 

		phase := phase + 1

	} otherwise {

		// HUB is disabled, reset to zero state
		io.output := 0
		row := 0
		row_offset := 0
		state := 0
		phase := 0
	}
}

