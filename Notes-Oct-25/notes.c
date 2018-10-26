/* Jake McKenzie Notes October 25 2018
 * 1) In the main objective of this was to run this function to see 
 * how the function call was converted to assembly instructions and 
 * which branch instructions would the C compiler used in KEIL produce.
 * 2) The optimization settings can be found in the book by that one guy
 * at UT Austin. The yellow book that is really good. 
 * 3) In the future if we do not use the switches on the board, we can 
 * use an external pull up resistor instead. 
 * */
short add(short x, short y){
    short z;
    z = x + y;
    if ((x > 0) && (y > 0) && (z > 0)) 
        z = 32767;
    if ((x < 0) && (y < 0) && (z < 0))
        z = -32767;
    return z;
}
int main(void) {
    short a, b;
    a = add(2000,2000);
    b = 0;
    while(1) {
        b = add(b,1);
    }
}