//Based off the "Arduino and C++ (for Windows)" code found at: http://playground.arduino.cc/Interfacing/CPPWindows

#include "Serial.h"

#include "Windows/AllowWindowsPlatformTypes.h"
#include "Windows/MinWindows.h"
#include "Windows/HideWindowsPlatformTypes.h"

#define BOOL2bool(B) B == 0 ? false : true

USerial* USerial::OpenComPort(bool &bOpened, int32 Port, int32 BaudRate)
{
	USerial* Serial = NewObject<USerial>();
	bOpened = Serial->Open(Port, BaudRate);
	return Serial;
}

int32 USerial::BytesToInt(TArray<uint8> Bytes)
{
	if (Bytes.Num() != 4)
	{
		return 0;
	}

	return *reinterpret_cast<int32*>(Bytes.GetData());
}

TArray<uint8> USerial::IntToBytes(const int32 &Int)
{
	TArray<uint8> Bytes;
	Bytes.Append(reinterpret_cast<const uint8*>(&Int), 4);
	return Bytes;
}

float USerial::BytesToFloat(TArray<uint8> Bytes)
{
	if (Bytes.Num() != 4)
	{
		return 0;
	}

	return *reinterpret_cast<float*>(Bytes.GetData());
}

TArray<uint8> USerial::FloatToBytes(const float &Float)
{
	TArray<uint8> Bytes;
	Bytes.Append(reinterpret_cast<const uint8*>(&Float), 4);
	return Bytes;
}

USerial::USerial()
	: WriteLineEnd(ELineEnd::n)
	, m_hIDComDev(nullptr)
	, m_OverlappedRead(nullptr)
	, m_OverlappedWrite(nullptr)
	, m_Port(-1)
	, m_Baud(-1)
{
	// Allocate the OVERLAPPED structs
	m_OverlappedRead = new OVERLAPPED();
	m_OverlappedWrite = new OVERLAPPED();

	FMemory::Memset(m_OverlappedRead, 0, sizeof(OVERLAPPED));
	FMemory::Memset(m_OverlappedWrite, 0, sizeof(OVERLAPPED));
}

USerial::~USerial()
{
	Close();

	// Delete allocated OVERLAPPED structs
	delete m_OverlappedRead;
	delete m_OverlappedWrite;
}

bool USerial::Open(int32 nPort, int32 nBaud)
{
	if (nPort < 0)
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid port number: %d"), nPort);
		return false;
	}
	if (m_hIDComDev)
	{
		UE_LOG(LogTemp, Warning, TEXT("Trying to use opened Serial instance to open a new one. "
				"Current open instance port: %d | Port tried: %d"), m_Port, nPort);
		return false;
	}

	FString szPort;
	if (nPort < 10)
		szPort = FString::Printf(TEXT("COM%d"), nPort);
	else
		szPort = FString::Printf(TEXT("\\\\.\\COM%d"), nPort);
	DCB dcb;

	m_hIDComDev = CreateFile(*szPort, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, NULL);
	if (m_hIDComDev == NULL)
	{
		unsigned long dwError = GetLastError();
		UE_LOG(LogTemp, Error, TEXT("Failed to open port COM%d (%s). Error: %08X"), nPort, *szPort, dwError);
		return false;
	}

	FMemory::Memset(m_OverlappedRead, 0, sizeof(OVERLAPPED));
	FMemory::Memset(m_OverlappedWrite, 0, sizeof(OVERLAPPED));

	COMMTIMEOUTS CommTimeOuts;
	//CommTimeOuts.ReadIntervalTimeout = 10;
	CommTimeOuts.ReadIntervalTimeout = 0xFFFFFFFF;
	CommTimeOuts.ReadTotalTimeoutMultiplier = 0;
	CommTimeOuts.ReadTotalTimeoutConstant = 0;
	CommTimeOuts.WriteTotalTimeoutMultiplier = 0;
	CommTimeOuts.WriteTotalTimeoutConstant = 10;
	SetCommTimeouts(m_hIDComDev, &CommTimeOuts);

	m_OverlappedRead->hEvent = CreateEvent(NULL, true, false, NULL);
	m_OverlappedWrite->hEvent = CreateEvent(NULL, true, false, NULL);

	dcb.DCBlength = sizeof(DCB);
	GetCommState(m_hIDComDev, &dcb);
	dcb.BaudRate = nBaud;
	dcb.ByteSize = 8;

	if (!SetCommState(m_hIDComDev, &dcb) ||
		!SetupComm(m_hIDComDev, 10000, 10000) ||
		m_OverlappedRead->hEvent == NULL ||
		m_OverlappedWrite->hEvent == NULL)
	{
		unsigned long dwError = GetLastError();
		if (m_OverlappedRead->hEvent != NULL) CloseHandle(m_OverlappedRead->hEvent);
		if (m_OverlappedWrite->hEvent != NULL) CloseHandle(m_OverlappedWrite->hEvent);
		CloseHandle(m_hIDComDev);
		m_hIDComDev = NULL;
		UE_LOG(LogTemp, Error, TEXT("Failed to setup port COM%d. Error: %08X"), nPort, dwError);
		return false;
	}

	//FPlatformProcess::Sleep(0.05f);
	AddToRoot();
	m_Port = nPort;
	m_Baud = nBaud;
	return true;
}

