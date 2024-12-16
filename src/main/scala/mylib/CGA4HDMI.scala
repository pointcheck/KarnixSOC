package mylib 

import spinal.core._
import spinal.lib._
import spinal.lib.Counter
import spinal.lib.bus.amba3.apb.{Apb3, Apb3Config, Apb3SlaveFactory}
import spinal.lib.misc.HexTools
import mylib._

case class Apb3CGA4HDMICtrl(
	horiz_back_porch: Int = 32,
	horiz_active: Int = 640,
	horiz_front_porch: Int = 32,
	horiz_sync: Int = 96,
	vert_back_porch: Int = 16,
	vert_active: Int = 480,
	vert_front_porch: Int = 27,
	vert_sync: Int = 2,
	charGenHexFile: String = "font8x16x256.hex"
      ) extends Component {
  val io = new Bundle {
    val apb              = slave(Apb3(addressWidth = 16, dataWidth = 32))
    val hdmi             = master(HDMIInterface())
    val pixclk_x10       = in Bool() 
    val vblank_interrupt = out Bool()
  }

  val busCtrl = Apb3SlaveFactory(io.apb)

  // Define Control word and connect it to APB3 bus
  /*
   * cgaCtrlWord: 31    - video enable
   *              27-24 - video mode
   *              23    - HSYNC INT enable
   *              22    - VSYNC INT enable
   *              21    - HSYNC status
   *              20    - VSYNC status
   *              19    - VBLANK status
   */

  val cgaCtrlWord = busCtrl.createReadAndWrite(Bits(32 bits), address = 48*1024+64) init(B"32'x80000000")
  val video_enabled = cgaCtrlWord(31).addTag(crossClockDomain) 
  val blanking_enabled = cgaCtrlWord(30).addTag(crossClockDomain) 
  val video_mode = cgaCtrlWord(25 downto 24).addTag(crossClockDomain)
  val scroll_v_dir = cgaCtrlWord(10).addTag(crossClockDomain) 
  val scroll_v = cgaCtrlWord(9 downto 0).asUInt.addTag(crossClockDomain) 

  val cgaCtrl2Word = busCtrl.createReadAndWrite(Bits(32 bits), address = 48*1024+68) init(B"32'xfc0f0000")
  val cursor_x = cgaCtrl2Word(6 downto 0).asUInt.addTag(crossClockDomain)
  val cursor_y = cgaCtrl2Word(13 downto 8).asUInt.addTag(crossClockDomain)
  val cursor_bottom = cgaCtrl2Word(19 downto 16).asUInt.addTag(crossClockDomain)
  val cursor_top = cgaCtrl2Word(23 downto 20).asUInt.addTag(crossClockDomain)
  val cursor_blink = cgaCtrl2Word(26 downto 24).asUInt.addTag(crossClockDomain)
  val cursor_blink_enabled = cgaCtrl2Word(27).addTag(crossClockDomain)
  val cursor_color = cgaCtrl2Word(31 downto 28).asUInt.addTag(crossClockDomain)

  // Define memory block for CGA framebuffer
  //val fb_mem = Mem(Bits(32 bits), wordCount = (320*240*2) / 32)
  val fb_mem = Mem(Bits(32 bits), wordCount = (320*(240+16)*2) / 32)
  // Connect framebuffer to APB3 bus
  val fb_access = io.apb.PENABLE && io.apb.PSEL(0) && io.apb.PADDR < 48*1024
  when(fb_access) {
    io.apb.PRDATA := fb_mem.readWriteSync(
        address = (io.apb.PADDR >> 2).resized,
        data  = io.apb.PWDATA.resized,
        enable  = fb_access,
        write  = io.apb.PWRITE,
        mask  = 3 
    )
    io.apb.PREADY := RegNext(fb_access)
  }


  // Define memory block for character generator
  val chargen_mem = Mem(Bits(8 bits), wordCount = 16*256 ) // font8x16 x 256
  HexTools.initRam(chargen_mem, charGenHexFile, 0x0l)

  /*
  // Sync edition of CharGen - uses BRAM
  var chargen_access = io.apb.PENABLE && io.apb.PSEL(0) && 
                  ((io.apb.PADDR & U"xf000") === U"xf000") // 60 * 1024
  when(chargen_access) {
    io.apb.PRDATA := fb_mem.readWriteSync(
        address = (io.apb.PADDR >> 2).resized,
        data  = io.apb.PWDATA.resized,
        enable  = chargen_access,
        write  = io.apb.PWRITE,
        mask  = 3 
    )
    io.apb.PREADY := chargen_access
  }
  */

 /*
  // Async edition of CharGen - uses too much COMBs and FFs 
  when( io.apb.PENABLE && io.apb.PSEL(0) && ((io.apb.PADDR & U"xf000") === U"xf000")) { // 61440
    when(io.apb.PWRITE) {
      chargen_mem((io.apb.PADDR & 0x0fff).resized) := io.apb.PWDATA(7 downto 0)
    } otherwise {
      io.apb.PRDATA := chargen_mem((io.apb.PADDR & 0x0fff).resized).resized
    }
    io.apb.PREADY := True
  }
  */
  

  // Connect palette to APB3 bus
  //
  val palette_mem = Mem(Bits(32 bits), wordCount = 16) // Pallete registers buffered
  /*
  when( io.apb.PENABLE && io.apb.PSEL(0) && ((io.apb.PADDR & U"xffc0") === U"xc000")) { // offset 49152
    when(io.apb.PWRITE) {
      palette_mem((io.apb.PADDR >> 2)(3 downto 0)) := io.apb.PWDATA(31 downto 0)
    } otherwise {
      io.apb.PRDATA := palette_mem((io.apb.PADDR >> 2)(3 downto 0)).resized
    }
    io.apb.PREADY := True
  }
  */
  val palette_access = io.apb.PENABLE && io.apb.PSEL(0) && ((io.apb.PADDR & U"xffc0") === U"xc000") // 49152 
  when(palette_access) {
    io.apb.PRDATA := palette_mem.readWriteSync(
        address = (io.apb.PADDR >> 2).resized,
        data  = io.apb.PWDATA.resized,
        enable  = palette_access,
        write  = io.apb.PWRITE,
        mask  = 3 
    )
    io.apb.PREADY := RegNext(palette_access)
  }

  val pixclk_in = Bool() /* Artificially synthesized clock */
  val pixclk = Bool() /* Artificially synthesized clock globally routed (ECP5 specific) */
  val pixclk_x10 = Bool() /* x10 multiplied clock */

  /* Route artificial TMDS clock using global lines, i.e. DCCA (ECP5 specific) */
  val dcca = new DCCA()
  dcca.CLKI := pixclk_in
  dcca.CE := True
  pixclk := dcca.CLKO

  val dviClockDomain = ClockDomain(
    clock = pixclk,
    config = ClockDomainConfig(resetKind = BOOT),
    frequency = FixedFrequency(25.0 MHz)
  )

  val tmdsClockDomain = ClockDomain(
    clock = io.pixclk_x10,
    config = ClockDomainConfig(resetKind = BOOT),
    frequency = FixedFrequency(250.0 MHz)
  )

  val dvi_area = new ClockingArea(dviClockDomain) {

    /* Define RGB regs, HV and DE signals */
    val red = Bits(8 bits)
    val green = Bits(8 bits)
    val blue = Bits(8 bits)
    val hSync = Bool()
    val vSync = Bool()
    val vBlank = Bool()
    val de = Bool()

    /* Generate counters */

    /* Convenience params */
    val horiz_total_width = horiz_back_porch + horiz_active + horiz_front_porch + horiz_sync
    val vert_total_height = vert_back_porch + vert_active + vert_front_porch + vert_sync

    /* Set of counters */
    val CounterX = Reg(UInt(log2Up(horiz_total_width) bits))
    val CounterY = Reg(UInt(log2Up(vert_total_height) bits))
    //val CounterY_ = (scroll_v_dir ? (CounterY - scroll_v) | (CounterY + scroll_v))
    val CounterY_ = (scroll_v_dir ? (CounterY - BufferCC(scroll_v)) | (CounterY + BufferCC(scroll_v)))
    val CounterF = Reg(UInt(7 bits)) // Frame counter, used for cursor blink feature


    CounterX := (CounterX === horiz_total_width - 1) ? U(0) | CounterX + 1

    when(CounterX === horiz_total_width - 1) {
        CounterY := ((CounterY === vert_total_height - 1) ? U(0) | CounterY + 1)
    }

    when(CounterX === 0 && CounterY === 0) {
      CounterF := CounterF + 1
    }


    /* Produce HSYNC, VSYNC and DE based on back/front porches */
    hSync := (CounterX >= horiz_back_porch + horiz_active + horiz_front_porch) &&
             (CounterX < horiz_back_porch + horiz_active + horiz_front_porch + horiz_sync)

    vSync := (CounterY >= vert_back_porch + vert_active + vert_front_porch) &&
             (CounterY < vert_back_porch + vert_active + vert_front_porch + vert_sync)

    de := (CounterX >= horiz_back_porch && CounterX < horiz_back_porch + horiz_active) &&
          (CounterY >= vert_back_porch && CounterY < vert_back_porch + vert_active)

    vBlank := (CounterY < vert_back_porch) || (CounterY >= vert_active + vert_back_porch)


    /* Generate display picture */

    val blanking_not_enabled = !blanking_enabled
    val word_load = Bool()
    val word = Reg(Bits(32 bits))
    val word_address = UInt(13 bits)

    word_load := False
    word_address := 0

    // 32 bit word of current character/PEL data
    word := fb_mem.readSync(address = word_address, enable = word_load, clockCrossing = true)
    
    switch(video_mode) {

      is(B"00") { // Text mode: 80x30 characters each 8x16 pixels

        // Load flag active on each 6th and 7th pixel
        word_load := (CounterX >= U(horiz_back_porch - 8) && CounterX < U(horiz_back_porch + horiz_active)) &&
                     (CounterY < vert_back_porch + vert_active + 16) && ((CounterX & U(6)) === U(6))

        // Index of the 32 bit word in framebuffer memory: addr = (y / 16) * 80 + x/8
        word_address := ((CounterY_ - 16)(9 downto 4) * 80 +
                         (CounterX - (horiz_back_porch - 8))(9 downto 3)).resized

        val char_row = CounterY_(3 downto 0)
        val char_mask = Reg(Bits(8 bits))
          
        val char_idx = word(7 downto 0)
        val char_fg_color = word(11 downto 8).asUInt
        val char_bg_color = word(19 downto 16).asUInt

        when((CounterX & 7) === 7) {
          char_mask := B"10000000"
        } otherwise {
          char_mask := B"0" ## char_mask(7 downto 1)
        }

        val char_data = chargen_mem((char_idx ## char_row).asUInt).asBits

        when(de && blanking_not_enabled) {

          var x = (CounterX - horiz_back_porch)(9 downto 3) // current ray column position
          var y = (CounterY_ - vert_back_porch)(9 downto 4) // current ray raw position
          var blink_flag = CounterF(cursor_blink) && cursor_blink_enabled &&
                           char_row >= cursor_top && char_row <= cursor_bottom

          when(x === cursor_x && y === cursor_y && blink_flag) {
            // Implement blinking cursor
            red := palette_mem(cursor_color).asBits(7 downto 0)
            green := palette_mem(cursor_color).asBits(15 downto 8)
            blue := palette_mem(cursor_color).asBits(23 downto 16)
          } elsewhen((char_data & char_mask) =/= 0) {
            // Draw character
            red := palette_mem(char_fg_color).asBits(7 downto 0)
            green := palette_mem(char_fg_color).asBits(15 downto 8)
            blue := palette_mem(char_fg_color).asBits(23 downto 16)
          } otherwise {
            // Draw background
            red := palette_mem(char_bg_color).asBits(7 downto 0)
            green := palette_mem(char_bg_color).asBits(15 downto 8)
            blue := palette_mem(char_bg_color).asBits(23 downto 16)
          }

        } otherwise {
          red := 0
          green := 0
          blue := 0 
        }
      }

      is(B"01") { // Graphics mode: 320x240, 2 bits per PEL with full color palette

        // Load flag active on each 30 and 31 pixel of 32 bit word
        word_load := (CounterX >= 0 && CounterX < U(horiz_back_porch + horiz_active)) &&
                     (CounterY_ < vert_back_porch + vert_active) && ((CounterX & U(30)) === U(30))

        // Index of the 32 bit word in framebuffer memory: addr = y/2 * 20 + x/2/16
        word_address := ((CounterY_ - 16)(9 downto 1) * 20 + CounterX(9 downto 5)).resized

        when(de && blanking_not_enabled) {

          val color = UInt(4 bits)

          color := (word << (CounterX(4 downto 1) << 1))(31 downto 30).asUInt.resized

          red := palette_mem(color).asBits(7 downto 0)
          green := palette_mem(color).asBits(15 downto 8)
          blue := palette_mem(color).asBits(23 downto 16)

        } otherwise {
          red := 0
          green := 0
          blue := 0 
        }
      }

      default { // Display red screen for unsupported video modes
          red := 255 
          green := 0
          blue := 0
      }
    }

    /* Do TMDS encoding */

    /* Pass each color reg through external TMDS encoder to get TMDS regs filled */

    val encoder_R = TMDS_encoder()
    encoder_R.clk := pixclk
    encoder_R.VD := red
    encoder_R.CD := B"00"
    encoder_R.VDE := de

    val encoder_G = TMDS_encoder()
    encoder_G.clk := pixclk
    encoder_G.VD := green
    encoder_G.CD := B"00"
    encoder_G.VDE := de

    val encoder_B = TMDS_encoder()
    encoder_B.clk := pixclk
    encoder_B.VD := blue
    encoder_B.CD := vSync ## hSync /* Blue channel carries HSYNC and VSYNC controls */
    encoder_B.VDE := de

    /* Produce TMDS clock differential signal which is PIXCLK, i.e. 25.0 MHz, not 250.0 MHz !!! */
    val tmds_clk = OBUFDS()
    tmds_clk.I := pixclk
    io.hdmi.tmds_clk_p := tmds_clk.O
    io.hdmi.tmds_clk_n := tmds_clk.OB

  }

  cgaCtrlWord(21 downto 19) := BufferCC(dvi_area.hSync ## dvi_area.vSync ## dvi_area.vBlank)
  io.vblank_interrupt := cgaCtrlWord(19) 

  val tmds_area = new ClockingArea(tmdsClockDomain) {

    /* Generate 25 MHz PIXCLK by dividing pixclk_x10 by 10 */

    val clk_div = Reg(UInt(4 bits)) init(0)
    val clk = Reg(Bool())

    clk_div := clk_div + 1

    when(clk_div === 4) {
      clk := True
    }

    when(clk_div === 9) {
      clk := False
      clk_div := 0
    }

    pixclk_in := clk

    /* Produce G, R and B data bits by shifting each TMDS register.
       Use BufferCC() to cross clock domains.  */

    val TMDS_shift_red = Reg(Bits(10 bits)) init(0)
    val TMDS_shift_green = Reg(Bits(10 bits)) init(0)
    val TMDS_shift_blue = Reg(Bits(10 bits)) init(0)
    val TMDS_mod10 = Reg(UInt(4 bits)) init(0)
    val TMDS_shift_load = Reg(Bool()) init(False)

    TMDS_shift_red := TMDS_shift_load ? BufferCC(dvi_area.encoder_R.TMDS) | TMDS_shift_red(9 downto 1).resized
    TMDS_shift_green := TMDS_shift_load ? BufferCC(dvi_area.encoder_G.TMDS) | TMDS_shift_green(9 downto 1).resized
    TMDS_shift_blue := TMDS_shift_load ? BufferCC(dvi_area.encoder_B.TMDS) | TMDS_shift_blue(9 downto 1).resized
    TMDS_mod10 := ((TMDS_mod10 === U(9)) ? U(0) | TMDS_mod10 + 1)
    TMDS_shift_load := TMDS_mod10 === U(9)

    /* Produce differential signals using hard OBUFDS block */

    val tmds_0 = OBUFDS()
    tmds_0.I := TMDS_shift_blue(0)
    io.hdmi.tmds_p(0) := tmds_0.O
    io.hdmi.tmds_n(0) := tmds_0.OB

    val tmds_1 = OBUFDS()
    tmds_1.I := TMDS_shift_green(0)
    io.hdmi.tmds_p(1) := tmds_1.O
    io.hdmi.tmds_n(1) := tmds_1.OB

    val tmds_2 = OBUFDS()
    tmds_2.I := TMDS_shift_red(0)
    io.hdmi.tmds_p(2) := tmds_2.O
    io.hdmi.tmds_n(2) := tmds_2.OB
  }

}

