`timescale 1ps / 1ps

////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer:
//
// Create Date:   22:40:11 06/23/2013
// Design Name:   usb
// Module Name:   C:/devel/checkouts/omicron_analyzer2/v2.0/design/usb_test.v
// Project Name:  design_v20
// Target Device:  
// Tool versions:  
// Description: 
//
// Verilog Test Fixture created by ISE for module: usb
//
// Dependencies:
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
// 
////////////////////////////////////////////////////////////////////////////////

module usb_test;

	// Inputs
	reg rst_n;
	reg clk_48;

	reg [6:0] usb_address;
	reg data_toggle;
	reg [1:0] handshake;
	reg [7:0] data_in;
	reg data_in_valid;

	// Outputs
	wire dp_out;
	wire dn_out;

	wire d_dir_out;
	wire usb_rst;
	wire transaction_active;
	wire [3:0] endpoint;
	wire direction_in;
	wire setup;
	wire [7:0] data_out;
	wire data_strobe;
	wire success;

    wire my_dp;
    wire my_dn;
    wire my_tx_enable;

    wire dp_in = d_dir_out? dp_out: my_tx_enable? my_dp: 1'b1;
    wire dn_in = d_dir_out? dn_out: my_tx_enable? my_dn: 1'b0;

	wire d0p_in = dp_in;
	wire d0n_in = dn_in;

	// Instantiate the Unit Under Test (UUT)
	usb uut (
		.rst_n(rst_n), 
		.clk_48(clk_48), 
		.dp_in(dp_in), 
		.dn_in(dn_in), 
		.d0p_in(d0p_in), 
		.d0n_in(d0n_in), 
		.dp_out(dp_out), 
		.dn_out(dn_out), 
		.d_dir_out(d_dir_out), 
		.usb_address(usb_address), 
		.usb_rst(usb_rst), 
		.transaction_active(transaction_active), 
		.endpoint(endpoint), 
		.direction_in(direction_in), 
		.setup(setup), 
		.data_toggle(data_toggle), 
		.handshake(handshake), 
		.data_out(data_out), 
		.data_in(data_in), 
		.data_in_valid(data_in_valid), 
		.data_strobe(data_strobe), 
		.success(success)
	);
    
    always #10416 clk_48 = !clk_48;
    
    reg[1:0] usb_clk_cntr;
    reg usb_clk;
    always @(posedge clk_48) begin
        usb_clk_cntr <= usb_clk_cntr + 1;
        if (!usb_clk_cntr)
            usb_clk = !usb_clk;
    end

    reg tx_transmit;
    reg[7:0] tx_data;
    wire tx_strobe;
    usb_tx tx(
        .rst_n(rst_n),
        .clk_48(clk_48),

        .usb_dp(my_dp),
        .usb_dn(my_dn),
        .usb_tx_en(my_tx_enable),
        
        .transmit(tx_transmit),
        .data(tx_data),
        .data_strobe(tx_strobe),
        
        .update_crc16(1'b0),
        .send_crc16(1'b0)
        );

	initial begin
		// Initialize Inputs
		rst_n = 0;
		clk_48 = 0;
		usb_address = 0;
		data_toggle = 0;
		handshake = 0;
		data_in = 0;
		data_in_valid = 0;
        tx_transmit = 0;
        tx_data = 0;
        usb_clk_cntr = 0;
        usb_clk = 0;

		// Wait 100 ns for global reset to finish
		#100000;
        rst_n = 1;
		#100000;
        
		// Add stimulus here
        tx_data = 8'b01101001;
        tx_transmit = 1;

        @(negedge tx_strobe);
        tx_data = 8'b00000000;

        @(negedge tx_strobe);
        tx_data = 8'b00010000;

        @(negedge tx_strobe);
        tx_transmit = 0;
	
        //
        data_in = "r";
        data_in_valid = 1;
        @(posedge clk_48) while (!data_strobe) @(posedge clk_48);

        data_in = ":";
        @(posedge clk_48) while (!data_strobe) @(posedge clk_48);

        data_in = "0";
        @(posedge clk_48) while (!data_strobe) @(posedge clk_48);
        data_in = "d";
        @(posedge clk_48) while (!data_strobe) @(posedge clk_48);

        data_in = ":";
        @(posedge clk_48) while (!data_strobe) @(posedge clk_48);

        data_in = "0";
        @(posedge clk_48) while (!data_strobe) @(posedge clk_48);
        data_in = "0";
        @(posedge clk_48) while (!data_strobe) @(posedge clk_48);
        data_in = "0";
        @(posedge clk_48) while (!data_strobe) @(posedge clk_48);
        data_in = "d";
        @(posedge clk_48) while (!data_strobe) @(posedge clk_48);

        data_in = "\n";
        @(posedge clk_48) while (!data_strobe) @(posedge clk_48);

        data_in_valid = 0;
    end
      
endmodule

