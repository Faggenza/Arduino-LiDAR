import tkinter as tk
from tkinter import ttk
import serial
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
import matplotlib.pyplot as plt
import math

class DataVisualizer:
    def __init__(self, root):
        self.root = root
        self.root.title("LiDAR Data Visualizer")
        
        self.figure, self.ax = plt.subplots(subplot_kw={'projection': 'polar'})
        self.canvas = FigureCanvasTkAgg(self.figure, master=root)
        self.canvas.get_tk_widget().pack(fill=tk.BOTH, expand=True)
        
        self.serial_port = serial.Serial('/dev/ttyUSB0', 230400, timeout=None)
        
        self.canvas.mpl_connect("motion_notify_event", self.on_hover)
        
        self.update_plot()
    
    def update_plot(self):
        data = self.read_serial_data()
        if data:
            angles, distances = zip(*data)
            angles = [math.radians(angle) for angle in angles]
            self.ax.clear()
            self.scatter = self.ax.scatter(angles, distances)
            self.canvas.draw()
        
        self.root.after(100, self.update_plot)
    
    def read_serial_data(self):
        line = self.serial_port.readline().decode('utf-8').strip()
        if line:
            try:
                data = line.split(',')
                formatted_data = [(float(data[i]), float(data[i+1])) for i in range(0, len(data), 2)]
                print(formatted_data)
                return formatted_data
            except:
                return None
        return None
    
    def on_hover(self, event):
        if event.inaxes == self.ax:
            cont, ind = self.scatter.contains(event)
            if cont:
                idx = ind["ind"][0]
                angle = self.scatter.get_offsets()[idx][0]
                distance = self.scatter.get_offsets()[idx][1]
                self.ax.set_title(f"Angle: {math.degrees(angle):.2f}°, Distance: {distance:.2f}")
                self.canvas.draw_idle()

if __name__ == "__main__":
    root = tk.Tk()
    app = DataVisualizer(root)
    root.mainloop()