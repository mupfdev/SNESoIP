/*
 * virtual snesoip userspace device v0.1 beta
 * by saturnu
 * 
 * If uhid is not available as /dev/uhid, then you can pass a different path as
 * first argument.
 * If <linux/uhid.h> is not installed in /usr, then compile this with:
 *   gcc -o ./vsnesoip -Wall -I./include ./vsnesoip.c
 * And ignore the warning about kernel headers. However, it is recommended to
 * use the installed uhid.h if available.
 */

#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/types.h>

#include <linux/uhid.h>
#include <linux/input.h>
#include <linux/joystick.h>

#include "axbtnmap.h"
#include "vsnesoip.h"

char  *confFile = "vsnesoip.conf";


#define NAME_LENGTH 128

int read_joystick(int fd);

char *axis_names[ABS_MAX + 1] = {
"X", "Y", "Z", "Rx", "Ry", "Rz", "Throttle", "Rudder", 
"Wheel", "Gas", "Brake", "?", "?", "?", "?", "?",
"Hat0X", "Hat0Y", "Hat1X", "Hat1Y", "Hat2X", "Hat2Y", "Hat3X", "Hat3Y",
"?", "?", "?", "?", "?", "?", "?", 
};

char *button_names[KEY_MAX - BTN_MISC + 1] = {
"Btn0", "Btn1", "Btn2", "Btn3", "Btn4", "Btn5", "Btn6", "Btn7", "Btn8", "Btn9", "?", "?", "?", "?", "?", "?",
"LeftBtn", "RightBtn", "MiddleBtn", "SideBtn", "ExtraBtn", "ForwardBtn", "BackBtn", "TaskBtn", "?", "?", "?", "?", "?", "?", "?", "?",
"Trigger", "ThumbBtn", "ThumbBtn2", "TopBtn", "TopBtn2", "PinkieBtn", "BaseBtn", "BaseBtn2", "BaseBtn3", "BaseBtn4", "BaseBtn5", "BaseBtn6", "BtnDead",
"BtnA", "BtnB", "BtnC", "BtnX", "BtnY", "BtnZ", "BtnTL", "BtnTR", "BtnTL2", "BtnTR2", "BtnSelect", "BtnStart", "BtnMode", "BtnThumbL", "BtnThumbR", "?",
"?", "?", "?", "?", "?", "?", "?", "?", "?", "?", "?", "?", "?", "?", "?", "?", 
"WheelBtn", "Gear up",
};


//Game Pad usb-desc
static unsigned char rdesc[] = {	
    0x05, 0x01,                    // USAGE_PAGE (Generic Desktop)
    0x09, 0x05,                    // USAGE (Game Pad)
    0xa1, 0x01,                    // COLLECTION (Application)
    0xa1, 0x00,                    //   COLLECTION (Physical)  
    0x05, 0x09,                    //     USAGE_PAGE (Button)
    0x19, 0x01,                    //     USAGE_MINIMUM (Button 1)
    0x29, 0x18,                    //     USAGE_MAXIMUM (Button 24)
    0x15, 0x00,                    //     LOGICAL_MINIMUM (0)
    0x25, 0x01,                    //     LOGICAL_MAXIMUM (1)
    0x95, 0x18,                    //     REPORT_COUNT (24)
    0x75, 0x01,                    //     REPORT_SIZE (1)
    0x81, 0x02,                    //     INPUT (Data,Var,Abs)
    0x05, 0x01,                    //     USAGE_PAGE (Generic Desktop)
    0x09, 0x30,                    //     USAGE (X)
    0x09, 0x31,                    //     USAGE (Y)
    0x09, 0x32,                    //     USAGE (Z)
    0x09, 0x33,                    //     USAGE (Rx)
    0x15, 0x81,                    //     LOGICAL_MINIMUM (-127)
    0x25, 0x7f,                    //     LOGICAL_MAXIMUM (127)
    0x75, 0x08,                    //     REPORT_SIZE (8)
    0x95, 0x04,                    //     REPORT_COUNT (4)
    0x81, 0x02,                    //     INPUT (Data,Var,Abs)
    0xc0,                          //     END_COLLECTION
    0xc0                           // END_COLLECTION
};


static unsigned char b0;
static unsigned char b1;


