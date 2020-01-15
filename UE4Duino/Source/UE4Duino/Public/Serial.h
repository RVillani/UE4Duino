#pragma once

#define FC_DTRDSR       0x01
#define FC_RTSCTS       0x02
#define FC_XONXOFF      0x04
#define ASCII_BEL       0x07
#define ASCII_BS        0x08
#define ASCII_LF        0x0A
#define ASCII_CR        0x0D
#define ASCII_XON       0x11
#define ASCII_XOFF      0x13

#include "CoreTypes.h"
#include "Serial.generated.h"

// Forward declaration
typedef struct _OVERLAPPED OVERLAPPED;

UENUM(BlueprintType, Category = "UE4Duino")
enum class ELineEnd : uint8
{
	rn	UMETA(DisplayName = "\r\n"),
	n	UMETA(DisplayName = "\n"),
	r	UMETA(DisplayName = "\r"),
	nr	UMETA(DisplayName = "\n\r")
};

UCLASS(BlueprintType, Category = "UE4Duino", meta = (Keywords = "com arduino serial"))
class UE4DUINO_API USerial : public UObject
{
	GENERATED_BODY()

public:
	/** Determines the line ending used when writing lines to serial port with PrintLine. */
	UPROPERTY(BlueprintReadWrite, Category = "UE4Duino | String")
	ELineEnd WriteLineEnd;

public:
	USerial();
	~USerial();

	/**
	 * Open a serial port and return the created Serial instance.
	 * Don't forget to close the port before exiting the game.
	 *
	 * @param bOpened If the serial port was successfully opened.
	 * @param Port The serial port to open.
	 * @param BaudRate BaudRate to open the serial port with.
	 * @return A Serial instance to work with the opened port.
	 */
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Open Serial Port"), Category = "UE4Duino", meta = (Keywords = "com arduino serial start"))
	static USerial* OpenComPort(bool &bOpened, int32 Port = 1, int32 BaudRate = 9600);

	/**
	 * Utility function to convert 4 bytes into an Integer. If the input array's length is not 4, returns 0.
	 * 
	 * @param Bytes A byte array with 4 values representing the integer in little-endian format.
	 * @return The final integer value or 0 for an invalid array.
	 */
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Bytes to Int"), Category = "UE4Duino", meta = (Keywords = "cast concatenate group bit bitwise"))
	static int32 BytesToInt(TArray<uint8> Bytes);

	/**
	 * Utility function to get the 4 bytes that make an integer.
	 *
	 * @param Int The integer value to be converted.
	 * @return A byte array containing the 4 bytes that make the integer, starting from the least significant one (little endian).
	 */
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Int to Bytes"), Category = "UE4Duino", meta = (Keywords = "cast separate bit bitwise"))
	static TArray<uint8> IntToBytes(const int32 &Int);

	/**
	 * Utility function to convert 4 bytes into a float. If the input array's length is not 4, returns 0.0.
	 *
	 * @param Bytes A byte array with 4 values representing the float in IEEE 754 standard format.
	 * @return The final float value or 0.0 for an invalid array.
	 */
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Bytes to Float"), Category = "UE4Duino", meta = (Keywords = "cast concatenate group bit bitwise"))
	static float BytesToFloat(TArray<uint8> Bytes);

	/**
	 * Utility function to get the 4 bytes that make a float.
	 *
	 * @param Float The float value to be converted.
	 * @return A byte array containing the 4 bytes that make the float, in IEEE 754 standard format.
	 */
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Float to Bytes"), Category = "UE4Duino", meta = (Keywords = "cast separate bit bitwise"))
	static TArray<uint8> FloatToBytes(const float &Float);

	/**
	 * Open a serial port. Don't forget to close the port before exiting the game.
	 * If this Serial instance has already an opened port,
	 * return false and doesn't change the opened port number.
	 *
	 * @param Port The serial port to open.
	 * @param BaudRate BaudRate to open the serial port with.
	 * @return If the serial port was successfully opened.
	 */
	UFUNCTION(BlueprintCallable, meta=(DisplayName = "Open Port"), Category = "UE4Duino", meta = (Keywords = "com start init"))
	bool Open(int32 Port = 2, int32 BaudRate = 9600);
	/**
	 * Close and end the communication with the serial port. If not open, do nothing.
	 */
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Close Port"), Category = "UE4Duino", meta = (Keywords = "com end finish release"))
	void Close();

