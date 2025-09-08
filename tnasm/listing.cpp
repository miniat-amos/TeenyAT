#include <iostream>
#include <iomanip>
#include <vector>

#include "teenyat.h"
#include "token.h"
#include "listing.h"

using namespace std;

vector <listing_block> listing_blocks;

void generate_listing() {

    int line_no_width = to_string(listing_blocks.size()).length();

    for(const listing_block &lb : listing_blocks) {
        /* print line's address if meaningful */
        if(lb.uses_words) {
            cout << "0x" << hex << setw(4) << setfill('0') << lb.address << dec;
        }
        else {
            /* leading space accounts for address, line number and colon */
            cout << "      ";
        }

        /* print line number */
        cout << "  " << setw(line_no_width) << setfill(' ') << lb.line_no << ": ";

        /* print hex words, if needed */

        cout << "[ ";
        if(lb.uses_words) {
            int cnt = lb.bin_uwords.size();
            int i = 0;
            while(i < cnt) {
                if(i > 0  &&  (i % 2) == 0) {
                    /* time to add a new line of words */
                    cout << "]  " << endl;
                    cout << "        " << string(line_no_width, ' ') << ": [ ";
                }
                cout << hex << setw(4) << setfill('0') << lb.bin_uwords[i] << dec;
                cout << " ";
                i++;
            }
            /* for odd numbers of words, fill final hex area with spaces */
            if(cnt % 2) {
                cout << "     ";
            }
        }
        else {
            cout << "          ";
        }

        cout << "]  ";
        /* print original asm */
        cout << lb.asm_line << endl;
    }

    return;
}