static bool btn1_down;
static bool btn2_down;
static bool btn3_down;
static bool btn4_down;
static bool btn5_down;
static bool btn6_down;
static bool btn7_down;
static bool btn8_down;
static bool btn9_down;
static bool btn10_down;
static bool btn11_down;
static bool btn12_down;
static bool btn13_down;
static bool btn14_down;
static bool btn15_down;
static bool btn16_down;
static bool btn17_down;
static bool btn18_down;
static bool btn19_down;
static bool btn20_down;
static bool btn21_down;
static bool btn22_down;
static bool btn23_down;
static bool btn24_down;



static int uhid_write(int fd, const struct uhid_event *ev)
{
	ssize_t ret;

	ret = write(fd, ev, sizeof(*ev));
	if (ret < 0) {
		fprintf(stderr, "Cannot write to uhid: %m\n");
		return -errno;
	} else if (ret != sizeof(*ev)) {
		fprintf(stderr, "Wrong size written to uhid: %ld != %lu\n",
			ret, sizeof(ev));
		return -EFAULT;
	} else {
		return 0;
	}
}

static int create(int fd)
{
	struct uhid_event ev;

	memset(&ev, 0, sizeof(ev));
	ev.type = UHID_CREATE;
	strcpy((char*)ev.u.create.name, "vsnesoip-gamepad");
	ev.u.create.rd_data = rdesc;
	ev.u.create.rd_size = sizeof(rdesc);
	ev.u.create.bus = BUS_USB;
	ev.u.create.vendor = 0x057E; //Nintendo
	ev.u.create.product = 0x0815;
	ev.u.create.version = 0;
	ev.u.create.country = 0;

	return uhid_write(fd, &ev);
}

static void destroy(int fd)
{
	struct uhid_event ev;

	memset(&ev, 0, sizeof(ev));
	ev.type = UHID_DESTROY;

	uhid_write(fd, &ev);
}


static int send_event(int fd)
{
	struct uhid_event ev;

	memset(&ev, 0, sizeof(ev));
	ev.type = UHID_INPUT;
	ev.u.input.size = 24;

if(!disable_p1){
	
	//player 1
	if (btn1_down)
		ev.u.input.data[0] |= 1 << 0;
	if (btn2_down)
		ev.u.input.data[0] |= 1 << 1;
	if (btn3_down)
		ev.u.input.data[0] |= 1 << 2;
	if (btn4_down)
		ev.u.input.data[0] |= 1 << 3;
	if (btn5_down)
		ev.u.input.data[0] |= 1 << 4;
	if (btn6_down)
		ev.u.input.data[0] |= 1 << 5;					
	if (btn7_down)
		ev.u.input.data[0] |= 1 << 6;					
	if (btn8_down)
		ev.u.input.data[0] |= 1 << 7;					
	if (btn9_down)
		ev.u.input.data[1] |= 1 << 0;				
	if (btn10_down)
		ev.u.input.data[1] |= 1 << 1;				
	if (btn11_down)
		ev.u.input.data[1] |= 1 << 2;			
	if (btn12_down)
		ev.u.input.data[1] |= 1 << 3;	
 }		
	
	//player 2			
	if (btn13_down)
		ev.u.input.data[1] |= 1 << 4;				
	if (btn14_down)
		ev.u.input.data[1] |= 1 << 5;				
	if (btn15_down)
		ev.u.input.data[1] |= 1 << 6;				
	if (btn16_down)
		ev.u.input.data[1] |= 1 << 7;				
	if (btn17_down)
		ev.u.input.data[2] |= 1 << 0;				
	if (btn18_down)
		ev.u.input.data[2] |= 1 << 1;				
	if (btn19_down)
		ev.u.input.data[2] |= 1 << 2;				
	if (btn20_down)
		ev.u.input.data[2] |= 1 << 3;				
	if (btn21_down)
		ev.u.input.data[2] |= 1 << 4;				
	if (btn22_down)
		ev.u.input.data[2] |= 1 << 5;				
	if (btn23_down)
		ev.u.input.data[2] |= 1 << 6;					
	if (btn24_down)
		ev.u.input.data[2] |= 1 << 7;				
				
	return uhid_write(fd, &ev);
}



