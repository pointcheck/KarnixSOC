package mylib

import spinal.core._
import spinal.lib._
import spinal.lib.io.TriState

case class HDMIInterface() extends Bundle with IMasterSlave{
	val tmds_p = Bits(3 bits)
	val tmds_n = Bits(3 bits)
	val tmds_clk_p = Bool() 
	val tmds_clk_n = Bool() 

	override def asMaster(): Unit = {
		out(tmds_p, tmds_n, tmds_clk_p, tmds_clk_n)
	}
}


case class TMDS_encoder() extends BlackBox{
	val clk = in Bool()
	val VD = in Bits(8 bits)
	val CD = in Bits(2 bits)
	val VDE = in Bool()
	val TMDS = out Bits(10 bits)
}


case class OBUFDS() extends BlackBox{
	val I = in Bool()
	val O, OB = out Bool()
}


case class DCCA() extends BlackBox{
  val CLKI = in  Bool()
  val CLKO = out  Bool()
  val CE = in  Bool()
}


/*
class TMDSEncoder() extends Component {
        val io = new Bundle {
              val clk = in(Bool())
              val VD = in(Bits(8 bits)) // Video data (red, green, blue)
              val CD = in(Bits(2 bits)) // Control data
              val VDE = in(Bool()) //Video Data Enable, select between CD (VDE=0) and CD (VDE=1)
              val TMDS = out(Reg(Bits(10 bits)) init(0))
        }

        val Nb1s = UInt(4 bits)
        val XNOR = Bool()
        val q_m = UInt(9 bits)

        val balance_acc = Reg(UInt(4 bits)) init(0)
        val balance = UInt(4 bits)
        val balance_sign_eq = Bool()
        val invert_q_m = Bool() 
        val balance_acc_inc = UInt(4 bits)
        val balance_acc_new = UInt(4 bits)
        val TMDS_data = Bits(10 bits)
        val TMDS_code = Bits(10 bits)

        Nb1s := U(io.VD(0)).resize(4) + U(io.VD(1)) + U(io.VD(2)) + U(io.VD(3)) + U(io.VD(4)) + U(io.VD(5)) + U(io.VD(6)) + U(io.VD(7))

        XNOR := (Nb1s > U(4)) || (Nb1s === U(4) && io.VD(0) === False)

        q_m := (~XNOR ## (q_m(6 downto 0).asBits ^ io.VD(7 downto 1) ^ B(XNOR).resize(7)).asBits ## io.VD(0)).asUInt

        balance := U(q_m(0)).resize(4) + U(q_m(1)) + U(q_m(2)) + U(q_m(3)) + U(q_m(4)) + U(q_m(5)) + U(q_m(6)) + U(q_m(7)) - U(4)

        balance_sign_eq := (balance(3) === balance_acc(3))

        invert_q_m := ((balance === U(0) || balance_acc === U(0)) ? ~q_m(8) | balance_sign_eq)

        balance_acc_inc := balance - U(
                                       (q_m(8) ^ ~balance_sign_eq) & ~(balance === U(0) || balance_acc === U(0))
                                      )

        balance_acc_new := invert_q_m ? (balance_acc - balance_acc_inc) | (balance_acc + balance_acc_inc)

        TMDS_data := invert_q_m ## q_m(8) ## (q_m(7 downto 0).asBits ^ invert_q_m.asBits.resize(8))

        TMDS_code := io.CD(1) ? (io.CD(0) ? B"1010101011" | B"0101010100") | (io.CD(0) ? B"0010101011" | B"1101010100")

        io.TMDS := io.VDE ? TMDS_data | TMDS_code

        balance_acc := io.VDE ? balance_acc_new | U(0)
}

*/

/*
module TMDS_encoder(
        input clk,
        input [7:0] io.VD,  // video data (red, green or blue)
        input [1:0] io.CD,  // control data
        input io.VDE,  // video data enable, to choose between io.CD (when io.VDE=0) and io.VD (when io.VDE=1)
        output reg [9:0] TMDS = 0
);

wire [3:0] Nb1s = io.VD[0] + io.VD[1] + io.VD[2] + io.VD[3] + io.VD[4] + io.VD[5] + io.VD[6] + io.VD[7];
wire XNOR = (Nb1s>4'd4) || (Nb1s==4'd4 && io.VD[0]==1'b0);
wire [8:0] q_m = {~XNOR, q_m[6:0] ^ io.VD[7:1] ^ {7{XNOR}}, io.VD[0]};

reg [3:0] balance_acc = 0;
wire [3:0] balance = q_m[0] + q_m[1] + q_m[2] + q_m[3] + q_m[4] + q_m[5] + q_m[6] + q_m[7] - 4'd4;
wire balance_sign_eq = (balance[3] == balance_acc[3]);
wire invert_q_m = (balance==0 || balance_acc==0) ? ~q_m[8] : balance_sign_eq;
wire [3:0] balance_acc_inc = balance - ({q_m[8] ^ ~balance_sign_eq} & ~(balance==0 || balance_acc==0));
wire [3:0] balance_acc_new = invert_q_m ? balance_acc-balance_acc_inc : balance_acc+balance_acc_inc;
wire [9:0] TMDS_data = {invert_q_m, q_m[8], q_m[7:0] ^ {8{invert_q_m}}};
wire [9:0] TMDS_code = io.CD[1] ? (io.CD[0] ? 10'b1010101011 : 10'b0101010100) : (io.CD[0] ? 10'b0010101011 : 10'b1101010100);

always @(posedge clk) TMDS <= io.VDE ? TMDS_data : TMDS_code;
always @(posedge clk) balance_acc <= io.VDE ? balance_acc_new : 4'h0;

endmodule
*/


