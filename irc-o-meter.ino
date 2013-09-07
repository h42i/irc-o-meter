int mmPin = 9;

byte mac[] = { 0x13, 0x37, 0x23, 0x42, 0xAB, 0xCD };
IPAddress ip(192,168,1,184);

int ircPort = 6667;
char ircServer[] = "irc.freenode.net";

EthernetClient client;

void setup()
{
	pinMode(mmPin, OUTPUT);
	
	Serial.begin(9600);
	
	if (Ethernet.begin(mac) == 0)
	{
		Serial.println("Failed to configure Ethernet using DHCP!");
		
		for(;;)
		{
		}
	}
	
	delay(1000);
	
	if (client.connect(serverIP, serverPort))
	{
		Serial.print("Connected to ");
		Serial.print(ircServer);
		Serial.println(".");
		
		delay(100);
		
		String nick = "NICK IRCoMeter";
		String user = "USER IRCoMeter 0 * :ircometer bot";
		String join = "JOIN #hasi";
		
		client.println(nick);
		client.println(user);
		client.println(join);
	}
	else
	{
		Serial.println("Failed to connect.");
	}
}

void loop()
{
	analogWrite(mmPin, 100);
}
