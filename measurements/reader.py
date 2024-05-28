import serial

baudrate = input("Insert baudrate: ")
filename = input("Insert name of file: ")

s = serial.Serial('/dev/ttyUSB0', baudrate)
index = 0

try:
    file_to_write = open(filename, mode='w')
    while (index <= 1200):
        line = s.readline().decode().strip()
        print("Data:", line)
        
        file_to_write.write(f"{index},{line}\n")
        index += 1

except KeyboardInterrupt:
    print("Aborted.")

finally:
    s.close()