	/**
	 * Will read characters from Serial port until \0 (null char) is found or there are no 
	 * characters left to read.
	 *
	 * @param bSuccess If there was anything to read.
	 * @return The read string
	 */
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Read String", keywords = "get read receive string words text characters"), Category = "UE4Duino")
	FString ReadString(bool &bSuccess);
	/**
	 * Will read characters from Serial port until \r\n (Arduino println line end) is found.
	 * 
	 * @param bSuccess If there was anything to read.
	 * @return The read string
	 */
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Read Line", keywords = "get read receive string words text characters"), Category = "UE4Duino")
	FString Readln(bool &bSuccess);
	/**
	 * Reads the string until a specific char is met.
	 * The Terminator char won't be included in the result string.
	 */
	//UFUNCTION(BlueprintCallable, meta = (DisplayName = "Read String Until", keywords = "get read receive string words text characters"), Category = "UE4Duino")
	FString ReadStringUntil(bool &bSuccess, uint8 Terminator);
	/**
	 * Reads a float from the serial port (sent as 4 bytes).
	 * @param bSuccess True if there were 4 bytes to read.
	 * @return The read value
	 */
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Read a Float", keywords = "get read receive"), Category = "UE4Duino")
	float ReadFloat(bool &bSuccess);
	/**
	 * Reads an integer from the serial port (sent as 4 bytes).
	 * @param bSuccess True if there were 4 bytes to read.
	 * @return The read value
	 */
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Read an Int", keywords = "get read receive integer"), Category = "UE4Duino")
	int32 ReadInt(bool &bSuccess);
	/**
	 * Reads a byte from the serial port.
	 * @param bSuccess True if there were 4 bytes to read.
	 * @return The read value
	 */
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Read a Byte", keywords = "get read receive"), Category = "UE4Duino")
	uint8 ReadByte(bool &bSuccess);
	/**
	 * Reads up to Limit bytes from the serial port. If there are less than Limit,
	 * reads all of them and return True.
	 * @param bSuccess True if there was at least 1 byte to read.
	 * @return An array containing the read bytes
	 */
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Read Bytes", keywords = "get read receive"), Category = "UE4Duino")
	TArray<uint8> ReadBytes(int32 Limit = 256);

	/**
	 * Writes a string without newline to the serial port.
	 * @param String The string to be sent to the serial port.
	 * @return True if the string was sent.
	 */
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Print", keywords = "send write string words text characters"), Category = "UE4Duino")
	bool Print(FString String);
	/**
	 * Writes a string with newline (\n) appended at the end to the serial port.
	 * @param String The string to be sent to the serial port.
	 * @return True if the string was sent.
	 */
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Print Line", keywords = "send write string words text characters"), Category = "UE4Duino")
	bool Println(FString String);
	/**
	 * Writes a float value to the serial port as 4 bytes.
	 * @param Value The value to be sent to the serial port.
	 * @return True if the bytes were sent.
	 */
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Write a Float", keywords = "send"), Category = "UE4Duino")
	bool WriteFloat(float Value);
	/**
	 * Writes an integer value to the serial port as 4 bytes.
	 * @param Value The value to be sent to the serial port.
	 * @return True if the bytes were sent.
	 */
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Write an Int", keywords = "integer send"), Category = "UE4Duino")
	bool WriteInt(int32 Value);
	/**
	 * Writes a byte value to the serial port.
	 * @param Value The value to be sent to the serial port.
	 * @return True if the byte was sent.
	 */
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Write a Byte", keywords = "send"), Category = "UE4Duino")
	bool WriteByte(uint8 Value);
	/**
	 * Writes a byte array as a sequence of bytes to the serial port.
	 * @param Buffer The byte array to be sent to the serial port.
	 * @return True if the bytes were sent.
	 */
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Write Bytes", keywords = "send"), Category = "UE4Duino")
	bool WriteBytes(TArray<uint8> Buffer);

	/** Clean the serial port by reading everything left to be read. */
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Flush Port"), Category = "UE4Duino")
	void Flush();

	/**
	 * Check if the serial port is open.
	 * @return True if the serial port is open.
	 */
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Is Port Open"), Category = "UE4Duino")
	bool IsOpened() { return m_hIDComDev != NULL; }

	/**
	 * Read the number of the serial port selected for this Serial instance.
	 * @return The number of the serial port.
	 */
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Get Port Number"), Category = "UE4Duino")
	int32 GetPort() { return m_Port; }

	/**
	 * Read the selected BaudRate for this Serial instance.
	 * @return The baud rate.
	 */
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Get Baud Rate"), Category = "UE4Duino")
	int32 GetBaud() { return m_Baud; }

	/**
	 * Converts a LineEnd enum value to String.
	 * @param LineEnd LineEnd enum value.
	 * @return The LineEnd value in string format.
	 */
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Line End to String", keywords = "cast convert"), Category = "UE4Duino")
	FString LineEndToStr(ELineEnd LineEnd);
	
protected:
	void* m_hIDComDev;

	// These are pointers to be able to use OVERLAPPED with forward declaration
	OVERLAPPED* m_OverlappedRead;
	OVERLAPPED* m_OverlappedWrite;

	int32 m_Port;
	int32 m_Baud;

};