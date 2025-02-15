import tkinter as tk
from tkinter import ttk
import serial.tools.list_ports
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
import matplotlib.pyplot as plt
import math

class DataVisualizer:
    def __init__(self, root):
        self.root = root
        self.root.title("LiDAR Data Visualizer")
        
        self.figure, self.ax = plt.subplots(subplot_kw={'projection': 'polar'})
        self.ax.set_ylim(0, 1000)
        self.canvas = FigureCanvasTkAgg(self.figure, master=root)
        self.canvas.get_tk_widget().pack(fill=tk.BOTH, expand=True)
        
        self.serial_port = serial.Serial('/dev/ttyUSB0', 230400, timeout=0.5)
        
        self.canvas.mpl_connect("motion_notify_event", self.on_hover)
        
        self.annotation = self.ax.annotate("", xy=(0,0), xytext=(10,10),
                                           textcoords="offset points",
                                           bbox=dict(boxstyle="round", fc="w"),
                                           arrowprops=dict(arrowstyle="->"))
        self.annotation.set_visible(False)
        
        self.scatter = None
        self.data_dict = [None] * 36000
        
        self.update_plot()
    
    def update_plot(self):
        data = self.read_serial_data()
        if data:
            for angle, distance in data:
                self.data_dict[int(angle*100)] = distance

            filtered_data = [(i / 100, self.data_dict[i]) for i in range(len(self.data_dict)) if self.data_dict[i] is not None]
            print("Filetered data: ", filtered_data)
            if filtered_data:
                angles, distances = zip(*filtered_data)
                angles = [math.radians(angle) for angle in angles]
                self.ax.clear()
                self.ax.set_ylim(0, 1000) 
                self.scatter = self.ax.scatter(angles, distances)
                self.canvas.draw()
        
        self.root.after(100, self.update_plot)
    
    def read_serial_data(self):
        line = self.serial_port.readline().decode('utf-8').strip()
        
        if line and line.startswith('A'):      
            try:
                data = line[2:].split(',')
                formatted_data = [(float(data[i]), float(data[i+1])) for i in range(0, len(data), 2)]
                return formatted_data
            except Exception as e:
                print(f"Error parsing data: {e}")
                return None
            
        return None
    
    def on_hover(self, event):
        if event.inaxes == self.ax and self.scatter:
            cont, ind = self.scatter.contains(event)
            if cont:
                idx = ind["ind"][0]
                angle = self.scatter.get_offsets()[idx][0]
                distance = self.scatter.get_offsets()[idx][1]
                self.annotation.xy = (angle, distance)
                self.annotation.set_text(f"Angle: {math.degrees(angle):.2f}°, Distance: {distance:.2f} mm")
                self.annotation.set_visible(True)
                self.canvas.draw_idle()
            else:
                self.annotation.set_visible(False)
                self.canvas.draw_idle()

if __name__ == "__main__":
    root = tk.Tk()
    app = DataVisualizer(root)
    root.mainloop()