void USerial::Close()
{
	if (!m_hIDComDev) return;

	if (m_OverlappedRead->hEvent != NULL) CloseHandle(m_OverlappedRead->hEvent);
	if (m_OverlappedWrite->hEvent != NULL) CloseHandle(m_OverlappedWrite->hEvent);
	CloseHandle(m_hIDComDev);
	m_hIDComDev = NULL;

	RemoveFromRoot();
}

FString USerial::ReadString(bool &bSuccess)
{
	return ReadStringUntil(bSuccess, '\0');
}

FString USerial::Readln(bool &bSuccess)
{
	return ReadStringUntil(bSuccess, '\n');
}

FString USerial::ReadStringUntil(bool &bSuccess, uint8 Terminator)
{
	bSuccess = false;
	if (!m_hIDComDev) return TEXT("");

	TArray<uint8> Chars;
	uint8 Byte = 0x0;
	bool bReadStatus;
	unsigned long dwBytesRead, dwErrorFlags;
	COMSTAT ComStat;

	ClearCommError(m_hIDComDev, &dwErrorFlags, &ComStat);
	if (!ComStat.cbInQue) return TEXT("");

	do {
		bReadStatus = BOOL2bool(ReadFile(
			m_hIDComDev,
			&Byte,
			1,
			&dwBytesRead,
			m_OverlappedRead));

		if (!bReadStatus)
		{
			if (GetLastError() == ERROR_IO_PENDING)
			{
				WaitForSingleObject(m_OverlappedRead->hEvent, 2000);
			}
			else
			{
				Chars.Add(0x0);
				break;
			}
		}

		if (Byte == Terminator || dwBytesRead == 0)
		{
			// when Terminator is \n, we know we're expecting lines from Arduino. But those
			// are ended in \r\n. That means that if we found the line Terminator (\n), our previous
			// character could be \r. If it is, we remove that from the array.
			if (Chars.Num() > 0 && Terminator == '\n' && Chars.Top() == '\r') Chars.Pop(false);

			Chars.Add(0x0);
			break;
		}
		else Chars.Add(Byte);

	} while (Byte != 0x0 && Byte != Terminator);

	bSuccess = true;
	auto Convert = FUTF8ToTCHAR((ANSICHAR*)Chars.GetData());
	return FString(Convert.Get());
}

float USerial::ReadFloat(bool &bSuccess)
{
	bSuccess = false;

	TArray<uint8> Bytes = ReadBytes(4);
	if (Bytes.Num() == 0) return 0;

	bSuccess = true;
	return *(reinterpret_cast<float*>(Bytes.GetData()));
}

int32 USerial::ReadInt(bool &bSuccess)
{
	bSuccess = false;

	TArray<uint8> Bytes = ReadBytes(4);
	if (Bytes.Num() == 0) return 0;

	bSuccess = true;
	return *(reinterpret_cast<int32*>(Bytes.GetData()));
}

