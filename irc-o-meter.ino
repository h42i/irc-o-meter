#include <SPI.h>
#include <Ethernet.h>

int mmPin = 9;

int greenPin = A0;
int redPin = 4;

byte mac[] = { 0x00, 0x26, 0x2D, 0x01, 0x55, 0xEA };
IPAddress ip(192,168,1,184);

int ircPort = 6667;
char ircServer[] = "irc.freenode.net";

EthernetClient client;

int loopCounter = 0;

int greenCounter = 0;
int redCounter = 0;

int refreshInterval = 2;
int baseLevel = 50;

int aggregatedMessages = 0;

void connectToIRC()
{
	if (client.connect(ircServer, ircPort))
	{
		Serial.print("Connected to ");
		Serial.print(ircServer);
		Serial.println(".");
		
		delay(10);
		
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

bool isCommand(String message, String command)
{
	int matchCounter = 0;
		
	String messageWithoutCommand = "";
	
	for (int i = 0; i < message.length(); i++)
	{
		if (message[i] == command[matchCounter])
		{
			matchCounter++;
		}
		else
		{
			matchCounter = 0;
		}
		
		if (matchCounter == command.length())
		{
			return true;
		}
	}
	
	return false;
}

void refreshIRC()
{
	if (!client.connected())
	{
		client.stop();
		
		connectToIRC();
	}
	
	String message = "";
	
	if (client.available())
	{
		char c;
		
		while ((c = client.read()) != '\n')
		{
			message += c;
		}
		
		//Serial.println(message);
		
		String privmsgCommand = "PRIVMSG";
		String pingCommand = "PING";
		String joinCommand = "JOIN";
		String partCommand = "PART";
		
		if (isCommand(message, pingCommand))
		{
			String pongCommand = "PONG";
			String answer = message.substring(pingCommand.length());
			
			client.println(pongCommand + answer);
		}
		else if (isCommand(message, privmsgCommand))
		{
			aggregatedMessages++;
			
			Serial.println(String(baseLevel));
			
			for (int i = 0; i < 50; i++)
			{
				analogWrite(mmPin, 255);
				delay(10);
			}
		}
		else if (isCommand(message, joinCommand))
		{
			digitalWrite(greenPin, HIGH);
			
			greenCounter = 10000;
		}
		else if (isCommand(message, partCommand))
		{
			digitalWrite(redPin, HIGH);
			
			redCounter = 10000;
		}
	}
}

void refreshBaseLevel()
{
	refreshIRC();
	
	baseLevel = (baseLevel - 1) + 64 * aggregatedMessages;
	
	if (baseLevel < 0)
	{
		baseLevel = 0;
	}
	
	aggregatedMessages = 0;
}

void setup()
{
	pinMode(mmPin, OUTPUT);
	
	pinMode(A0, OUTPUT);
	pinMode(A1, OUTPUT);
	
	digitalWrite(greenPin, LOW);
	digitalWrite(A1, LOW);
	
	pinMode(4, OUTPUT);
	pinMode(5, OUTPUT);
	
	digitalWrite(redPin, LOW);
	digitalWrite(5, LOW);
	
	Serial.begin(9600);
	
	Ethernet.begin(mac, ip);
	
	delay(1000);
	
	connectToIRC();
}

void loop()
{
	refreshIRC();
	
	if (loopCounter >= 5000)
	{
		refreshBaseLevel();
		loopCounter = 0;
	}
	else
	{
		loopCounter++;
	}
	
	if (greenCounter)
	{
		greenCounter--;
		
		if (!greenCounter)
		{
			digitalWrite(greenPin, LOW);
		}
	}
	
	if (redCounter)
	{
		redCounter--;
		
		if (!redCounter)
		{
			digitalWrite(redPin, LOW);
		}
	}
	
	analogWrite(mmPin, baseLevel / 4);
}
