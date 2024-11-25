# TransmissionLogger.py
# Isabelle Gaudet/Group 4
# 2024-11-23

import serial 
import pandas as pd
import matplotlib.pyplot as plt
import time
from datetime import date, datetime

# function to log data
def data_logging():
    ser = serial.Serial("COM3", 9600, timeout=1)
    # create empty list to store
    data = []
    
    #start=current time
    start_time = time.time()
    start_millisecond = time.time()*1000.0

    # Burn one line
    ser.readline()

    # while loop -read data for 60 seconds
    while time.time()-start_time<60:
        read = ser.readline().decode('utf-8').split(",")
        # print(read)
        # exit(0)
        hex = read[0].split()[1]
        intensity = int(read[1].split()[1])
        # if data read..
        if hex:
            adc = int(hex, 16)  # convert hex to int
            # intensity = adc/1023*100

            
            # timestamp=current date/time
            # timestamp=datetime.now().strftime('%Y-%m-%d %H:%M:%S')
            timestamp = (time.time()*1000.0-start_millisecond)/1000.0
            # intensity,adc value=split data
            
            # append line data to data list
            data.append([timestamp,intensity,adc])
            
            # print data to console
            print(f'Time: {timestamp}, Intensity: {intensity}, ADC: {adc}')
    ser.close()
        
    # creating dataframe 
    df=pd.DataFrame(data, columns=['Timestamp', 'Intensity','ADC'])

    # "The generated CSV file should be named as per your group name and have proper index numbers and column names."
    # save data to csv file
    df.to_csv('Group4_Data.csv', index=False)
    print("Data saved to Group4_Data.csv")

    # call data_plotting function
    data_plotting(df)

# function to plot data
def data_plotting(df):
    # create figure with 2 subplots side by side
    fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(12, 5))
    
    # First subplot - Intensity vs Time
    ax1.set_xlabel('Time (s)')
    ax1.set_ylabel('Intensity Level (%)')
    ax1.plot(df['Timestamp'], df['Intensity'], color='b')
    ax1.set_title('Intensity Level vs Time')
    ax1.set_ylim(0, 100)  # Set y-axis limits for intensity
    ax1.grid(True)
    
    # Second subplot - ADC vs Time
    ax2.set_xlabel('Time (s)')
    ax2.set_ylabel('ADC Reading')
    ax2.plot(df['Timestamp'], df['ADC'], color='r')
    ax2.set_title('ADC Reading vs Time')
    ax2.set_ylim(0, 1023)  # Set y-axis limits for ADC
    ax2.grid(True)
    
    # Adjust layout and add main title
    plt.suptitle('Sensor Readings Over Time')
    plt.tight_layout()
    plt.show()

if __name__ == '__main__':
    data_logging()
    # data_plotting(pd.read_csv('Group4_Data.csv'))
    