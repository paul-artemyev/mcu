import serial
import time
import matplotlib.pyplot as plt
import re
 
PORT = 'COM6'
BAUDRATE = 115200
 
temp_data = []
pres_data = []
hum_data = []
 
ser = serial.Serial(PORT, BAUDRATE, timeout=1)
time.sleep(2)


 
start = time.time()
while time.time() - start < 10:
    ser.write(b'temp\r\n')
    time.sleep(0.05)
    temp_resp = ser.read(ser.in_waiting).decode()
    
    ser.write(b'pres\r\n')
    time.sleep(0.05)
    pres_resp = ser.read(ser.in_waiting).decode()
    
    ser.write(b'hum\r\n')
    time.sleep(0.05)
    hum_resp = ser.read(ser.in_waiting).decode()
    
    temp = re.search(r'(\d+\.\d+)', temp_resp)
    pres = re.search(r'(\d+\.\d+)', pres_resp)
    hum = re.search(r'(\d+\.\d+)', hum_resp)
    
    t = float(temp.group(1)) if temp else 0
    p = float(pres.group(1))/2.5 if pres else 0
    h = float(hum.group(1)) if hum else 0
    
    temp_data.append(t)
    pres_data.append(p)
    hum_data.append(h)
    
    print(f"{t:.1f} C, {p:.0f} Pa, {h:.0f} %")
    time.sleep(1)
 
ser.close()
 
plt.figure(figsize=(10, 8))
 
plt.subplot(3, 1, 1)
plt.plot(temp_data, 'r-')
plt.ylabel('Temp (C)')
 
plt.subplot(3, 1, 2)
plt.plot(pres_data, 'b-')
plt.ylabel('Pressure (Pa)')
 
plt.subplot(3, 1, 3)
plt.plot(hum_data, 'g-')
plt.ylabel('Hum (%)')
plt.xlabel('Samples')
 
plt.show()