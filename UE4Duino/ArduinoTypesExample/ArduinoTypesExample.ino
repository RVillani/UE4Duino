void setup()
{
  Serial.begin(9600);
  Serial.setTimeout(5);
}

void loop()
{
  if (!Serial.available()) return;

  String str = Serial.readString();
  
  // command to send a float
  if (str == "float")
  {
    float fVal = 12.34f;
    byte* fBuffer = reinterpret_cast<byte*>(&fVal);
    Serial.write(fBuffer, 4);
  }

  // command to send an integer
  else if (str == "int")
  {
    long int lVal = 123456L;
    byte lBuffer[] = {
      byte(lVal & 0xff),
      byte(lVal >> 8 & 0xff),
      byte(lVal >> 16 & 0xff),
      byte(lVal >> 24 & 0xff)
    };
    Serial.write(lBuffer, 4);
  }

  // command to send a line
  else if (str == "lines")
  {
    Serial.println("This is a line.");
    Serial.println("This is another line");
  }
}