uint8 USerial::ReadByte(bool &bSuccess)
{
	bSuccess = false;
	if (!m_hIDComDev) return 0x0;

	uint8 Byte = 0x0;
	bool bReadStatus;
	unsigned long dwBytesRead, dwErrorFlags;
	COMSTAT ComStat;

	ClearCommError(m_hIDComDev, &dwErrorFlags, &ComStat);
	if (!ComStat.cbInQue) return 0x0;

	bReadStatus = BOOL2bool(ReadFile(
		m_hIDComDev,
		&Byte,
		1,
		&dwBytesRead,
		m_OverlappedRead));

	if (!bReadStatus)
	{
		if (GetLastError() == ERROR_IO_PENDING)
		{
			WaitForSingleObject(m_OverlappedRead->hEvent, 2000);
		}
		else
		{
			return 0x0;
		}
	}

	bSuccess = dwBytesRead > 0;
	return Byte;
}

TArray<uint8> USerial::ReadBytes(int32 Limit)
{
	TArray<uint8> Data;

	if (!m_hIDComDev) return Data;

	Data.Empty(Limit);

	uint8* Buffer = new uint8[Limit];
	bool bReadStatus;
	unsigned long dwBytesRead, dwErrorFlags;
	COMSTAT ComStat;

	ClearCommError(m_hIDComDev, &dwErrorFlags, &ComStat);
	if (!ComStat.cbInQue) return Data;

	bReadStatus = BOOL2bool(ReadFile(
		m_hIDComDev,
		Buffer,
		Limit,
		&dwBytesRead,
		m_OverlappedRead));

	if (!bReadStatus)
	{
		if (GetLastError() == ERROR_IO_PENDING)
		{
			WaitForSingleObject(m_OverlappedRead->hEvent, 2000);
		}
		else
		{
			return Data;
		}
	}

	Data.Append(Buffer, dwBytesRead);
	return Data;
}

bool USerial::Print(FString String)
{
	auto Convert = FTCHARToUTF8(*String);
	TArray<uint8> Data;
	Data.Append((uint8*)Convert.Get(), Convert.Length());

	return WriteBytes(Data);
}

bool USerial::Println(FString String)
{
	return Print(String + LineEndToStr(WriteLineEnd));
}

bool USerial::WriteFloat(float Value)
{
	TArray<uint8> Buffer;
	Buffer.Append(reinterpret_cast<uint8*>(&Value), 4);
	return WriteBytes(Buffer);
}

bool USerial::WriteInt(int32 Value)
{
	TArray<uint8> Buffer;
	Buffer.Append(reinterpret_cast<uint8*>(&Value), 4);
	return WriteBytes(Buffer);
}

bool USerial::WriteByte(uint8 Value)
{
	TArray<uint8> Buffer({ Value });
	return WriteBytes(Buffer);
}

bool USerial::WriteBytes(TArray<uint8> Buffer)
{
	if (!m_hIDComDev) false;

	bool bWriteStat;
	unsigned long dwBytesWritten;

	bWriteStat = BOOL2bool(WriteFile(m_hIDComDev, Buffer.GetData(), Buffer.Num(), &dwBytesWritten, m_OverlappedWrite));
	if (!bWriteStat && (GetLastError() == ERROR_IO_PENDING))
	{
		if (WaitForSingleObject(m_OverlappedWrite->hEvent, 1000))
		{
			dwBytesWritten = 0;
			return false;
		}
		else
		{
			GetOverlappedResult(m_hIDComDev, m_OverlappedWrite, &dwBytesWritten, false);
			m_OverlappedWrite->Offset += dwBytesWritten;
			return true;
		}
	}

	return true;
}

void USerial::Flush()
{
	if (!m_hIDComDev) return;

	TArray<uint8> Data;

	do {
		Data = ReadBytes(8192);
	} while (Data.Num() > 0);
}

FString USerial::LineEndToStr(ELineEnd LineEnd)
{
	switch (LineEnd)
	{
	case ELineEnd::rn:
		return TEXT("\r\n");
	case ELineEnd::n:
		return TEXT("\n");
	case ELineEnd::r:
		return TEXT("\r");
	case ELineEnd::nr:
		return TEXT("\n\r");
	default:
		return TEXT("null");
	}
}
