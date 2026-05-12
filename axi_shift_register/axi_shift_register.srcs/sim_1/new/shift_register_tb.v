`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 05/11/2026 09:43:06 PM
// Design Name: 
// Module Name: shift_register_tb
// Project Name: 
// Target Devices: 
// Tool Versions: 
// Description: 
// 
// Dependencies: 
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
// 
//////////////////////////////////////////////////////////////////////////////////
`timescale 1ns / 1ps

module shift_register_tb();
    localparam T = 10;
    
    reg clk;
    reg rst_n;
    reg dir;
    reg din;
    reg en;
    wire [3:0] q;
    
    shift_register dut
    (
        .clk(clk),
        .rst_n(rst_n),
        .dir(dir),
        .din(din),
        .en(en),
        .q(q)
    );

    always
    begin
        clk = 0;
        #(T/2);
        clk = 1;
        #(T/2);
    end
    
    initial
    begin
        dir = 0;
        din = 0;
        en = 0;
        
        // *** Reset ***
        rst_n = 0;
        #(T*5);
        rst_n = 1;
        #(T*5);
        
        // *** Shift left 1001 ***
        dir = 0;    // Shift left
        en = 1;
        din = 1;
        #T;
        din = 0;
        #(T*2);
        din = 1;
        #T;
        en = 0;
        
        // *** Shift right 1100 ***
        dir = 1;    // Shift right
        en = 1;
        din = 0;
        #(T*2);
        din = 1;
        #(T*2);
        en = 0;
    end

endmodule