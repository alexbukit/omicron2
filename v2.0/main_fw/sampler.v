module sample_mux_one #(
    parameter w = 16
    )(
    input[w-1:0] i,
    output o,
    input[$clog2(w)-1:0] s
    );

assign o = i[s];

endmodule

module sample_mux #(
    parameter w = 16
    )(
    input[w-1:0] i,
    output[w-1:0] o,
    input[$clog2(w)*w-1:0] s
    );

genvar x;
generate
    for (x = 0; x < w; x = x + 1) begin : g
        sample_mux_one #(.w(w)) z(
            .i(i),
            .o(o[x]),
            .s(s[$clog2(w)*(x+1)-1:$clog2(w)*x])
            );
    end
endgenerate

endmodule

// Note that for the compressor to work, each cycle where in_strobe
// is set should be immediately followed by a cycle in which
// in_strobe is clear.
//
// The size of the output stream can be up to 1.5 times that
// of the input stream, but will typically be much closer to 0.
//
// The compressor will auto-restart every 32Ki samples (a page),
// in order to inject restart points for the potential parser.
// The autoreset counter should be reset using the `clear` signal
// before starting a sampling run. The `new_page` signal will be
// asserted along with the first sample in each page.
//
// The compressor also keeps a 40-bit sample index and pushes
// it out along with the `out_data`.
module sample_compressor(
    input clk,
    input rst_n,

    input clear,

    input[15:0] in_data,
    input in_strobe,

    output reg new_page,
    output reg[15:0] out_data,
    output reg out_strobe,
    output reg[39:0] out_sample_index,

    output reg overflow_error
    );

localparam
    st_init    = 2'd0,
    st_single  = 2'd1,
    st_run     = 2'd2,
    st_recover = 2'd3;

reg[1:0] state;
reg[15:0] last_data;
reg[15:0] cntr;
reg[14:0] reset_cntr;

reg[39:0] sample_index;

