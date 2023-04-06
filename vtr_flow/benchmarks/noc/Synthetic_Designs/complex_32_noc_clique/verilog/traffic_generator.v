/* This is the traffic generator module. This
	generate data to be sent over the NoC to the
	traffic processor module*/
	
module traffic_generator(
    clk,
    reset,
    tdata,
    tvalid
);

parameter noc_dw = 32; //NoC Data Width
parameter byte_dw = 8; 
parameter filter_dw = 18;
parameter acc_const = 4;

/*****************INPUT/OUTPUT Definition********************/
input wire clk;
input wire reset;

output wire [noc_dw - 1 : 0] tdata;
output wire tvalid;

/*******************Internal Variables**********************/
wire [filter_dw - 1 : 0] fir_out;
reg  [filter_dw - 1 : 0] fir_in;
reg fir_valid;

/*******************module instantiation*******************/
fir fir_filter(
	.clk(clk),
	.reset(reset),
	.clk_ena(1'b1),
	.i_valid(fir_valid),
	.i_in(fir_in),
	.o_valid(tvalid),
	.o_out(fir_out)
);

/******************Sequential Logic*************************/
always @ (posedge clk, posedge reset)
begin
	if(reset == 1'b1) begin
	  fir_in <= 0;
     fir_valid <= 1'b0;
	end
	else begin
		fir_in <= fir_in + acc_const;
		fir_valid <= 1'b1;
	end
end

/*******************Output Logic***************************/
assign tdata = {{14{1'b0}},fir_out};

endmodule 