/**
 * Bowler Server, this is a basic bowler server
 * lib https://github.com/madhephaestus/BowlerCom
 */
int baudrate = 9600;
#include <BowlerCom.h>

int txPin = 11;
int rxPin = 10;
BowlerCom com;
SoftwareSerial mySerial(rxPin, txPin); // RX, TX

#define comBuffSize 256
uint8_t privateRXCom[comBuffSize];
BYTE_FIFO_STORAGE store;
uint8_t err, i;
BowlerPacket Packet;
boolean processTest(BowlerPacket * Packet) {
	println_E("My Code executed ");
	// process packet here
	uint8_t i = 0;
	float divisor = ((float) get32bit(Packet, 0)) / 1000.0; // pull out a fixed point into a float
	int8_t size = Packet->use.data[4]; //size of the list of numbers
	println_E("Divisor = ");
	p_fl_E(divisor);
	for (i = 0; i < size; i++) {
		int32_t value = get32bit(Packet, 5 + (i * 4)); // extract the value int he list of valieus
		println_E("Value = ");
		p_int_E(value);
		float calculation = ((float) value) / divisor;
		println_E("Calculation = ");
		p_fl_E(calculation);
		int32_t transportNum =calculation * 1000.0;
		println_E("Transport Num = ");
			p_int_E(transportNum);
		set32bit(Packet, // the packet struct referenc to write to
				transportNum, // the value scaled up for transport
				3 + (i * 4)); // calculate the offset in the packet
	}
	Packet->use.data[0] = 37; // set the int
	Packet->use.data[1] = 1; // set the boolean as 1 or 0
	Packet->use.data[2] = size; // set the size of the array of floats
	printPacket(Packet, ERROR_PRINT);
}

NAMESPACE_LIST testns;
RPC_LIST test_ns_test_get;

void setup() {

	mySerial.begin(baudrate);
	Serial.begin(baudrate);
#  if  defined(__ARDUINO_ARC__)
  while (!Serial ||!mySerial );    // wait for the serial port to open
#endif
	com.begin(&Serial);

	startDebugPint(&mySerial);
	//setPrintLevelInfoPrint();
	//setPrintLevelWarningPrint();
	setPrintLevelErrorPrint();
	//setPrintLevelNoPrint();
	
	//Load the namespace data
	testns.rpcSet = NULL;
	testns.asyncEventCheck = NULL;//a function pointer for calling every time the server is called
	testns.next = NULL;
	strcpy(testns.namespaceString, "test.ns.*;1.0;;");
	//Load the RPC data
	//This is the bowler method for this RPC
	test_ns_test_get.bowlerMethod = BOWLER_GET;
	//This is the callback function pointer for execution of the method
	test_ns_test_get.callback = &processTest;
	//This is the array of argument data types
	test_ns_test_get.arguments[0] = BOWLER_FIXED1K;
	test_ns_test_get.arguments[1] = BOWLER_I32STR; // a list of numbers as 32 bit signed integers
	test_ns_test_get.arguments[2] = 0;
	//This is the bowler method for this RPC
	test_ns_test_get.responseMethod = BOWLER_POST;
	//This is the array of argument data types
	test_ns_test_get.responseArguments[0] = BOWLER_I08; //8 bit ints
	test_ns_test_get.responseArguments[1] = BOWLER_BOOL; // boolean values
	test_ns_test_get.responseArguments[2] = BOWLER_FIXED1K_STR;
	test_ns_test_get.responseArguments[3] = 0;

	//This is the linked list field
	test_ns_test_get.next = NULL;
	//This is the 4 byte code for of the RPC
	strcpy( test_ns_test_get.rpc, "test");

	// add the new RPC to its namespace
	addRpcToNamespace(&testns, &test_ns_test_get);
	//add namespace to stack
	addNamespaceToList(&testns);
	println_E("Starting..");
}

void loop() {
	com.server();
}

