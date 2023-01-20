import time
import serial
import matplotlib.pyplot as plt
import matplotlib.animation as animation
from functools import partial

def anmt(i, dataList5, datalist1, serialCom5, serialCom1):
    data_befdec5 = serialCom5.readline()
    arduinoData_string5 = data_befdec5.decode("utf-8").strip('\r\n')    # Decode receive Arduino data as a formatted string
    print(arduinoData_string5)

    data_befdec1 = serialCom1.readline()
    arduinoData_string1 = data_befdec1.decode("utf-8").strip('\r\n')                                       

    try:
        arduinoData_float5 = float(arduinoData_string5)   # Convert to float
        dataList5.append(arduinoData_float5)               # Add to the list holding the fixed number of points to animate       
    except:                  # Pass if data point is bad                               
        print("Error encountered, line was not recorded.")  
        pass

    try:  
        arduinoData_float1 = float(arduinoData_string1)   # Convert to float
        datalist1.append(arduinoData_float1) 
    except:
        print("Error encountered, line was not recorded.")  
        pass
        
    dataList5 = dataList5[-50:]                           # Fix the list size so that the animation plot 'window' is x number of points
    datalist1 = datalist1[-50:]

    ax[0,0].clear()                                           # Clear last data frame
    ax[0,0].plot(dataList5)                                   # Plot new data frame
    ax[0,0].set_ylim([0, 1000])                              # Set Y axis limit of plot
    ax[0,0].set_title("Robot 5")                        # Set title of figure
    ax[0,0].set_ylabel("Value")                              # Set title of y axis 
    
    ax[0,1].clear() 
    ax[0,1].plot(datalist1)                                 # Plot new data frame'''
    ax[0,1].set_ylim([0, 1000])                             # Set Y axis limit of plot
    ax[0,1].set_title("Robot 1")                            # Set title of figure
    ax[0,1].set_ylabel("Value")  

    ax[1,0].set_ylim([0, 1000])                             # Set Y axis limit of plot
    ax[1,0].set_title("Robot 3")                            # Set title of figure
    ax[1,0].set_ylabel("Value")  

    ax[1,1].set_ylim([0, 1000])                             # Set Y axis limit of plot
    ax[1,1].set_title("Robot 4")                            # Set title of figure
    ax[1,1].set_ylabel("Value")  


dataList5 = []                                           # Create empty list variable for later use
datalist1 = []
'''fig = plt.figure()                                      # Create Matplotlib plots fig is the 'higher level' plot window
ax = fig.add_subplot(111)                               # Add subplot to main fig window'''
fig, ax = plt.subplots(2, 2)

serialCom5 = serial.Serial("COM13", 9600)  # Establish Serial object with COM port and BAUD rate to match Arduino Port/rate
serialCom1 = serial.Serial("COM21", 9600)
print(serialCom5)

time.sleep(2)                                           # Time delay for Arduino Serial initialization 
#Перезапуск ардуино
serialCom5.setDTR(False)
time.sleep(1)
serialCom5.flushInput()
serialCom5.setDTR(True)

serialCom1.setDTR(False)
time.sleep(1)
serialCom1.flushInput()
serialCom1.setDTR(True)
                                                        # Matplotlib Animation Fuction that takes takes care of real time plot.
                                                        # Note that 'fargs' parameter is where we pass in our dataList and Serial object. 
ani = animation.FuncAnimation(fig, func = partial(anmt, dataList5 = dataList5, datalist1 = datalist1, serialCom5 = serialCom5, serialCom1 = serialCom1), frames=100, interval=300) 

plt.show()                                              # Keep Matplotlib plot persistent on screen until it is closed
serialCom5.close()  
serialCom1.close()