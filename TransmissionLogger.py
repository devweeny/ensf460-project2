# TransmissionLogger.py
# Isabelle Gaudet/Group 4
# 2024-11-23

import serial 
import pandas as pd
import matplotlib.pyplot as plt
import time
from datetime import datetime

# function to log data
def data_logging():
    ser = serial.Serial(#uart port, baudrate, 
                        timeout=1)
    # create empty list to store
    data = []
    
    #start=current time
    start_time = time.time()
    # while loop -read data for 60 seconds
    while time.time()-start_time<60:
        line = ser.readline().decode('utf-8').strip()
        # if data read..
        if line:
            
            # timestamp=current date/time
            timestamp=datetime.now().strftime('%Y-%m-%d %H:%M:%S')
            # intensity,adc value=split data
            intensity,adc=map(int,line.split(','))
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
    
# intensity levels (%) vs. time (s)
# ADC reading vs, time
# subplots

    df['Timestamp'] = pd.to_datetime(df['Timestamp'])
    df.set_index('Timestamp', inplace=True)
    
    # create figure and axis objects with subplots
    fig,ax1=plt.subplots()
    
    # time on x axis
    ax1.set_xlabel('Time (s)')
    # intensity on y axis
    ax1.set_ylabel('Intensity Level (%)', color='b')
    # plotting intensity vs. time
    ax1.plot(df.index, df['Intensity'], color='b')
    # setting y-axis label color - not required
    ax1.tick_params(axis='y', labelcolor='b')
    
    # create second set of y axes against the same x axis (time)
    ax2=ax1.twinx()
    # now ADC reading on y axis
    ax2.set_ylabel('ADC Reading', color='r')
    # plotting ADC reading vs. time
    ax2.plot(df.index, df['ADC'], color='r')
    # setting y-axis label color - not required again
    ax2.tick_params(axis='y', labelcolor='r')
    
    # visual adjustments + title
    fig.tight_layout()
    plt.title('Intensity Level (%) vs. Time (s) and ADC Reading vs. Time (s)')
    # displaying
    plt.show()

if __name__ == '__main__':
    data_logging()