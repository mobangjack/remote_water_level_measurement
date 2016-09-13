//#define _TX_
#define _RX_

#ifdef _TX_

#include"tx.h"

void main(){
	tx();
}

#endif


#ifdef _RX_

#include"rx.h"

void main(){
	rx();
}

#endif

