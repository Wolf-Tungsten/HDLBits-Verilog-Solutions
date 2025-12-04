module top_module( 
    input [15:0] a, b,
    input cin,
    output cout,
    output [15:0] sum 
  );

    wire [3:0] cout_temp;
    
    bcd_fadd u_bcd_fadd(
        .a(a[3:0]),
        .b(b[3:0]),
        .cin(cin),
        .cout(cout_temp[0]),
        .sum(sum[3:0])
    );

    generate
    	genvar i;
    	for(i=1; i<4; i++) 
    	begin
    		bcd_fadd u_bcd_fadd(
    				.a(a[4*i+3 : 4*i]),
    				.b(b[4*i+3 : 4*i]),
    				.cin(cout_temp[i-1]),
    				.cout(cout_temp[i]),
    				.sum(sum[4*i+3 : 4*i])
    			);	
    	end
    endgenerate

    assign cout = cout_temp[3];

endmodule

// Support module for BCD full-adder used by multiple DUTs
module bcd_fadd(
    input  [3:0] a,
    input  [3:0] b,
    input        cin,
    output       cout,
    output [3:0] sum
);
    wire [4:0] t = a + b + cin;          // 0..19
    assign {cout, sum} = (t > 9) ? {1'b1, t + 5'd6} : {1'b0, t[3:0]};
endmodule
