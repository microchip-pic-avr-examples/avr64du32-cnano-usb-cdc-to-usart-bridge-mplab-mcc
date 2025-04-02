try:
    import sys
    import serial
    import argparse
    import serial.tools.list_ports
except ImportError:
    sys.exit("""ImportError: You are probably missing some modules. Try 'pip install pyserial'""")
    
    
# VID and PID for AVR DU and CNano Debugger, must match values set in MCC
DU_VID = 0x04D8
DU_PID = 0x0B16
DEBUGGER_VID = 0x03EB
DEBUGGER_PID = 0x2175

# Function for fetching virtual com-ports based on VID and PID
def GetSerialPort(vid, pid):
    serialPort = ""
    for port in serial.tools.list_ports.comports():
        if (port.vid == vid and port.pid == pid):
            serialPort = port.device
    return serialPort

def printHex(input):
    return ' '.join(f'{c:0>2X}' for c in input)

def main(serialPortNum, debugPortNum):
    # Gets default device and debugger serial ports if not given
    if "" == serialPortNum:
        serialPortNum = GetSerialPort(DU_VID, DU_PID)
    if "" == debugPortNum:
        debugPortNum = GetSerialPort(DEBUGGER_VID, DEBUGGER_PID)
        
    # Starts device serial communication if serial port exists, else throws error
    print("Opening serial communication...")
    if "" != serialPortNum:
        cdcSerial = serial.Serial(serialPortNum,115200,timeout=1)
    else:
        print("CDC Virtual Serial Port number error detected - Could not start serial communication")
        print("\t 1. Make sure both the DEBUGGER and TARGET port are connected to the computer")
        print("\t 2. Disconnect other AVR devices from the computer")
        sys.exit("Error occurred")
        
    # Starts debugger serial communication if serial port exists, else throws error
    if "" != debugPortNum:
        debugSerial = serial.Serial(debugPortNum,115200,timeout=1)
    else:
        print("Debugger Serial Port number error detected - Could not start serial communication")
        sys.exit("Error occurred")
    
    # An array containing the message "Welcome to the AVR DU experience! USB connectivity on a modern AVR." in decimal ASCII
    sentData = [87, 101, 108, 99, 111, 109, 101, 32, 116, 111, 32, 116, 104, 101, 32, 65, 86, 82, 32, 68, 85, 32, 101, 120, 112, 101, 114, 105, 101, 110, 99, 101, 33, 32, 85, 83, 66, 32, 99, 111, 110, 110, 101, 99, 116, 105, 118, 105, 116, 121, 32, 111, 110, 32, 97, 32, 109, 111, 100, 101, 114, 110, 32, 65, 86, 82, 46]
    sentDataSize = len(sentData)
    # Write on CDC
    cdcSerial.write(sentData)
    print("Transmitting Data USB->USART:")
    print(printHex(sentData))
    
    # Read on Debugger
    receivedData = debugSerial.read(sentDataSize)
    print("Received Data:")
    print(receivedData)
    
    # Write on Debugger
    debugSerial.write(sentData)
    print("Transmitting Data USART->USB:")
    print(printHex(sentData))
    
    # Read on CDC
    receivedData = cdcSerial.read(sentDataSize)
    print("Received Data:")
    print(receivedData)
    
    print("Closing the serial communication.")
    cdcSerial.close
    debugSerial.close

if __name__ == '__main__':
    # Generate help and use messages
    parser = argparse.ArgumentParser(
    description='USB CDC Python example script for CDC USART Bridge on AVR DU',
    epilog='Example: python usb_cdc_usart_bridge.py',
    formatter_class=argparse.RawTextHelpFormatter)
    parser.add_argument(
    '-s','--serialPortNum', help='device communication port, defaults to finding device based on VID and PID', default="")
    parser.add_argument(
    '-d','--debugPortNum', help='debugger communication port, defaults to finding device based on VID and PID', default="")
    args = parser.parse_args()
    
    main(args.serialPortNum, args.debugPortNum)
