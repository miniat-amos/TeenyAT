/* Name:	led.c
*  Author:	John Pertell
*  
*  This brief program is meant to serve as an ultra simplistic example of
*  how the TeenyAT header and source can be quickly used to create a
*  simulated system.  In this case, the system loads a binary named "tbone.bin". 
*  and only runs for 77 clock cycles. Every cycle the system checks the data on 
*  GPIO Port A. If bit0 is set to zero, then the virtual led is "off". 
*  If the bit is set to a one, then the virtual led is "on". An "off" state is 
*  representated by the '.' symbol and an "on" is represented by the '@' symbol. 
*/

#include <stdio.h>
#include <stdlib.h>
#include "teenyat.h" // Include the TeenyAT header

int main(int argc, char *argv[]) {

	/* Open the tbone binary */
	FILE *bin_file = fopen("tbone.bin", "rb");

	/* Create an instance of the TeenyAT and load in the tbone binary.
	   The "NULL" pointers indicate that the bus read/write callback   
	   functions are not being used.
	*/
	teenyat t;
	tny_init_from_file(&t, bin_file, NULL, NULL);
	tny_word port_a;

	/* Clock the TeenyAT 77 times */
	for ( int i=0; i <= 77; i++ ) {
		tny_clock(&t);

		/* Load the current value of Port A into
		   the tny_word, port_a.
		*/
		tny_get_ports(&t,&port_a, NULL);
		
		/* Check the state of bit0 */
		if(port_a.bits.bit0 == 0) {
			printf("."); // LED Off
		}
		else {
			printf("@"); // LED On
		}
	}
	printf("\n");
	return EXIT_SUCCESS;
}