static int button_mapping(int fd)
{
	char buf[128];
	ssize_t ret, i;

	ret = read(STDIN_FILENO, buf, sizeof(buf));
	if (ret == 0) {
		fprintf(stderr, "Read HUP on stdin\n");
		return -EFAULT;
	} else if (ret < 0) {
		fprintf(stderr, "Cannot read stdin: %m\n");
		return -errno;
	}

int s=3;

	for (i = 0; i < ret; ++i) {
		switch (buf[i]) {
		case 'r': 
			sleep(s);
			btn13_down = !btn13_down;
			send_event(fd);
			sleep(1);
			btn13_down = !btn13_down;
			send_event(fd);
			
			break;
		case 'l': 
			sleep(s);
			btn14_down = !btn14_down;
			send_event(fd);
			sleep(1);
			btn14_down = !btn14_down;
			send_event(fd);			
			break;
		case 'd':
			sleep(s);
			btn15_down = !btn15_down;
			send_event(fd);
			sleep(1);
			btn15_down = !btn15_down;
			send_event(fd);			
			break;
		case 'u': 
			sleep(s);
			btn16_down = !btn16_down;
			send_event(fd);
			sleep(1);
			btn16_down = !btn16_down;
			send_event(fd);			
			break;
		case 's': 
			sleep(s);
			btn17_down = !btn17_down;
			send_event(fd);
			sleep(1);
			btn17_down = !btn17_down;
			send_event(fd);			
			break;
		case 'e': 
			sleep(s);
			btn18_down = !btn18_down;
			send_event(fd);
			sleep(1);
			btn18_down = !btn18_down;
			send_event(fd);			
			break;
		case 'y': 
			sleep(s);
			btn19_down = !btn19_down;
			send_event(fd);
			sleep(1);
			btn19_down = !btn19_down;
			send_event(fd);			
			break;
		case 'b': 
			sleep(s);
			btn20_down = !btn20_down;
			send_event(fd);
			sleep(1);
			btn20_down = !btn20_down;
			send_event(fd);			
			break;
		case 'a': 
			sleep(s);
			btn21_down = !btn21_down;
			send_event(fd);
			sleep(1);
			btn21_down = !btn21_down;
			send_event(fd);			
			break;
		case 'x':
			sleep(s);
			btn22_down = !btn22_down;
			send_event(fd);
			sleep(1);
			btn22_down = !btn22_down;
			send_event(fd);			
			break;
		case '1':
			sleep(s);
			btn23_down = !btn23_down;
			send_event(fd);
			sleep(1);
			btn23_down = !btn23_down;
			send_event(fd);			
			break;
		case '2': 
			sleep(s);
			btn24_down = !btn24_down;
			send_event(fd);
			sleep(1);
			btn24_down = !btn24_down;
			send_event(fd);			
			break;
																																																								
			
			
			
		case 'q':
			return -ECANCELED;
		default:
			fprintf(stderr, "Invalid input: %c\n", buf[i]);
		}
	}

	return 0;
}


