// Serial.h

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

#include "UE4DuinoPCH.h"
#include "AllowWindowsPlatformTypes.h"
#include "windows.h"
#include "HideWindowsPlatformTypes.h"
#include "Serial.generated.h"

UCLASS(BlueprintType, Category = "UE4Duino")
class USerial : public UObject
{
	GENERATED_BODY()

public:
	USerial();
	~USerial();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Open COM Port"), Category = "UE4Duino")
	static USerial* OpenComPort(bool &bOpened, int32 Port = 1, int32 BaudRate = 9600);

	UFUNCTION(BlueprintCallable, meta=(DisplayName = "Open COM"), Category = "UE4Duino")
	bool Open(int32 Port = 2, int32 Baud = 9600);
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Close COM"), Category = "UE4Duino")
	void Close();

	/**
	* Will read characters from Serial port until \0 (null char) is found or there are no 
	* characters left to read.
	*/
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Read String", keywords = "get read receive string words text characters"), Category = "UE4Duino")
	FString ReadString(bool &bSuccess);
	/** Will read characters from Serial port until \n (newline) is found */
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Read Line", keywords = "get read receive string words text characters"), Category = "UE4Duino")
	FString Readln(bool &bSuccess);
	FString ReadStringUntil(bool &bSuccess, uint8 Terminator);
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Read a Float", keywords = "get read receive"), Category = "UE4Duino")
	float ReadFloat(bool &bSuccess);
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Read an Int", keywords = "get read receive integer"), Category = "UE4Duino")
	int32 ReadInt(bool &bSuccess);
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Read a Byte", keywords = "get read receive"), Category = "UE4Duino")
	uint8 ReadByte(bool &bSuccess);
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Read Bytes", keywords = "get read receive"), Category = "UE4Duino")
	TArray<uint8> ReadBytes(int32 Limit = 256);

	/** Writes a string without newline to the serial port. */
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Print", keywords = "send write string words text characters"), Category = "UE4Duino")
	bool Print(FString String);
	/** Writes a string with newline at the end to the serial port. */
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Print Line", keywords = "send write string words text characters"), Category = "UE4Duino")
	bool Println(FString String);
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Write a Float", keywords = "send"), Category = "UE4Duino")
	bool WriteFloat(float Value);
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Write an Int", keywords = "integer send"), Category = "UE4Duino")
	bool WriteInt(int32 Value);
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Write a Byte", keywords = "send"), Category = "UE4Duino")
	bool WriteByte(uint8 Value);
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Write Bytes", keywords = "send"), Category = "UE4Duino")
	bool WriteBytes(TArray<uint8> Buffer);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Flush COM"), Category = "UE4Duino")
	void Flush();

	UFUNCTION(BlueprintPure, meta = (DisplayName = "Is COM Open"), Category = "UE4Duino")
	bool IsOpened() { return m_hIDComDev != NULL; }

	UFUNCTION(BlueprintPure, meta = (DisplayName = "Get Port Number"), Category = "UE4Duino")
	int32 GetPort() { return m_Port; }

	UFUNCTION(BlueprintPure, meta = (DisplayName = "Get Baud Rate"), Category = "UE4Duino")
	int32 GetBaud() { return m_Baud; }
	
protected:
	void* m_hIDComDev;
	OVERLAPPED m_OverlappedRead, m_OverlappedWrite;

	int32 m_Port;
	int32 m_Baud;

};