wire end_page = (reset_cntr == 15'h7fff);
reg new_page_latch;

always @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
        state <= st_init;
        last_data <= 16'sbx;
        out_data <= 1'sbx;
        out_strobe = 1'b0;
        overflow_error <= 1'b0;
        reset_cntr <= 1'b0;
        new_page_latch <= 1'b1;
        new_page <= 1'b0;
        sample_index <= 1'b0;
    end else begin
        out_data <= 1'sbx;
        out_sample_index <= 1'sbx;
        out_strobe = 1'b0;

        if (clear) begin
            state <= st_init;
            overflow_error <= 1'b0;
            reset_cntr <= 1'b0;
            new_page_latch <= 1'b1;
            new_page <= 1'b0;
            sample_index <= 1'b0;
        end else begin
            case (state)
                st_init: if (in_strobe) begin
                    out_data <= in_data;
                    out_strobe = 1'b1;
                    out_sample_index <= sample_index;
                    if (!end_page)
                        state <= st_single;
                end
                st_single: if (in_strobe) begin
                    out_data <= in_data;
                    out_strobe = 1'b1;
                    out_sample_index <= sample_index;
                    if (end_page) begin
                        state <= st_init;
                    end else if (last_data == in_data) begin
                        state <= st_run;
                        cntr <= 1'b0;
                    end
                end
                st_run: if (in_strobe) begin
                    if (cntr == 1'b0)
                        out_sample_index <= sample_index;

                    if (last_data != in_data) begin
                        state <= st_recover;
                        out_data <= cntr;
                        out_strobe = 1'b1;
                    end else begin
                        if (cntr == 16'hFFFE) begin
                            out_data <= 16'hFFFF;
                            out_strobe = 1'b1;
                            if (end_page)
                                state <= st_init;
                        end
                    end
                    if (cntr == 16'hFFFE)
                        cntr <= 1'b0;
                    else
                        cntr <= cntr + 1'b1;
                end
                st_recover: begin
                    if (in_strobe)
                        overflow_error <= 1'b1;
                    state <= st_single;
                    out_data <= last_data;
                    out_strobe = 1'b1;
                    out_sample_index <= sample_index;
                end
            endcase

            if (out_strobe) begin
                reset_cntr <= reset_cntr + 1'b1;
                new_page_latch <= end_page;
                new_page <= new_page_latch;
            end
        end

        if (in_strobe) begin
            last_data <= in_data;
            sample_index <= sample_index + 1'b1;
        end
    end
end

endmodule

module sample_strober(
    input clk,
    input rst_n,

    input[15:0] s,
    input[15:0] last_s,
    output sample_strobe,

    input enable,
    input clear_timer,

    input[31:0] period,
    input[15:0] rising_edge_mask,
    input[15:0] falling_edge_mask
    );

reg[31:0] cntr;
reg cntr_strobe;
reg rise_strobe;
reg fall_strobe;
assign sample_strobe = (enable && cntr_strobe) || rise_strobe || fall_strobe;

wire[15:0] rising_edges = ~last_s & s & rising_edge_mask;
wire[15:0] falling_edges = last_s & ~s & falling_edge_mask;

always @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
        cntr <= 1'b0;
    end else begin
        cntr_strobe <= (cntr == period);
        rise_strobe <= rising_edges != 0;
        fall_strobe <= falling_edges != 0;

        if (enable) begin
            if (cntr == period)
                cntr <= 1'b0;
            else
                cntr <= cntr + 1'b1;
        end

        if (clear_timer)
            cntr <= 1'b0;
    end
end

endmodule

module sample_serializer(
    input clk,
    input rst_n,

    input[15:0] in_data,
    input in_strobe,
    output reg[15:0] out_data,
    output reg out_strobe,
    output reg[63:0] sample_index,

    input clear_sampler_index,
    input[2:0] log_channels
    );

always @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
        out_strobe <= 1'b0;
        sample_index <= 1'b0;
    end else begin
        if (out_strobe)
            out_strobe <= 1'b0;

        if (in_strobe) begin
            case (log_channels)
                3'd0: begin
                    out_data <= { out_data[14:0], in_data[0] };
                    if (sample_index[3:0] == 4'd15)
                        out_strobe <= 1'b1;
                    sample_index <= sample_index + 4'd1;
                end
                3'd1: begin
                    out_data <= { out_data[13:0], in_data[1:0] };
                    if (sample_index[3:0] == 4'd14)
                        out_strobe <= 1'b1;
                    sample_index <= sample_index + 4'd2;
                end
                3'd2: begin
                    out_data <= { out_data[11:0], in_data[3:0] };
                    if (sample_index[3:0] == 4'd12)
                        out_strobe <= 1'b1;
                    sample_index <= sample_index + 4'd4;
                end
                3'd3: begin
                    out_data <= { out_data[7:0], in_data[7:0] };
                    if (sample_index[3:0] == 4'd8)
                        out_strobe <= 1'b1;
                    sample_index <= sample_index + 4'd8;
                end
                default: begin
                    out_data <= in_data;
                    out_strobe <= 1'b1;
                    sample_index <= sample_index + 5'd16;
                end
            endcase
        end
    end
end

endmodule

module sampler(
    input clk,
    input rst_n,

    input[15:0] s,

    output[15:0] out_data,
    output out_valid,

    input avalid,
    input awe,
    input[4:0] aaddr,
    input[31:0] adata,
    output reg bvalid,
    output reg[31:0] bdata
    );

reg[15:0] s_sync0, s_sync1, s_sync2, s_sync3, s_sync4;

always @(posedge clk) begin
    s_sync0 <= s;
    s_sync1 <= s_sync0;
    s_sync2 <= s_sync1;
    s_sync3 <= s_sync2;
    s_sync4 <= s_sync3;
end

wire sample_strobe;
reg ss0_enable;
reg ss0_clear_timer;
reg[31:0] ss0_timer_period;
reg[15:0] ss0_rising_edge_mask;
reg[15:0] ss0_falling_edge_mask;

sample_strober ss0(
    .clk(clk),
    .rst_n(rst_n),

    .s(s_sync3),
    .last_s(s_sync2),
    .sample_strobe(sample_strobe),

    .enable(ss0_enable),
    .clear_timer(ss0_clear_timer),

    .period(ss0_timer_period),
    .rising_edge_mask(ss0_rising_edge_mask),
    .falling_edge_mask(ss0_falling_edge_mask)
    );

reg[2:0] ser0_log_channels;
wire[63:0] ser0_sample_index;
reg ser0_clear_sample_index;

sample_serializer ser0(
    .clk(clk),
    .rst_n(rst_n),

    .in_data(s_sync4),
    .in_strobe(sample_strobe),
    .out_data(out_data),
    .out_strobe(out_valid),
    .sample_index(ser0_sample_index),

    .clear_sampler_index(ser0_clear_sample_index),
    .log_channels(ser0_log_channels)
    );

reg[31:0] read_temp;

always @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
        bvalid <= 1'b0;
        bdata <= 32'hxxxxxxxx;
        ss0_clear_timer <= 1'b0;
        ser0_clear_sample_index <= 1'b0;
        ss0_enable <= 1'b0;
        ss0_timer_period <= 1'b0;
        read_temp <= 1'b0;
    end else begin
        ss0_clear_timer <= 1'b0;
        ser0_clear_sample_index <= 1'b0;

        if (avalid && awe) begin
            case ({ aaddr[4:2], 2'b00 })
                5'h00: begin
                    ss0_enable <= adata[0];
                    ss0_clear_timer <= adata[1];
                    ser0_clear_sample_index <= adata[2];
                    ser0_log_channels <= adata[10:8];
                end
                5'h04: begin
                    ss0_timer_period <= adata;
                end
                5'h08: begin
                    { ss0_rising_edge_mask, ss0_falling_edge_mask } <= adata;
                end
            endcase
        end

        if (avalid && !awe) begin
            case ({ aaddr[4:2], 2'b00 })
                5'd00: bdata <= { ser0_log_channels, 7'b0, ss0_enable };
                5'd04: bdata <= ss0_timer_period;
                5'd08: bdata <= { ss0_rising_edge_mask, ss0_falling_edge_mask };
                5'd10: begin
                    bdata <= ser0_sample_index[31:0];
                    read_temp <= ser0_sample_index[63:32];
                end
                5'd14: bdata <= read_temp;
                default: bdata <= 32'hxxxxxxxx;
            endcase
        end

        bvalid <= avalid;
    end
end

endmodule
