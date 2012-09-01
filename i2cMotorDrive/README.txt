compile command:

gcc i2c-demo.c -I/usr/local/include -L/usr/local/lib -o i2c-demo

Usage:

	sudo ./ic2-demo --ldirection [0 = backwards, 1 = forwards]
	                --lspeed [Range 1 to 255]
                        --rdirection [0 = backwards, 1 = forwards]
                        --rspeed [Range 1 to 255]
                        --duration [duration in seconds]

Example:

	sudo ./i2cdemo --ldirection 1 --lspeed 200 --rdirection 1 --rspeed
200 --duration 2