int main(int argc, char **argv)
{
	int fd;
	const char *path = "/dev/uhid";
	struct pollfd pfds[2];
	int ret;
	struct termios state;

	ret = tcgetattr(STDIN_FILENO, &state);
	if (ret) {
		fprintf(stderr, "Cannot get tty state\n");
	} else {
		state.c_lflag &= ~ICANON;
		state.c_cc[VMIN] = 1;
		ret = tcsetattr(STDIN_FILENO, TCSANOW, &state);
		if (ret)
			fprintf(stderr, "Cannot set tty state\n");
	}

	if (argc >= 2) {
		if (!strcmp(argv[1], "-h") || !strcmp(argv[1], "--help")) {
			puts("");
			fprintf(stderr, "Usage: %s [%s]\n", argv[0], path);
			
			return EXIT_SUCCESS;
		} else {
			path = argv[1];
		}
	}

	if(debug)
	fprintf(stderr, "Open uhid-cdev %s\n", path);
	
	fd = open(path, O_RDWR | O_CLOEXEC);
	if (fd < 0) {
		fprintf(stderr, "Cannot open uhid-cdev %s: %m\n", path);
		return EXIT_FAILURE;
	}
	fdx=fd;

	if(debug)
	fprintf(stderr, "Create uhid device\n");
	
	ret = create(fd);
	if (ret) {
		close(fd);
		return EXIT_FAILURE;
	}

	init_login(confFile);
	
	printf("get master data done\n");
	
	//start joystick reading
	if(!mapping_mode)
	read_joystick(fd);
	else{
	
	
	
	
	pfds[0].fd = STDIN_FILENO;
	pfds[0].events = POLLIN;
	pfds[1].fd = fd;
	pfds[1].events = POLLIN;

	fprintf(stderr, "mapping mode:\nbutton pressed 3sec after input\ndpad:abxy buttons:udlr 1:L 2:R s:start e:select\nPress 'q' to quit...\n");
	while (1) {
		ret = poll(pfds, 2, -1);
		if (ret < 0) {
			fprintf(stderr, "Cannot poll for fds: %m\n");
			break;
		}
		if (pfds[0].revents & POLLHUP) {
			fprintf(stderr, "Received HUP on stdin\n");
			break;
		}
		if (pfds[1].revents & POLLHUP) {
			fprintf(stderr, "Received HUP on uhid-cdev\n");
			break;
		}

		if (pfds[0].revents & POLLIN) {
			ret = button_mapping(fd);
			if (ret)
				break;
		}
	}
	}
	

	//never reached ^^
	fprintf(stderr, "\nDestroy uhid device\n");
	destroy(fd);
	return EXIT_SUCCESS;
}


