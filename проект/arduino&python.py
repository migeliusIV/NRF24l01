import time
import serial
import matplotlib.pyplot as plt
import matplotlib.animation as animation
from functools import partial

def anmt(i, dataList, serialCom):
    #print(serialCom)
    data_befdec = serialCom.readline()
    arduinoData_string = data_befdec.decode("utf-8").strip('\r\n')    # Decode receive Arduino data as a formatted string
    print(i)                                           # 'i' is a incrementing variable based upon frames = x argument

    try:
        '''data_befdec = serialCom.readline()
        arduinoData_string = data_befdec.decode("utf-8").strip('\r\n')   ''' # Decode receive Arduino data as a formatted string
        arduinoData_float = float(arduinoData_string)   # Convert to float
        dataList.append(arduinoData_float)              # Add to the list holding the fixed number of points to animate      
    except:                  # Pass if data point is bad                               
        print("Error encountered, line was not recorded.")  
        pass
        
    dataList = dataList[-50:]                           # Fix the list size so that the animation plot 'window' is x number of points
    
    ax.clear()                                          # Clear last data frame
    ax.plot(dataList)                                   # Plot new data frame
    
    ax.set_ylim([0, 700])                              # Set Y axis limit of plot
    ax.set_title("Robot 5")                        # Set title of figure
    ax.set_ylabel("Value")                              # Set title of y axis 

dataList = []                                           # Create empty list variable for later use
                                                        
fig = plt.figure()                                      # Create Matplotlib plots fig is the 'higher level' plot window
ax = fig.add_subplot(111)                               # Add subplot to main fig window

serialCom = serial.Serial("COM13", 9600)  # Establish Serial object with COM port and BAUD rate to match Arduino Port/rate
#print(serialCom)

time.sleep(2)                                           # Time delay for Arduino Serial initialization 

#Перезапуск ардуино
serialCom.setDTR(False)
time.sleep(1)
serialCom.flushInput()
serialCom.setDTR(True)
                                                        # Matplotlib Animation Fuction that takes takes care of real time plot.
                                                        # Note that 'fargs' parameter is where we pass in our dataList and Serial object. 
ani = animation.FuncAnimation(fig, func = partial(anmt, dataList = dataList, serialCom = serialCom), frames=100, interval=300) 

plt.show()                                              # Keep Matplotlib plot persistent on screen until it is closed
serialCom.close()  
