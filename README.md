target: at least 1kHz sample rate. Each sample contains readings from all 8 channels, each channel reads 18 bits
	 => bits rate need to be 1kHz * 8 * 16 = 128kHz
current baud rate = 115200 bits / second = 115.2kHz

Thoughts: 
1. What is the stable baudrate? seems that it is find to double it. 
2. We can also reduce the number of bits for each channel to 16. 