int read_joystick (int fd0)
{
	
	int fd, i;
	unsigned char axes = 2;
	unsigned char buttons = 2;
	int version = 0x000800;
	char name[NAME_LENGTH] = "Unknown";
	uint16_t btnmap[BTNMAP_SIZE];
	uint8_t axmap[AXMAP_SIZE];
	int btnmapok = 1;


	if ((fd = open(js_dev, O_RDONLY)) < 0) {		
		perror("error: could not open device");
		return 1;
	}
	
	ioctl(fd, JSIOCGVERSION, &version);
	ioctl(fd, JSIOCGAXES, &axes);
	ioctl(fd, JSIOCGBUTTONS, &buttons);
	ioctl(fd, JSIOCGNAME(NAME_LENGTH), name);

	getaxmap(fd, axmap);
	getbtnmap(fd, btnmap);

	if(debug)
	printf("Driver version is %d.%d.%d.\n", version >> 16, (version >> 8) & 0xff, version & 0xff);

	/* Determine whether the button map is usable. */
	for (i = 0; btnmapok && i < buttons; i++) {
		if (btnmap[i] < BTN_MISC || btnmap[i] > KEY_MAX) {
			btnmapok = 0;
			break;
		}
	}
	if (!btnmapok) {
		/* btnmap out of range for names. Don't print any. */
		puts("vsnesoip is not fully compatible with your kernel. Unable to retrieve button map!");
		printf("Joystick (%s) has %d axes ", name, axes);
		printf("and %d buttons.\n", buttons);
	} else {
		printf("Joystick (%s) has %d axes (", name, axes);
		for (i = 0; i < axes; i++)
			printf("%s%s", i > 0 ? ", " : "", axis_names[axmap[i]]);
		puts(")");

		printf("and %d buttons (", buttons);
		for (i = 0; i < buttons; i++) {
			printf("%s%s", i > 0 ? ", " : "", button_names[btnmap[i] - BTN_MISC]);
		}
		puts(").");
	}

	printf("(interrupt to exit)\n");

		struct js_event js;

		while (1) {
			if (read(fd, &js, sizeof(struct js_event)) != sizeof(struct js_event)) {
				perror("\nvsnesoip: error reading");
				return 1;
			}

			if(debug){
				if(js.type!=129 && js.type!=130) //init values?
				printf("Event: type %d, time %d, number %d, value %d\n", js.type, js.time, js.number, js.value);
			}

			//AXIS
			if(js.type==js_axis_type || js.type==js_button_type){

					if(js.type==js_axis_type){
						
						if(js.number==js_axis_y_nr){

							if(js.value==js_axis_up_val) { 
								 btn1_down=1; b1 |= 1 << 3; 
								 }
								 
							if(js.value==js_axis_down_val) {
								btn2_down=1; b1 |= 1 << 2;
								}
								
							if(js.value==0) { 
									btn1_down = 0;
									btn2_down = 0;
									b1 &= ~(1 << 3);
									b1 &= ~(1 << 2);
							}
							
						
						}
						else if(js.number==js_axis_x_nr){
						
							if(js.value==js_axis_left_val) {
								btn3_down=1; b1 |= 1 << 1;
								}
								
							if(js.value==js_axis_right_val) { 
								btn4_down=1; b1 |= 1 << 0;
								}
								
							if(js.value==0) {
									btn3_down=0;
									btn4_down=0;
									b1 &= ~(1 << 1);
									b1 &= ~(1 << 0);
							}
						}
					}
					//BUTTONS
					else if(js.type==js_button_type){
					
							if(js.number==js_button_a_nr ) { btn5_down=js.value; if(js.value)  b0 |= 1 << 7; else b0 &= ~(1 << 7); }
							if(js.number==js_button_b_nr ) { btn6_down=js.value; if(js.value)  b1 |= 1 << 7; else b1 &= ~(1 << 7); }
							if(js.number==js_button_x_nr ) { btn7_down=js.value; if(js.value)  b0 |= 1 << 6; else b0 &= ~(1 << 6); }
							if(js.number==js_button_y_nr ) { btn8_down=js.value; if(js.value)  b1 |= 1 << 6; else b1 &= ~(1 << 6); }
							if(js.number==js_button_l_nr ) { btn9_down=js.value; if(js.value)  b0 |= 1 << 5; else b0 &= ~(1 << 5); }
							if(js.number==js_button_r_nr ) { btn10_down=js.value;  if(js.value)  b0 |= 1 << 4; else b0 &= ~(1 << 4); }
							if(js.number==js_button_st_nr ) { btn11_down=js.value; if(js.value)  b1 |= 1 << 4; else b1 &= ~(1 << 4); }
							if(js.number==js_button_se_nr ) { btn12_down=js.value; if(js.value)  b1 |= 1 << 5; else b1 &= ~(1 << 5); }
						}

					//exit combination
					if(select_x)
						if(btn7_down && btn12_down){
							return 0;
							fflush(stdout);
						}
		
					if(!disable_p1)
					send_event(fd0); //send button to virtual game pad device
					
					char buffer[6];
								
					if(ready){
						
						if(debug){
						//printf("send [%02X][%02X]\n",b1,b0);		
						printf("send b1 %s\n", byte_to_binary(b1));
						printf("send b0 %s\n", byte_to_binary(b0));
						}
						
						snprintf(buffer,12,"%02X%02X\n",b1,b0);					
						//send button to snesoip opponent
						sendto(_SNESoIP_socket_tcp,buffer,strlen(buffer),0,
						(struct sockaddr *)&_SNESoIP_server_addr,sizeof(struct sockaddr_in) );
						//only slave mode
						//TODO master mode
						
					}//end connection ok		
					
				}//end axis or button


			fflush(stdout);
		} //break while to exit
		return -1;
	}
	
const char *byte_to_binary(int x)
{
    static char b[9];
    b[0] = '\0';

    int z;
    for (z = 128; z > 0; z >>= 1)
    {
        strcat(b, ((x & z) == z) ? "1" : "0");
    }

    return b;
}

	
//called by incoming thread	
int process_incoming(unsigned char a, unsigned char b){

		//process network data
		if(debug){
			//printf("recv from echo server: %02x %02x\n", a, b);		
			printf("recv b1 %s\n", byte_to_binary(a));
			printf("recv b0 %s\n", byte_to_binary(b));
		}

		btn13_down = a & (1 << 0); //Ri
		btn14_down = a & (1 << 1); //Le
		btn15_down = a & (1 << 2); //Do
		btn16_down = a & (1 << 3); //Up
		btn17_down = a & (1 << 4); //St
		btn18_down = a & (1 << 5); //Sl
		btn19_down = a & (1 << 6); //Y
		btn20_down = a & (1 << 7); //B 
		btn21_down = b & (1 << 7); //A
		btn22_down = b & (1 << 6); //X
		btn23_down = b & (1 << 5); //L
		btn24_down = b & (1 << 4); //R

		send_event(fdx); 

	return 0;
}





